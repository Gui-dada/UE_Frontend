"""蓝图资产操作 helper。

能力：
  get_blueprint_info(bp_path)             - 读蓝图名、父类短名、generated_class
  reparent_blueprint(bp_path, new_parent)  - 改父类 + 前后对比 + 保存 + 回滚提示

原理（UE 5.7 限制绕开）：
  Blueprint.ParentClass 是 BlueprintProtected，Python 反射读不到。
  但可以通过：
    bp → generated_class(bp) → BlueprintGeneratedClass
    → unreal.get_type_from_class(gen_class) → Python 包装类型
    → .__name__ → 父类短名（如 "GameModeBase"）
"""

import unreal


_COMMON_PARENTS = {
    "Actor": "/Script/Engine.Actor",
    "Pawn": "/Script/Engine.Pawn",
    "Character": "/Script/Engine.Character",
    "GameMode": "/Script/Engine.GameMode",
    "GameModeBase": "/Script/Engine.GameModeBase",
    "PlayerController": "/Script/Engine.PlayerController",
    "PlayerState": "/Script/Engine.PlayerState",
    "GameState": "/Script/Engine.GameState",
    "GameStateBase": "/Script/Engine.GameStateBase",
    "HUD": "/Script/Engine.HUD",
    "Info": "/Script/Engine.Info",
    "Controller": "/Script/Engine.Controller",
}


def _resolve_parent_class(class_input):
    """把字符串/Class 输入解析成 unreal.Class。

    支持：
      - "/Script/Engine.GameMode"（引擎类完整路径）
      - "/Game/.../BP_X.BP_X_C"（蓝图类路径）
      - "GameMode"（常见短名，查 _COMMON_PARENTS）
      - unreal.Class 对象（直接返回）
    """
    if isinstance(class_input, unreal.Class):
        return class_input

    if not isinstance(class_input, str):
        raise TypeError(
            f"class_input 必须是 str 或 unreal.Class，实际 {type(class_input).__name__}"
        )

    path = class_input.strip()

    if path in _COMMON_PARENTS:
        path = _COMMON_PARENTS[path]

    if not path.startswith("/"):
        path = f"/Script/Engine.{path}"

    cls = unreal.load_class(None, path)
    if cls is None:
        raise ValueError(f"无法加载类：{path}")
    return cls


def _load_blueprint(bp_path):
    """加载 Blueprint 资产，返回 Blueprint 对象或 None。"""
    if not unreal.EditorAssetLibrary.does_asset_exist(bp_path):
        return None
    return unreal.EditorAssetLibrary.load_asset(bp_path)


def _read_parent_class(bp_path):
    """读蓝图当前父类，返回 (short_name, full_path_hint)。

    short_name: "GameModeBase" / "Actor" 等（来自 Python 类型 __name__）
    full_path_hint: 如果能通过 _COMMON_PARENTS 反查到完整路径则返回，否则 None

    UE 5.7 限制：只能拿短名，拿不到完整路径。但短名足够判断"父类是否变了"。
    """
    try:
        bp = _load_blueprint(bp_path)
        if not bp:
            return None, None

        gen_class = None
        try:
            gen_class = unreal.BlueprintEditorLibrary.generated_class(bp)
        except Exception:
            pass

        if gen_class is None:
            try:
                gen_class = bp.generated_class()
            except Exception:
                pass

        if gen_class is None:
            return None, None

        py_type = unreal.get_type_from_class(gen_class)
        if py_type is None:
            return None, None

        short_name = py_type.__name__
        full_path = _COMMON_PARENTS.get(short_name)
        return short_name, full_path
    except Exception:
        return None, None


def get_blueprint_info(bp_path):
    """读取蓝图基本信息。

    返回：
      {
        "ok": bool,
        "path": str,
        "name": str | None,
        "parent_class": str | None,       # 父类短名，如 "GameModeBase"
        "parent_class_path": str | None,  # 完整路径（若短名在 _COMMON_PARENTS 里）
        "generated_class": str | None,
        "error": str | None,
      }
    """
    result = {
        "ok": False,
        "path": bp_path,
        "name": None,
        "parent_class": None,
        "parent_class_path": None,
        "generated_class": None,
        "error": None,
    }

    try:
        bp = _load_blueprint(bp_path)
        if bp is None:
            result["error"] = f"蓝图不存在：{bp_path}"
            return result

        result["name"] = str(bp.get_name())

        parent_name, parent_path = _read_parent_class(bp_path)
        result["parent_class"] = parent_name
        result["parent_class_path"] = parent_path

        try:
            gen = unreal.BlueprintEditorLibrary.generated_class(bp)
            if gen is None:
                gen = bp.generated_class()
            result["generated_class"] = str(gen.get_name()) if gen else None
        except Exception:
            pass

        result["ok"] = result["generated_class"] is not None
        return result
    except Exception as e:
        result["error"] = f"{type(e).__name__}: {e}"
        return result


