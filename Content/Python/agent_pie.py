"""PIE validation helper. Multi-step calls:
  begin_play()  -> request PIE start (async)
  pie_status()  -> current PIE state + Actor list from Game World
  end_play()    -> request PIE stop
"""
import unreal


def begin_play():
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.editor_request_begin_play()
    return {"requested": True}


def pie_status():
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    in_pie = les.is_in_play_in_editor()
    result = {"in_pie": in_pie, "world_name": None, "actor_count": 0, "actors": []}
    if not in_pie:
        return result
    world = ues.get_game_world()
    if world is None:
        return result
    result["world_name"] = world.get_name()
    try:
        actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
        result["actor_count"] = len(actors)
        result["actors"] = [str(a.get_name()) for a in list(actors)[:30]]
    except Exception as e:
        result["error"] = f"{type(e).__name__}: {e}"
    return result


def end_play():
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.editor_request_end_play()
    return {"requested": True}
