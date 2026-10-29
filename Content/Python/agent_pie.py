"""PIE validation helper v2.2 - 日志轮转兼容。

兼容原有接口：
  begin_play()        -> 只发启动请求，返回 baseline_size + log_path
  pie_status(...)     -> PIE 状态、Actor 断言、日志增量及结构化结果
  end_play()          -> 只发停止请求

v2.2 重点：
- 日志文件动态扫描（兼容编辑器重启后 UE 自动轮转到 MyProject02_2.log 等）
- begin_play 返回 log_path，编排层显式传给 pie_status，避免跨调用扫描不一致
- 日志采集状态与"没有错误日志"明确区分
"""
import glob
import os
import re
import time
import unreal

LOG_DIR = r"E:/CODING/ai_agent_FBend/UE_LearnFiles/Saved/Logs"
PRIMARY_LOG = os.path.join(LOG_DIR, "MyProject02.log")

NOISE_CATEGORIES = {
    "LogHttp",
    "LogDerivedDataCache",
    "LogZenServiceInstance",
    "LogOnline",
    "LogAudioMixer",
    "LogAudio",
    "LogSlate",
    "LogUObjectHash",
    "LogAutomationController",
}

_LOG_LEVEL_RE = re.compile(
    r"^(?:\[[^\]]+\]\[\s*\d+\]\s*)?(Log\w+):\s*(Error|Warning):\s*(.*)$"
)


def _latest_log_path():
    """返回 Saved/Logs 下最近的 MyProject02*.log（排除 backup）。

    UE 编辑器重启时会把当前日志轮转为 MyProject02_2.log / MyProject02_3.log 等，
    旧日志保留为 MyProject02.log 或 MyProject02-backup-*.log。
    """
    try:
        candidates = [
            p for p in glob.glob(os.path.join(LOG_DIR, "MyProject02*.log"))
            if "backup" not in os.path.basename(p).lower()
        ]
        if not candidates:
            return PRIMARY_LOG
        return max(candidates, key=lambda p: os.path.getmtime(p))
    except Exception:
        return PRIMARY_LOG


def _log_size(path=None):
    """返回指定日志文件大小；path 为 None 时取当前最新日志。"""
    path = path or _latest_log_path()
    try:
        return os.stat(path).st_size
    except OSError:
        return -1


def _read_log_delta(baseline_size, max_bytes=800_000, path=None):
    """读取 path 文件从 baseline_size 之后的内容。"""
    path = path or _latest_log_path()

    result = {
        "ok": False,
        "status": "READ_ERROR",
        "text": "",
        "baseline_size": baseline_size,
        "current_size": -1,
        "bytes_read": 0,
        "truncated": False,
        "error": None,
        "path": path,
    }
    if baseline_size is None or baseline_size < 0:
        result["status"] = "BASELINE_UNAVAILABLE"
        result["error"] = "测试开始前未能取得有效日志基线"
        return result
    try:
        current_size = os.stat(path).st_size
        result["current_size"] = current_size
        if current_size < baseline_size:
            start = 0
            result["truncated"] = True
            result["status"] = "FILE_TRUNCATED"
        elif current_size == baseline_size:
            result["ok"] = True
            result["status"] = "NO_NEW_DATA"
            return result
        else:
            start = max(baseline_size, current_size - max_bytes)
            if start > baseline_size:
                result["status"] = "DELTA_TRUNCATED_TO_MAX_BYTES"
        with open(path, "rb") as f:
            f.seek(start)
            raw = f.read(max_bytes)
        result["text"] = raw.decode("utf-8", errors="replace")
        result["bytes_read"] = len(raw)
        result["ok"] = True
        if result["status"] == "READ_ERROR":
            result["status"] = "OK"
        return result
    except Exception as exc:
        result["status"] = "READ_ERROR"
        result["error"] = f"{type(exc).__name__}: {exc}"
        return result


def _classify_line(line):
    m = _LOG_LEVEL_RE.match(line.strip())
    if not m:
        return None
    category, level, msg = m.group(1), m.group(2), m.group(3)
    if category in NOISE_CATEGORIES:
        return None
    return ("error" if level == "Error" else "warning", category, msg.strip())


def begin_play(duration_sec=0, startup_timeout_sec=0):
    """只发 PIE 启动请求 + 记 baseline + log_path，立即返回。

    PIE 启动是否成功由后续 pie_status() 的 in_pie 字段判断。
    """
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    if les.is_in_play_in_editor():
        les.editor_request_end_play()
        log_path = _latest_log_path()
        return {
            "requested": False,
            "started": None,
            "baseline_size": _log_size(log_path),
            "log_path": log_path,
            "reason": "已有 PIE，已发停止请求；请 5 秒后调用 pie_status 确认，再重跑 begin_play",
        }

    log_path = _latest_log_path()
    baseline = _log_size(log_path)
    les.editor_request_begin_play()

    return {
        "requested": True,
        "started": None,
        "baseline_size": baseline,
        "log_path": log_path,
        "note": "PIE 启动是异步的，请 5 秒后调用 pie_status() 检查 in_pie",
    }


