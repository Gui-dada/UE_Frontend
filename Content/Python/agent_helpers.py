"""Agent 自动化 helper 库。

LLM 生成的脚本只需 import 本模块并调用函数，不用碰底层 unreal API。
所有函数已实测可用（P5-A 探测阶段验证）。
"""
import unreal


def load_level(path: str) -> int:
    """加载关卡，返回加载后的 actor 数量。"""
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les.load_level(path)
    return len(eas.get_all_level_actors())


def count_actors() -> int:
    """返回当前关卡 actor 数量。"""
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    return len(eas.get_all_level_actors())


def spawn_cubes(positions) -> list:
    """在给定位置 spawn N 个 Cube（StaticMeshActor + BasicShapes/Cube）。

    positions: list of (x, y, z) 或 unreal.Vector
    返回: 每个 spawn 出来的 Actor 名字列表
    """
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    cube = unreal.load_object(None, "/Engine/BasicShapes/Cube.Cube")
    assert cube is not None, "Cube asset load failed: /Engine/BasicShapes/Cube.Cube"
    names = []
    for i, pos in enumerate(positions):
        if isinstance(pos, (tuple, list)):
            loc = unreal.Vector(float(pos[0]), float(pos[1]), float(pos[2]))
        else:
            loc = pos
        sma = eas.spawn_actor_from_class(
            unreal.StaticMeshActor,
            loc,
            unreal.Rotator(0, 0, 0),
        )
        assert sma is not None, f"spawn_actor_from_class failed at index {i}"
        sma.static_mesh_component.set_static_mesh(cube)
        sma.set_actor_label(f"Cube_{i + 1}")
        names.append(sma.get_name())
    return names


def create_blueprint(name: str, folder: str, parent_class=None) -> dict:
    """创建并保存蓝图资产。

    name: 蓝图名（如 "BP_PickupTemplate"）
    folder: 内容浏览器路径（如 "/Game/_AgentTest"）
    parent_class: 父类，默认 unreal.Actor
    返回: {"path": str, "saved": bool, "exists": bool}
    """
    parent = parent_class if parent_class is not None else unreal.Actor
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent)
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    bp = tools.create_asset(name, folder, unreal.Blueprint, factory)
    assert bp is not None, f"create_asset returned None for {folder}/{name}"
    saved = unreal.EditorAssetLibrary.save_loaded_asset(bp)
    full_path = f"{folder}/{name}"
    exists = unreal.EditorAssetLibrary.does_asset_exist(full_path)
    return {"path": full_path, "saved": bool(saved), "exists": bool(exists)}


def delete_asset(path: str) -> bool:
    """删除资产（清理测试用）。"""
    return bool(unreal.EditorAssetLibrary.delete_asset(path))