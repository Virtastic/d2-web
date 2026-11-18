// 1.13c D2MULTI+0x13DE0: no arguments. Reads the widget handles d04, d08, d1c, d20, d24, d3c, d40, d44 (g[0..7] in that order) and sets the
// enabled/visible state of the controls for the mode Ordinal_10003(d40) reports: 7C20 (ECX = widget, EDX = 0/1), 7C14, then the two captions
// GetStringByIndex(0x148B)/(0x148C) placed on d04/d08 through 7CE0(ECX = widget, EDX = text, 4 or 5, 0, 0); finishes by tail-calling 0x13AF0.
#include "d2multi_env.h"

void d2multi_13de0(const D2mEnv& e, const uint32_t* g) {
    const uint32_t d04 = g[0], d08 = g[1], d1c = g[2], d20 = g[3], d24 = g[4], d3c = g[5], d40 = g[6], d44 = g[7];
    uint32_t on = d2m_call(e, 0x7cda, 0, d40) != 0;
    uint32_t a = on ? 0 : 1, b = on ? 1 : 0;
    uint32_t mode = on ? 4 : 5;
    d2m_call(e, 0x7c20, 0, d44, a);
    d2m_call(e, 0x7c20, 0, d24, a);
    d2m_call(e, 0x7c20, 0, d1c, b);
    d2m_call(e, 0x7c20, 0, d20, b);
    d2m_call(e, 0x7c14, 0, d3c, b);
    d2m_call(e, 0x7c14, 0, d1c, b);
    d2m_call(e, 0x7c14, 0, d20, b);
    uint32_t t1 = d2m_call(e, 0x7ea8, 0, 0x148b);
    d2m_call(e, 0x7ce0, 0, d04, t1, 0, 0, mode, 0, 0);
    uint32_t t2 = d2m_call(e, 0x7ea8, 0, 0x148c);
    d2m_call(e, 0x7ce0, 0, d08, t2, 0, 0, mode, 0, 0);
    d2m_call(e, 0x13af0);
}
