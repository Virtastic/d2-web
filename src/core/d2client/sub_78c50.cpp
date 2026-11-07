// 1.13c D2Client+0x78C50 (705 B, cdecl, no input): the skills-panel slot pass (the page-1 branch of 0x78F20).
//   unit = [0xBBFC] (0 = nothing), bfb8 = [0xBFB8], x/y from [0xDBC48] [0xDBC4C] [0x11B9A0] [0x11B9A4], page = [0x924F8],
//   ds = the D2Common data-table struct reached through the IAT slot at 0xCE474 (+0xB8C skill-desc rows 0x120 B with the page
//   byte at +2, +0xB94 their count, +0xB98 skill rows 0x23C B with the skill id word at +0, a dword at +0x170, the desc index
//   word at +0x194, +0xBA0 their count), a3d38/a3b38 = the per-slot arrays at 0x113D38 / 0x113B38 (written).
// Callees by RVA: 0xC250 (unit,5,0) the count drawn at the top, 0xC1FC (unit,0x36), 0xD37E (text), 0xCB38/0xCB3E (slot count / id),
//   0xCB44 (unit,id) usable, 0xC4EA (ECX=unit EDX=skill, -1) / 0xC718 (unit,c4,flag) skill lookup / level, 0x17B0 (EAX=skill id) max level,
//   0xCB4A (unit,id), 0xC316 (unit,req,id,level), 0x78820 (the slot draw).
#include <cstdint>
#include <cstdio>
#include <emscripten.h>
#include "d2client_callee.h"

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78c50(const D2cSvc& v, uint32_t unit, int32_t bfb8, int32_t b9a4, int32_t bc4c, int32_t bc48, int32_t b9a0,
                     uint32_t page, const uint8_t* ds, uint32_t* a3d38, uint32_t* a3b38) {
    if (!unit) return;
    const uint32_t inv = *(const uint32_t*)(uintptr_t)(unit + 4);
    uint16_t wbuf[10] = {};
    {
        char buf[16];
        std::snprintf(buf, sizeof buf, "%d", (int32_t)d2c_call(v, 0xc250, 0, 0, 0, 0, 0, unit, 5, 0));
        for (int i = 0; i < 8 && buf[i]; ++i) wbuf[i] = (uint8_t)buf[i];   // toUnicode(wbuf, buf, 9): the count includes the terminator
    }
    const bool on = d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 0x36) != 0 || bfb8 != 0;
    d2c_call(v, 0xd37e, 0, (uint32_t)(uintptr_t)wbuf, (uint32_t)(bc48 - b9a0 - 0x34), 0, 0, (uint32_t)(b9a4 + bc4c - 0x190), on ? 1u : 0u, 0);
    const int32_t v1 = (int32_t)d2c_call(v, 0xc250, 0, 0, 0, 0, 0, unit, 5, 0);
    auto rd = [&](uint32_t off) { return *(const uint32_t*)(ds + off); };
    for (int32_t i = 0; i < (int32_t)d2c_call(v, 0xcb38, 0, 0, 0, 0, 0, inv); ++i) {
        const int32_t id = (int32_t)d2c_call(v, 0xcb3e, 0, 0, 0, 0, 0, inv, (uint32_t)i);
        if (id < 0 || id >= (int32_t)rd(0xba0)) continue;
        const uint8_t* e = (const uint8_t*)(uintptr_t)rd(0xb98) + (uint32_t)id * 0x23c;
        const int32_t k = (int16_t)(e[0x194] | e[0x195] << 8);
        if (k < 0 || k >= (int32_t)rd(0xb94)) continue;
        const uint8_t* r = (const uint8_t*)(uintptr_t)rd(0xb8c) + (uint32_t)k * 0x120;
        if (page != (uint32_t)(int32_t)(int8_t)r[2]) continue;
        const uint32_t sk = (uint32_t)(int32_t)(int16_t)(e[0] | e[1] << 8);
        const uint32_t ep = (uint32_t)(uintptr_t)e;
        if (d2c_call(v, 0xcb44, 0, 0, 0, 0, 0, unit, (uint32_t)id)) {
            uint32_t ok = 1, lvl = 0;
            const uint32_t c4 = d2c_call(v, 0xc4ea, 0, unit, (uint32_t)id, 0, 0, 0xffffffffu);
            if (c4) lvl = d2c_call(v, 0xc718, 0, 0, 0, 0, 0, unit, c4, 0);
            if ((int32_t)lvl >= (int32_t)d2c_call(v, 0x17b0, sk, 0, 0, 0, 0)) ok = 0;
            if (d2c_call(v, 0xcb4a, 0, 0, 0, 0, 0, unit, (uint32_t)id) == 0) ok = 0;
            const uint32_t req = *(const uint32_t*)(e + 0x170);
            uint32_t t = 1;
            if (req != 0xffffffffu) t = d2c_call(v, 0xc316, 0, 0, 0, 0, 0, unit, req, (uint32_t)id, lvl);
            if (v1 < (int32_t)t || ok == 0) { a3d38[i] = 0xffffffffu; a3b38[i] = 5; }
            else if (a3d38[i] != 1) { a3d38[i] = 0; a3b38[i] = 0; }
            uint32_t a2 = 0;
            if (c4) a2 = d2c_call(v, 0xc718, 0, 0, 0, 0, 0, unit, c4, 1);
            d2c_call(v, 0x78820, 0, 0, 0, 0, 0, (uint32_t)id, ep, a2, a3d38[i], a3b38[i]);
        } else {
            a3d38[i] = 0xffffffffu; a3b38[i] = 5;
            uint32_t a2 = 0;
            const uint32_t c4 = d2c_call(v, 0xc4ea, 0, unit, sk, 0, 0, 0xffffffffu);
            if (c4) a2 = d2c_call(v, 0xc718, 0, 0, 0, 0, 0, unit, c4, 1);
            d2c_call(v, 0x78820, 0, 0, 0, 0, 0, (uint32_t)id, ep, a2, a3d38[i], a3b38[i]);
        }
    }
}
