// 1.13c D2Win+0x8C60 -- list-box (type 4) mouse hit-test: maps the mouse Y to a row index (p[0x18]).
#include <cstdint>
#include <emscripten.h>

struct D2Win8C60Env {
    const uint32_t* bitMasks;             // gdwBitMasks
    int32_t mouseOfs;                     // 0x6F90148C
    const int32_t* curFont;               // 0x6F8FFD34
    const uint8_t* const* fontTab;        // 0x6F901518, stride 5 dwords; byte +10 = line height
    void (*setFont)(int32_t);             // 0x12FE0
    void (*sub8b50)(int32_t font);        // 0x8B50, EAX = the control's font (+0x94)
    void (*fatal)(int32_t id);            // does not return
    int32_t (*callback)(uint32_t fn, uint32_t* param);   // the +0x34 handler, stdcall(param); null: called directly
};

// param = the retail stack word: a pointer to a 32-bit slot holding the control's address (a dword view of the control);
// param itself is passed to the callback.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_8c60(uint32_t* param, const D2Win8C60Env* e) {
    int32_t* p = (int32_t*)(uintptr_t)*param;
    if (!p) { e->fatal(0x4d0); return 0; }
    if (p[0] != 4) { e->fatal(0x4d1); return 0; }
    uint32_t f = (uint32_t)p[2];
    if ((e->bitMasks[2] & f) && (e->bitMasks[0] & f) && p[0x15] > 0) {
        if (((uint8_t)p[0x17]) & 1) {
            int32_t y = (p[6] - p[4]) + e->mouseOfs;
            e->setFont(p[0x25]);
            e->sub8b50(p[0x25]);
            int32_t row = (y - p[0x11]) / (int32_t)(e->fontTab[*e->curFont * 5][10] + p[0x2a]) + p[0x16];
            if (!(((uint8_t)p[0x17]) & 8)) p[0x18] = (row < 0 || row >= p[0x15]) ? -1 : row;
            else { if (row < 0) row = 0; p[0x18] = row < p[0x15] - 1 ? row : p[0x15] - 1; }
        }
        if (p[0xd]) return e->callback ? e->callback((uint32_t)p[0xd], param) : ((int32_t(*)(uint32_t*))(uintptr_t)(uint32_t)p[0xd])(param);
    }
    return 0;
}
