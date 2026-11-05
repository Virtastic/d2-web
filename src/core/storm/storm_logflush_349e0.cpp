// 1.13c Storm+0x349E0 -- flush every log node. Replaces the 3b1e0-batch port.
#include "storm_logenv.h"
extern "C" void Storm_LogFlushAll_349e0(const StormLogEnv& e) {
    for (uint32_t b = 0; b < 4; ++b) {
        e.enter(stormLogLock(b));
        for (uint32_t a = e.heads[b]; a; a = e.node(a)[1]) {
            uint32_t* n = e.node(a);
            if (n[3]) {
                e.write(n[2], a + 0x1c, n[3]);
                e.flush(n[2]);
                n[3] = 0; n[4] = 0;
            }
        }
        e.leave(stormLogLock(b));
    }
}