def reparent_blueprint(bp_path, new_parent_class):
    """改变蓝图父类，改前读、改后读、对比确认。

    【警告】真实修改资产，会触发蓝图重新编译 + 保存 uasset。

    参数：
      bp_path: 蓝图资产路径
      new_parent_class: 新父类（短名 / 完整路径 / unreal.Class）

    返回：
      {
        "ok": bool,                             # 改后读到的父类 == 预期
        "path": str,
        "old_parent_class": str | None,         # 改前读到的父类短名
        "old_parent_class_path": str | None,
        "new_parent_class": str | None,         # 改后读到的父类短名
        "new_parent_class_path": str | None,
        "expected_parent_class": str | None,    # 预期新父类短名
        "error": str | None,
        "rollback_hint": str,
      }
    """
    result = {
        "ok": False,
        "path": bp_path,
        "old_parent_class": None,
        "old_parent_class_path": None,
        "new_parent_class": None,
        "new_parent_class_path": None,
        "expected_parent_class": None,
        "error": None,
        "rollback_hint": "",
    }

    try:
        bp = _load_blueprint(bp_path)
        if bp is None:
            result["error"] = f"蓝图不存在：{bp_path}"
            return result

        # 改前读
        old_name, old_path = _read_parent_class(bp_path)
        result["old_parent_class"] = old_name
        result["old_parent_class_path"] = old_path
        if old_path:
            result["rollback_hint"] = (
                f"agent_blueprint.reparent_blueprint({bp_path!r}, {old_path!r})"
            )
        elif old_name:
            result["rollback_hint"] = (
                f"agent_blueprint.reparent_blueprint({bp_path!r}, {old_name!r})"
            )
        else:
            result["rollback_hint"] = (
                "无法自动生成回滚命令（改前未读到父类），"
                f"回滚方法：git restore <{bp_path}.uasset>"
            )

        # 解析新父类
        try:
            new_cls = _resolve_parent_class(new_parent_class)
            result["expected_parent_class"] = str(new_cls.get_name())
        except Exception as e:
            result["error"] = f"新父类解析失败：{type(e).__name__}: {e}"
            return result

        # 如果改前父类 == 目标父类，直接成功（不重复改）
        if old_name == result["expected_parent_class"]:
            result["ok"] = True
            result["new_parent_class"] = old_name
            result["new_parent_class_path"] = old_path
            return result

        # 调用 reparent
        try:
            unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_cls)
        except Exception as e:
            result["error"] = f"reparent_blueprint 调用失败：{type(e).__name__}: {e}"
            return result

        # 保存
        try:
            unreal.EditorAssetLibrary.save_loaded_asset(bp, only_if_is_dirty=False)
        except Exception as e:
            result["error"] = f"reparent 已调用但保存失败：{type(e).__name__}: {e}"
            return result

        # 改后重新读（关键：验证真的生效了）
        new_name, new_path = _read_parent_class(bp_path)
        result["new_parent_class"] = new_name
        result["new_parent_class_path"] = new_path

        # 判定成功
        if new_name == result["expected_parent_class"]:
            result["ok"] = True
        else:
            result["error"] = (
                f"reparent 调用成功但父类未生效："
                f"old={old_name}, expected={result['expected_parent_class']}, "
                f"actual={new_name}"
            )

        return result
    except Exception as e:
        result["error"] = f"{type(e).__name__}: {e}"
        return result




def _resolve_component_class(input_obj):
    """把组件类输入解析成 unreal.Class。

    支持：
      - "PointLightComponent"（短名）
      - "/Script/Engine.PointLightComponent"（完整路径）
      - unreal.PointLightComponent（Python 类对象）
    """
    if isinstance(input_obj, unreal.Class):
        return input_obj
    if isinstance(input_obj, str):
        path = input_obj if input_obj.startswith("/") else f"/Script/Engine.{input_obj}"
        cls = unreal.load_class(None, path)
        if cls is None:
            raise ValueError(f"无法加载组件类：{path}")
        return cls
    if hasattr(input_obj, "static_class"):
        return input_obj.static_class()
    raise TypeError(f"不支持的组件类输入：{type(input_obj).__name__}")


