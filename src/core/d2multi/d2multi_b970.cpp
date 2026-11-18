// D2Multi 0xB970 -- realm/character-list screen build. No arguments, no result (plain ret).
// Callees by thunk: 7CAA D2Win #10098 (stdcall, one word: the control template; returns the control), 7EA8 D2Lang #10003 (ECX = string id),
// 7C56 #10020, 7C38 #10031, 7CC8 #10202, 7C98 #10170, 7C32 #10010, 7CBC #10167 (ECX = list control, EDX = name), 7C44 #10185, 7C26 #10053,
// 7C80 #10021, 7CD4 #10037, 7C50 #10166 (returns a wide string or 0), 7C14 #10147. The name list is vtable [3917C]+0x20 (ECX = byte
// [39F14]+0x1EB & 0x20; returns a double-NUL list of names); in the capture that slot points at thunk 0x7BFC, which is how this port calls it.
// gv = the nine control globals 3A018..3A038 (gv[7], 3A034, is not written). ctx = [39F14]; u2w = Unicode::unicode2Win.
#include "d2multi_env.h"

static int32_t b970_strcmp(const uint8_t* a, const uint8_t* b) {        // the inlined two-bytes-a-step compare: 0 / -1 / 1
    for (;;) {
        if (a[0] != b[0]) return a[0] < b[0] ? -1 : 1;
        if (a[0] == 0) return 0;
        if (a[1] != b[1]) return a[1] < b[1] ? -1 : 1;
        if (a[1] == 0) return 0;
        a += 2; b += 2;
    }
}

// 1.13c D2Multi+0xB970
extern "C" void d2multi_b970(const D2mEnv& e, void (*u2w)(char*, const uint16_t*, int), uint32_t* gv, uint8_t* ctx) {
    gv[0] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a7b0));                    // 3A018
    gv[1] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a7e0));                    // 3A01C
    d2m_call(e, 0x7c56, 0, gv[1], d2m_call(e, 0x7ea8, 0, 0x148d));
    gv[2] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a810));                    // 3A020
    d2m_call(e, 0x7c56, 0, gv[2], d2m_call(e, 0x7ea8, 0, 0x148e));
    gv[3] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a840));                    // 3A024
    d2m_call(e, 0x7c56, 0, gv[3], d2m_call(e, 0x7ea8, 0, 0x148f));
    gv[4] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a900));                    // 3A028
    gv[5] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1aff0));                    // 3A02C
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, gv[5]);
    gv[6] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a870));                    // 3A030
    d2m_call(e, 0x7cc8, 0, gv[6], 0x20);
    d2m_call(e, 0x7c98, 0, gv[6], D2M_MOD(0x8f90));
    d2m_call(e, 0x7c32, 0, gv[6], D2M_MOD(0x8ac0));
    gv[8] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a8d0));                    // 3A038
    const uint8_t* list = (const uint8_t*)(uintptr_t)d2m_call(e, 0x7bfc, 0, ctx[0x1eb] & 0x20u);
    uint8_t* last = ctx + 0x37d;                                                     // the last-used name
    uint8_t* me = ctx + 0x35d;                                                       // the current name
    if (*last != 0) d2m_call(e, 0x7cbc, 0, gv[8], d2m_ptr(last));
    if (*me != 0 && b970_strcmp(last, me) != 0) d2m_call(e, 0x7cbc, 0, gv[8], d2m_ptr(me));
    for (const uint8_t* p = list; *p != 0; ) {
        if (*me == 0 || b970_strcmp(p, me) != 0) d2m_call(e, 0x7cbc, 0, gv[8], d2m_ptr(p));
        while (*p != 0) ++p;
        ++p;
    }
    d2m_call(e, 0x7c44, 0, gv[6], D2M_MOD(0x38d48));
    d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, gv[6]);
    d2m_call(e, 0x7c80, 0, gv[6]);
    d2m_call(e, 0x7cd4, 0, gv[8], 0);
    const uint32_t w = d2m_call(e, 0x7c50, 0, gv[6]);
    uint32_t has = 0;
    if (w != 0) {
        char buf[0x20];
        u2w(buf, (const uint16_t*)(uintptr_t)w, 0x1f);
        has = buf[0] != 0;
    }
    d2m_call(e, 0x7c14, 0, gv[4], has);
}
