// 1.13c D2Multi+0xB300: realm-UI init. ctx = [39f14]. gv: words 0..19 = [3912c..3917b] (word i = 0x3912c + 4i), 20 = [39f7c], 21 = [3a0d4].
// The Unicode constructors / win2Unicode / strcpy / strcat build text in stack buffers that nothing reads back (import stubs in the capture), so they are not modelled.
#include "d2multi_env.h"

extern "C" void d2multi_b300(const D2mEnv& e, uint32_t* gv, uint8_t* ctx) {
    for (int i = 3; i <= 19; i++) gv[i] = 0;
    static const struct { int idx; uint32_t rva; } ld[7] = { {9, 0x19e50}, {10, 0x19e80}, {11, 0x19eb0}, {12, 0x19ee0}, {13, 0x19f10}, {18, 0x1a000}, {19, 0x1a030} };
    for (const auto& l : ld) gv[l.idx] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(l.rva));
    d2m_call(e, 0x7c14, 0, gv[10], 0);
    gv[0] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a060));
    gv[1] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a090));
    gv[20] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a0c0));
    uint8_t arr[0x20] = {0}, fmt[0x20] = {0}, uni[0x20] = {0};
    const uint32_t flags = (uint32_t)ctx[0x1eb] | ((uint32_t)ctx[0x1ec] << 8);
    const uint8_t* title = (const uint8_t*)(uintptr_t)d2m_call(e, 0x7dd0, 0, 0, 0, 0, 0, ctx[0x1ea], (flags >> 8) & 0x1f, flags & 4, flags & 0x20);
    // retail copies the title (when #10205 answers one) into a stack buffer, appends the name at ctx+0xB9 (both NUL-terminated), and hands it to 0x7C5C in EDX
    uint32_t line[0x40] = {0};
    uint8_t* txt = (uint8_t*)line;
    uint32_t n = 0;
    if (title) for (; title[n] && n < 0xff; n++) txt[n] = title[n];
    for (uint32_t i = 0; ctx[0xb9 + i] && n < 0xff; i++) txt[n++] = ctx[0xb9 + i];
    d2m_call(e, 0x7c5c, 0, gv[20], (uint32_t)(uintptr_t)txt, 0, 0, (ctx[0x3c7] >> 2) & 1);
    if (e.blk) e.blk(e.ctx, 0x7c5c, line, (n + 1 + 3) / 4);   // hookblk: the composed line, len+1 bytes, the rest of the last dword zero
    d2m_call(e, 0x8d80, ctx[0x1ea], 0, 0, 0, (uint32_t)(uintptr_t)arr);
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)fmt, D2M_MOD(0x179b8), ctx[0x3bd] ? ctx[0x3bd] : 1);
    d2m_call(e, 0x7ea8, 0, 0xfd9);
    d2m_call(e, 0x7ce0, 0, gv[20], (uint32_t)(uintptr_t)uni);
    const uint32_t t1 = d2m_call(e, 0x7ea8, 0, 0x2b74);
    d2m_call(e, 0x7ce0, 0, gv[20], t1);
    const uint32_t q = ((flags >> 8) & 0x1f) / (((flags & 0x20) | 0x80) >> 5);
    const uint32_t id = q == 0 ? 0x2b75 : q == 1 ? 0x1424 : q == 2 ? 0x1423 : 0x1422;
    const uint32_t t2 = d2m_call(e, 0x7ea8, 0, id);
    d2m_call(e, 0x7ce0, 0, gv[20], t2);
    gv[21] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a180));
    if (gv[21] != 0) {
        const uint32_t t3 = d2m_call(e, 0x7ea8, 0, 0x2b73);
        d2m_call(e, 0x7ce0, 0, gv[21], t3, 0, 0, 3);
    }
}