def add_component(bp_path, component_class_input, parent_component_name=None):
    """给蓝图添加组件（挂在根组件或指定父组件下）。

    返回：
      {
        "ok": bool,
        "path": str,
        "component_class": str | None,       # 如 "PointLightComponent"
        "parent_handle_used": str | None,
        "error": str | None,
      }
    """
    result = {
        "ok": False,
        "path": bp_path,
        "component_class": None,
        "parent_handle_used": None,
        "error": None,
    }

    try:
        bp = _load_blueprint(bp_path)
        if not bp:
            result["error"] = f"蓝图不存在：{bp_path}"
            return result

        try:
            comp_cls = _resolve_component_class(component_class_input)
            result["component_class"] = str(comp_cls.get_name())
        except Exception as e:
            result["error"] = f"组件类解析失败：{type(e).__name__}: {e}"
            return result

        subsys = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
        if not subsys:
            result["error"] = "SubobjectDataSubsystem 不可用"
            return result

        handles = subsys.k2_gather_subobject_data_for_blueprint(bp)
        if not handles:
            result["error"] = "蓝图无子对象（可能未编译）"
            return result

        # 父 handle：默认根（handles[0]）
        parent_handle = handles[0]
        if parent_component_name:
            for h in handles:
                try:
                    data = subsys.get_data_from_handle(h)
                    for getter in ("get_display_name", "get_variable_name"):
                        fn = getattr(data, getter, None)
                        if callable(fn):
                            name = str(fn())
                            if parent_component_name in name:
                                parent_handle = h
                                break
                except Exception:
                    pass
        result["parent_handle_used"] = str(parent_handle)

        # 加组件
        params = unreal.AddNewSubobjectParams(
            parent_handle=parent_handle,
            new_class=comp_cls,
            blueprint_context=bp,
        )
        handle, fail_reason = subsys.add_new_subobject(params)
        if handle is None:
            result["error"] = f"add_new_subobject 失败：{fail_reason}"
            return result

        # 保存
        try:
            unreal.EditorAssetLibrary.save_loaded_asset(bp, only_if_is_dirty=False)
        except Exception as e:
            result["error"] = f"已添加但保存失败：{type(e).__name__}: {e}"
            return result

        result["ok"] = True
        return result
    except Exception as e:
        result["error"] = f"{type(e).__name__}: {e}"
        return result


def list_components(bp_path):
    """列出蓝图子对象。

    UE 5.7 限制：SubobjectData 是空 struct，读不到组件名/类。
    只能返回 handle 数量。判断"加组件是否成功"靠前后 count 对比。
    """
    result = {"ok": False, "path": bp_path, "count": 0, "error": None}
    try:
        bp = _load_blueprint(bp_path)
        if not bp:
            result["error"] = f"蓝图不存在：{bp_path}"
            return result
        subsys = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
        if not subsys:
            result["error"] = "SubobjectDataSubsystem 不可用"
            return result
        handles = subsys.k2_gather_subobject_data_for_blueprint(bp) or []
        result["count"] = len(handles)
        result["ok"] = True
        return result
    except Exception as e:
        result["error"] = f"{type(e).__name__}: {e}"
        return result
    """列出蓝图所有子对象（尽力而为，字段可能读不到）。"""
    result = {"ok": False, "path": bp_path, "count": 0, "components": [], "error": None}
    try:
        bp = _load_blueprint(bp_path)
        if not bp:
            result["error"] = f"蓝图不存在：{bp_path}"
            return result
        subsys = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
        handles = subsys.k2_gather_subobject_data_for_blueprint(bp) or []
        result["count"] = len(handles)

        for i, h in enumerate(handles):
            entry = {"index": i}
            try:
                data = subsys.get_data_from_handle(h)
                for getter in (
                    "get_display_name", "get_variable_name",
                    "get_object_display_name", "get_class_display_name",
                    "get_editor_property",
                ):
                    fn = getattr(data, getter, None)
                    if callable(fn):
                        try:
                            entry[getter] = str(fn())
                        except Exception:
                            pass
            except Exception as e:
                entry["err"] = f"{type(e).__name__}: {e}"
            result["components"].append(entry)

        result["ok"] = True
        return result
    except Exception as e:
        result["error"] = f"{type(e).__name__}: {e}"
        return result