def pie_status(
    expected_actor_count=None,
    min_actor_count=None,
    max_actor_count=None,
    max_errors=None,
    max_warnings=None,
    baseline_size=None,
    log_path=None,
    check_log=None,
    log_tail_lines=200,
):
    """PIE 状态 + Actor 断言 + 日志检查。

    log_path: begin_play 返回的日志文件路径；None 时取当前最新
    """
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)

    effective_log_path = log_path or _latest_log_path()

    if check_log is None:
        check_log = (
            baseline_size is not None
            or max_errors is not None
            or max_warnings is not None
        )

    result = {
        "in_pie": les.is_in_play_in_editor(),
        "world_name": None,
        "actor_count": 0,
        "actors": [],
        "errors": [],
        "warnings": [],
        "error_count": 0,
        "warning_count": 0,
        "log_check": {
            "enabled": bool(check_log),
            "ok": None if not check_log else False,
            "status": "SKIPPED" if not check_log else "NOT_RUN",
            "path": effective_log_path,
            "baseline_size": baseline_size,
            "current_size": None,
            "bytes_read": 0,
            "truncated": False,
            "tail": [],
            "error": None,
        },
        "assertions": [],
        "ok": True,
    }

    def _check_assertion(kind, expected, actual, ok, detail=None):
        item = {
            "kind": kind,
            "expected": expected,
            "actual": actual,
            "ok": bool(ok),
        }
        if detail is not None:
            item["detail"] = detail
        result["assertions"].append(item)
        if not ok:
            result["ok"] = False

    if not result["in_pie"]:
        _check_assertion("in_pie", True, False, False)
    else:
        world = ues.get_game_world()
        if world is None:
            _check_assertion("world", "not None", None, False)
        else:
            result["world_name"] = world.get_name()
            try:
                actors = unreal.GameplayStatics.get_all_actors_of_class(
                    world, unreal.Actor
                )
                result["actor_count"] = len(actors)
                result["actors"] = [str(a.get_name()) for a in list(actors)[:30]]
            except Exception as exc:
                _check_assertion(
                    "get_actors",
                    "success",
                    f"{type(exc).__name__}: {exc}",
                    False,
                )

    n = result["actor_count"]
    if expected_actor_count is not None:
        _check_assertion("expected_count", expected_actor_count, n,
                         n == expected_actor_count)
    if min_actor_count is not None:
        _check_assertion("min_count", f">= {min_actor_count}", n,
                         n >= min_actor_count)
    if max_actor_count is not None:
        _check_assertion("max_count", f"<= {max_actor_count}", n,
                         n <= max_actor_count)

    if check_log:
        log_result = _read_log_delta(baseline_size, path=effective_log_path)
        result["log_check"].update({
            "ok": log_result["ok"],
            "status": log_result["status"],
            "current_size": log_result["current_size"],
            "bytes_read": log_result["bytes_read"],
            "truncated": log_result["truncated"],
            "error": log_result["error"],
            "path": log_result["path"],
        })

        errs, warns = [], []
        for line in log_result["text"].splitlines():
            cls = _classify_line(line)
            if cls is None:
                continue
            kind, cat, msg = cls
            entry = {"category": cat, "message": msg}
            if kind == "error":
                errs.append(entry)
            else:
                warns.append(entry)

        result["error_count"] = len(errs)
        result["warning_count"] = len(warns)
        result["errors"] = errs[:50]
        result["warnings"] = warns[:30]
        tail_count = max(0, int(log_tail_lines))
        result["log_check"]["tail"] = (
            log_result["text"].splitlines()[-tail_count:]
            if tail_count else []
        )

        if not log_result["ok"]:
            _check_assertion(
                "log_capture",
                "readable log delta",
                log_result["status"],
                False,
                log_result["error"],
            )
        elif log_result["truncated"]:
            _check_assertion(
                "log_capture",
                "complete log delta",
                "FILE_TRUNCATED",
                False,
                "日志文件在测试期间缩小或被重建，日志覆盖范围不完整",
            )

        if max_errors is not None:
            _check_assertion("max_errors", f"<= {max_errors}",
                             len(errs), len(errs) <= max_errors)
        if max_warnings is not None:
            _check_assertion("max_warnings", f"<= {max_warnings}",
                             len(warns), len(warns) <= max_warnings)

    return result


def end_play(timeout_sec=0):
    """只发停止请求，立即返回。是否真正停止由后续 pie_status 判断。"""
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if not les.is_in_play_in_editor():
        return {"requested": False, "ended": True, "reason": "PIE 已经停止"}
    les.editor_request_end_play()
    return {
        "requested": True,
        "ended": None,
        "note": "PIE 停止是异步的，请 2-3 秒后调用 pie_status 检查 in_pie",
    }



