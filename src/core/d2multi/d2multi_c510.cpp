// superseded port of D2Multi 0xC510 (fastcall: ECX = unix time, EDX = flag): formats the time as the local date text and queues three log lines. Returns 1.
// flag == 0 reports a D2Lang string and ends the process (_exit(-1)); the rule makes those calls and returns.
#include "d2multi_env.h"

extern "C" uint32_t d2multi_c510(const D2mEnv& e, uint32_t t, uint32_t flag) {
    if (flag == 0) {
        uint32_t s = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0xc9e);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), s);
        d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    if (t != 0) {
        const uint32_t lo = t + 0xb6109100u;
        const uint32_t hi = 2u + (lo < t ? 1u : 0u);   // adc eax, 2: the carry of the low add
        d2m_call(e, 0x30e0, 0, 0, 0, 0, 0, lo, hi, 10000000u, 0);   // __allmul (t + 11644473600 s) * 10^7; FileTimeToLocalFileTime is the host's
        uint32_t ft[2] = { 0, 0 };
        char txt[0x40] = { 0 };
        d2m_call(e, 0xb160, 0, 0, 0, d2m_ptr(txt), 0, d2m_ptr(ft), 0x40);
        d2m_call(e, 0xb2a0, 0, 0, 0, 0, 0); d2m_edi(e, 0xb2a0, d2m_ptr(txt));
        d2m_call(e, 0xb2a0, 0, 4, 0, 0, 0); d2m_edi(e, 0xb2a0, flag);
        d2m_call(e, 0xb2a0, 0, 0, 0, 0, 0); d2m_edi(e, 0xb2a0, D2M_MOD(0x175d7));
    }
    return 1;
}
