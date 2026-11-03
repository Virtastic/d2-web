// 1.13c D2Multi+0x121B0: EAX = string; returns the pointer one past its terminating NUL. A null string is fatal (Ordinal_10265(0xad), Ordinal_10024, _exit(-1)).
#include "d2multi_env.h"

uint8_t* d2multi_121b0(const D2mEnv& e, uint8_t* p) {
    if (!p) {
        uint32_t m = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0xad);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), m);
        d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    while (*p) p++;
    return p + 1;
}
