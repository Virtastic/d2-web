// 1.13c D2Multi+0xD9D0: client table [3917c] slot +0x68 decides. Zero: string 0x14e7 (D2Lang) to the text rule ACB0 with callback 8A40.
// Non-zero: slot +0xac, C490, D770, the Storm query 7BAE (5 stack words: "Diablo II", "Default Channel", 0, a 32-byte out buffer, 0x20),
// then C690 with ECX = the buffer or, if the query failed, the default name at 175D7. Stdcall ret 4, the argument is unused; returns 1.
// vt: the client table behind [3917c] (each slot word carries the callee RVA in its low 24 bits).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_d9d0(const D2mEnv& e, const uint32_t* vt) {
    const uint32_t r = d2m_call(e, vt[0x68 / 4] & 0xFFFFFF);
    if (!r) {
        const uint32_t s = d2m_call(e, 0x7ea8, 0, 0x14e7);
        d2m_call(e, 0xacb0, 0, 0, 0, D2M_MOD(0x8a40), 0, s);
        return 1;
    }
    d2m_call(e, vt[0xac / 4] & 0xFFFFFF);
    d2m_call(e, 0xc490);
    d2m_call(e, 0xd770);
    uint8_t buf[32];
    const uint32_t w[5] = { D2M_MOD(0x174ec), D2M_MOD(0x179d4), 0, d2m_ptr(buf), 0x20 };
    const uint32_t q = d2m_call(e, 0x7bae, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3]);
    d2m_x(e, 0x7bae, &w[4], 1);
    d2m_call(e, 0xc690, 0, q ? d2m_ptr(buf) : D2M_MOD(0x175d7));
    return 1;
}