def run_sequence(steps, world=None):
    """PIE 已启动后执行测试序列。

    steps 是列表，每项是一个 dict，支持的类型：

      {"type": "sleep", "sec": 1.5}
      {"type": "console", "cmd": "Stat FPS"}
      {"type": "spawn", "class_path": "/Script/Engine.TargetPoint"}
      {"type": "spawn", "class_path": "/Game/Blueprints/BP_Enemy.BP_Enemy_C"}
      {"type": "assert_actor_count", "min": 40}           # 或 expected / max
      {"type": "assert_actor_exists", "name_contains": "Enemy"}
      {"type": "assert_no_new_errors"}                    # 检查日志增量是否有新 Error

    返回：
      {
        "ok": bool,                    # 所有 step 是否通过
        "steps": [{index, type, ok, ...}],
        "final_actor_count": int,
        "final_actor_names": [str],
        "errors": [str],
      }
    """
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)

    if not les.is_in_play_in_editor():
        return {"ok": False, "error": "PIE 未运行，先调用 begin_play 并等待 5-6 秒"}

    if world is None:
        world = ues.get_game_world()
    if world is None:
        return {"ok": False, "error": "PIE World 不可用（get_game_world 返回 None）"}

    def _actor_count():
        return len(unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor))

    def _actor_names(limit=None):
        actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
        names = [str(a.get_name()) for a in actors]
        return names[:limit] if limit else names

    results = []
    errors = []
    ok_all = True

    for idx, step in enumerate(steps):
        stype = step.get("type", "")
        step_result = {"index": idx, "type": stype, "ok": True}

        try:
            if stype == "sleep":
                sec = max(0.0, float(step.get("sec", 1.0)))
                time.sleep(sec)
                step_result["detail"] = f"slept {sec}s"

            elif stype == "console":
                cmd = str(step["cmd"])
                unreal.SystemLibrary.execute_console_command(world, cmd)
                step_result["detail"] = f"sent: {cmd}"

            elif stype == "spawn":
                class_path = str(step["class_path"])
                # Summon 是 UE 的 spawn 命令；类路径格式：
                #   引擎类：/Script/Engine.TargetPoint
                #   蓝图类：/Game/Path/BP_X.BP_X_C
                cmd = "Summon " + class_path
                before_n = _actor_count()
                unreal.SystemLibrary.execute_console_command(world, cmd)
                time.sleep(0.5)  # Summon 是异步的，等 spawn 生效
                after_n = _actor_count()
                step_result["detail"] = (
                    f"Summon {class_path}: count {before_n} -> {after_n}"
                )
                # 不强制 delta > 0（有些 Summon 可能因位置/条件失败，让后续 assert 判断）

            elif stype == "assert_actor_count":
                n = _actor_count()
                checks = []
                ok = True
                if "expected" in step:
                    checks.append(("expected", step["expected"]))
                    ok = ok and n == step["expected"]
                if "min" in step:
                    checks.append(("min", step["min"]))
                    ok = ok and n >= step["min"]
                if "max" in step:
                    checks.append(("max", step["max"]))
                    ok = ok and n <= step["max"]
                step_result["actual"] = n
                step_result["expected"] = dict(checks)
                step_result["ok"] = ok
                if not ok:
                    errors.append(
                        f"step {idx} assert_actor_count: expected {dict(checks)}, got {n}"
                    )
                    ok_all = False

            elif stype == "assert_actor_exists":
                substr = str(step["name_contains"])
                names = _actor_names()
                found = any(substr in n for n in names)
                step_result["found"] = found
                step_result["ok"] = found
                step_result["sample"] = names[:10]
                if not found:
                    errors.append(
                        f"step {idx} assert_actor_exists: no actor name contains '{substr}' "
                        f"(sample: {names[:5]})"
                    )
                    ok_all = False

            elif stype == "assert_no_new_errors":
                # 用 begin_play 记录的 baseline 比较；这里通过 agent_pie 的 helper 读取
                baseline = step.get("baseline_size")
                log_path = step.get("log_path")
                if baseline is None:
                    step_result["ok"] = False
                    step_result["error"] = "baseline_size 未提供"
                    errors.append(f"step {idx} assert_no_new_errors: baseline_size missing")
                    ok_all = False
                else:
                    delta = _read_log_delta(baseline, path=log_path)
                    errs = []
                    for line in delta.get("text", "").splitlines():
                        cls = _classify_line(line)
                        if cls and cls[0] == "error":
                            errs.append({"category": cls[1], "message": cls[2]})
                    step_result["error_count"] = len(errs)
                    step_result["errors_sample"] = errs[:5]
                    step_result["ok"] = (len(errs) == 0)
                    if errs:
                        errors.append(
                            f"step {idx} assert_no_new_errors: {len(errs)} new error(s) in log"
                        )
                        ok_all = False

            else:
                step_result["ok"] = False
                step_result["error"] = f"unknown step type: {stype}"
                ok_all = False
                errors.append(f"step {idx}: unknown type '{stype}'")

        except Exception as exc:
            step_result["ok"] = False
            step_result["error"] = f"{type(exc).__name__}: {exc}"
            errors.append(f"step {idx} ({stype}) raised: {type(exc).__name__}: {exc}")
            ok_all = False

        results.append(step_result)

        # 一旦失败就停（避免后续断言依赖失败的前置操作）
        if not step_result["ok"]:
            break

    return {
        "ok": ok_all,
        "steps": results,
        "final_actor_count": _actor_count(),
        "final_actor_names": _actor_names(limit=20),
        "errors": errors,
    }