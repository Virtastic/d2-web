// 1.13c Storm+0x35170 -- shut the log module down: flush, close every log under its bucket lock, delete the locks.
#include "storm_logenv.h"
extern "C" void Storm_LogShutdown_35170(const StormLogEnv& e) {
    e.flushAll();
    for (uint32_t b = 0; b < 4; ++b) {
        e.enter(stormLogLock(b));
        for (uint32_t a = e.heads[b]; a; a = e.heads[b]) {
            e.closeLog(e.node(a)[0]);
            e.purge(0x46A70, 0);
        }
        e.leave(stormLogLock(b));
    }
    for (uint32_t b = 0; b < 4; ++b) e.deleteLock(stormLogLock(b));
}
