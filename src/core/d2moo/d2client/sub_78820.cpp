// 1.13c D2Client+0x78820 (638 B): the stat/count label beside a panel slot. stdcall (ret 0x14), no register input.
//   a0 = slot id, a1 = a struct whose byte +5 is tested against the byte [gdwBitMasks+8], a2 = the count, a3 = the state
//   (1 = frame + 1, -1 = no row), a4 = the draw mode (forced to 3 under the mouse, to 1 when the mask test fails).
// Globals: cls = byte [0x11991C], celTbl = the 0x22-stride cell table at 0xF4970 (dword per class = the cell), mouseX/mouseY =
// [0x11B828]/[0x11B824], unit = [0x11BBFC], bm8 = byte [gdwBitMasks + 8] (IAT data slot 0xCEE50).
// Callees (D2cSvc, keyed by RVA): 0x17F0 (EAX = a0, returns the slot record), 0x77CA0 (EAX = record ptr with its low byte
// := rec[3], returns y), 0x77C50 (EAX = y with its low byte := rec[4], returns x), 0xC4EA (1 stack word: -1) / 0xC718 (unit, c4, 0) / 0xCB32 (D2Common thunks),
// 0xD0E4 (cell draw, 6 stack words, blk[0] = frame, blk[13] = cell), 0x33550/0x33B50 (hover tooltips), 0xD3A8 (font), 0xD37E (text).
#include <cstdint>
#include <cstdio>
#include <emscripten.h>
#include "d2client_callee.h"

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78820(const D2cSvc& v, uint32_t a0, const uint8_t* a1, int32_t a2, int32_t a3, int32_t a4,
                     uint8_t cls, const uint8_t* celTbl, int32_t mouseX, int32_t mouseY, uint32_t unit, uint8_t bm8) {
    uint32_t esi = d2c_call(v, 0x17f0, a0, 0, 0, 0, 0);
    if (!esi) return;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)esi;
    uint32_t ebp = d2c_call(v, 0x77ca0, (esi & ~0xffu) | rec[3], 0, 0, 0, 0);
    uint32_t ebx = d2c_call(v, 0x77c50, (ebp & ~0xffu) | rec[4], 0, 0, 0, 0);
    uint32_t cel = *(const uint32_t*)(celTbl + (uint32_t)cls * 0x22);
    uint32_t frame = rec[7];
    if (a3 == 1) frame++;
    const bool hov = mouseX > (int32_t)ebx && mouseX < (int32_t)ebx + 0x30 && mouseY > (int32_t)ebp - 0x30 && mouseY < (int32_t)ebp;
    if (hov) a4 = 3;
    if (!(bm8 & a1[5])) a4 = 1;
    uint32_t c4 = d2c_call(v, 0xc4ea, 0, unit, a0, 0, 0, 0xffffffffu);
    uint32_t r718 = d2c_call(v, 0xc718, 0, 0, 0, 0, 0, unit, c4, 0);
    uint32_t cb = d2c_call(v, 0xcb32, 0, 0, 0, 0, 0, unit, a0);
    if (r718 == 0 && cb != 0 && a3 != -1) a4 = 3;
    uint32_t blk[18] = {};
    blk[0] = frame; blk[13] = cel; blk[16] = 0;
    d2c_call(v, 0xd0e4, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, ebx, ebp, 0xffffffffu, 5, (uint32_t)a4);
    if (hov) {
        if (a3 == -1) d2c_call(v, 0x33550, 0, 0, 0, 0, 0, unit, a0, (uint32_t)a2, 0, ebx, ebp);
        else d2c_call(v, 0x33b50, 0, a0, 0, 0, 0, unit, (uint32_t)a2, 0, ebx, ebp);
    }
    if (r718 || a2) {
        char buf[16]; uint16_t wbuf[10] = {};
        std::snprintf(buf, sizeof buf, "%d", a2);
        for (int i = 0; i < 8 && buf[i]; ++i) wbuf[i] = (uint8_t)buf[i];   // toUnicode(wbuf, buf, 9): the count includes the terminator
        if (a2 > 9) { d2c_call(v, 0xd3a8, 0, 4, 0, 0, 0); ebx -= 4; }
        else d2c_call(v, 0xd3a8, 0, 1, 0, 0, 0);
        int32_t cbv = (int32_t)d2c_call(v, 0xcb32, 0, 0, 0, 0, 0, unit, a0);
        uint32_t col = cbv > 0 ? 3 : cbv < 0 ? 1 : 0;
        d2c_call(v, 0xd37e, 0, (uint32_t)(uintptr_t)wbuf, ebx + 0x30, 0, 0, ebp + 0xc, col, 0);
    }
}
