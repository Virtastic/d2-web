// 1.13c Storm+0x34AE0 -- open a new log file and register its node. Returns 1 and the id in *out, or 0.
#include "storm_logenv.h"
extern "C" uint32_t Storm_LogCreate_34ae0(const StormLogEnv& e, const char* name, uint32_t* out) {
    if (!out || !name || !*name) { *e.lastError = 0x57; e.setLastError(0x57); if (out) *out = 0; return 0; }
    *out = 0;
    const uint32_t h = e.create(name);
    if (h == 0xFFFFFFFFu) return 0;
    const uint32_t id = ++*e.nextId;
    *out = id;
    uint32_t bucket = 0;
    const uint32_t a = e.find(id, &bucket, 1);
    if (!a) { *out = 0; return 0; }
    uint32_t* n = e.node(a);
    n[2] = h; n[6] = 1; n[5] = 0;
    e.leave(stormLogLock(bucket));
    return 1;
}
