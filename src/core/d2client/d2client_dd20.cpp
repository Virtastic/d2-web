// 1.13c D2Client+0xDD20: tear the message-source list down. Under the lock, while the head at 0x123400 is a positive word:
// 0x2FF0(head) unlinks it, the free thunk 0xBE6C(head, file string, -2, 0) releases it. Then the lock is left and deleted (host
// no-ops) and the flag at 0x11C56C cleared. EAX is the lock delete's 0.
#include "d2c_seam.h"
extern "C" uint32_t d2c_dd20(const D2cSeam* s, const uint32_t* head, uint32_t* flag) {
    for (int32_t n; (n = (int32_t)*head) > 0;) {
        (*s)(0x2ff0, 0, 0, 0, 0, 0, (uint32_t)n);
        (*s)(0xbe6c, 0, 0, 0, 0, 0, (uint32_t)n, s->mod(0xfa408), 0xfffffffeu, 0);
    }
    *flag = 0;
    return 0;
}
