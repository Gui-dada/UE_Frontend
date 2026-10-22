"""UE 5.7 read-only query extension for the existing agent_helpers module.

Installation: copy this file into the same UE Python scripts directory as
agent_helpers.py. In a Remote Execution script, run `import agent_helpers_extension`
first; the extension attaches `find_actors_near_location` to agent_helpers.
No actor/asset mutation APIs are called here.
"""
from __future__ import annotations

import math
import unreal
import agent_helpers


def find_actors_near_location(location=(0.0, 0.0, 0.0), tolerance=50.0):
    """Return JSON-friendly Actor count and matches near an XYZ location.

    Distance is Euclidean in UE world units. The function only reads editor state.
    """
    if len(location) != 3:
        raise ValueError("location must contain exactly (x, y, z)")
    target = tuple(float(v) for v in location)
    tolerance = float(tolerance)
    if tolerance < 0:
        raise ValueError("tolerance must be >= 0")

    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = subsystem.get_all_level_actors()
    matches = []
    tolerance_sq = tolerance * tolerance

    for actor in actors:
        loc = actor.get_actor_location()
        xyz = (float(loc.x), float(loc.y), float(loc.z))
        dx, dy, dz = xyz[0] - target[0], xyz[1] - target[1], xyz[2] - target[2]
        distance_sq = dx * dx + dy * dy + dz * dz
        if distance_sq <= tolerance_sq:
            matches.append({
                "name": str(actor.get_name()),
                "location": {"x": xyz[0], "y": xyz[1], "z": xyz[2]},
                "distance": math.sqrt(distance_sq),
            })

    result = {
        "success": True,
        "count": len(actors),
        "matches": matches,
        "match_count": len(matches),
        "target": {"x": target[0], "y": target[1], "z": target[2]},
        "tolerance": tolerance,
    }
    # Attach for subsequent scripts that import agent_helpers directly.
    agent_helpers.find_actors_near_location = find_actors_near_location
    return result


# Attach on import so callers can use agent_helpers.find_actors_near_location(...).
agent_helpers.find_actors_near_location = find_actors_near_location
