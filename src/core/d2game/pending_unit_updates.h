#pragma once

namespace d2game {
// 1.13c D2Game+0x9D850. The environment owns the pointer representation
// and the callee bindings; traversal never converts a retail address to a native pointer.
template<class Environment>
void updateQueuedUnits(Environment& env)
{
    for (int i = 0; i < 5; ++i) {
        auto act = env.act(i);
        if (!act || !env.pending(act)) continue;
        for (auto room = env.firstRoom(act); room; room = env.nextRoom(room)) {
            for (auto unit = env.firstUnit(room); unit; unit = env.nextUnit(unit))
                env.finishUnit(unit);
            env.clearQueue(room);
        }
        env.clearPending(act);
    }
    env.clearArenaFlag();
}
}
