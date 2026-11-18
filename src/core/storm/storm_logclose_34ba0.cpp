// 1.13c Storm+0x34BA0 -- close one log by id.
#include "storm_logenv.h"
extern "C" void Storm_LogClose_34ba0(const StormLogEnv& e, uint32_t id) {
    if (!id) return;
    const uint32_t b = id & 3, lock = stormLogLock(b);
    e.enter(lock);
    uint32_t a = e.heads[b];
    while (a && e.node(a)[0] != id) a = e.node(a)[1];
    if (!a) { e.leave(lock); return; }
    uint32_t* n = e.node(a);
    if (n[3]) { e.write(n[2], a + 0x1c, n[3]); e.flush(n[2]); n[3] = 0; n[4] = 0; }
    e.close(n[2]);
    e.release(b, a);   // 0x346E0 unlinks, frees the node and leaves the bucket lock
}
