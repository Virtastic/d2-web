// src/core/d2win/d2win_e5f0.cpp -- 1.13c D2Win.dll+0xE5F0, draw one animated character-select figure (EBX = node, stack = gamma)
// Decompile-derived from the retail bytes, resolved against the disassembly: the node arrives in EBX and the draw gamma on
// the stack (ret 4). The layer-list lookup takes the list in EAX, the mode in EDX and the token from the 0x1FE5C table in
// ESI; the count and layer-order lookups take the animation record in EAX; the per-layer setup takes the node in EAX and
// the draw block in ESI. The frame counter is re-read after the layer loop, and both shared blink counters wrap with a
// signed compare. Imported callees, D2Win state and the tick source are seams on the Env struct, passed last.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2WinE5F0Env {
    void (*fatal)(uint32_t line);                            // D2Win+0x76A2 / 0x7684 / 0x11FE: assert text, exit(-1)
    const uint32_t* bitMasks;                                // 0x6F8FB17C -> gdwBitMasks
    const uint32_t* modeToken;                               // 0x6F8FFE5C, one dword per mode
    uint32_t* g38; uint32_t* g3c; uint32_t* g40; uint32_t* g44;   // 0x6F9A9E38..: next tick, overlay cel, overlay frames, overlay frame
    uint32_t* g48; uint32_t* g4c; uint32_t* g50;             // 0x6F9A9E48..: shadow cel, shadow frames, shadow frame
    const uint32_t* (*layerList)(uint32_t list, uint32_t mode, uint32_t token);   // D2Win+0xB860 (EAX, EDX, ESI)
    const uint8_t* (*layerCount)(uint32_t rec);              // D2Win+0xBAD0 (EAX)
    const uint8_t* (*layerOrder)(uint32_t rec, uint8_t cls, int32_t frame);       // D2Win+0xC680 (EAX; stdcall 2)
    uint8_t (*layerFlags)(uint8_t cls, uint8_t mode);        // D2Win+0xB7B0 (CL; stdcall 1)
    void (*layerSetup)(uint32_t* node, uint32_t* s);         // D2Win+0xE0D0 (EAX, ESI)
    uint32_t (*palette)(uint32_t idx);                       // D2Win+0xB810 (ECX)
    void (*draw)(uint32_t* s, int32_t x, int32_t y, uint32_t gamma, uint32_t mode, uint32_t pal);   // D2Win+0x77CE, D2Gfx draw-cel (stdcall 6)
    uint32_t (*tick)();                                      // kernel32 GetTickCount
};

// 1.13c D2Win+0xE5F0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_E5F0(uint32_t* p, uint32_t gamma, const D2WinE5F0Env* e) {
    uint32_t s[19];                                          // the 0x48-byte draw block, then the one-dword palette stub at +0x48
    if (!p) e->fatal(0x5e8);
    const uint32_t* m = e->bitMasks;
    if (!(m[0] & p[0])) return 0;
    if (m[10] & p[0]) {                                      // shadow
        if (!*e->g48) e->fatal(0x5f3);
        memset(s, 0, 0x48);
        s[13] = *e->g48; s[0] = *e->g50; s[16] = 0;
        e->draw(s, (int32_t)p[5] + 5, (int32_t)p[6] - 5, 0xffffffffu, 3, 0);
    }
    if (p[0x2f]) {                                           // name plate: palette stub {?, byte +0xC0, byte +0xC1}
        uint8_t* ex = (uint8_t*)&s[18];
        ex[1] = ((const uint8_t*)p)[0xc0]; ex[2] = ((const uint8_t*)p)[0xc1];
        memset(s, 0, 0x48);
        s[13] = p[0x2f]; s[16] = 0; s[0] = 0; s[14] = 1;
        e->draw(s, (int32_t)p[5] - 0x14, (int32_t)p[6] - 0x3c, 0xffffffffu, 5, (uint32_t)(uintptr_t)ex);
    }
    if (!p[0x2e]) e->fatal(0x617);
    const uint32_t* ent = e->layerList(p[0x2e], p[8], e->modeToken[p[9]]);
    if (!ent) return 0;
    const uint32_t rec = ((const uint32_t*)(uintptr_t)ent[0])[5];
    if (!rec) e->fatal(0x620);
    const uint8_t* cnt = e->layerCount(rec);
    const int32_t frame = (int32_t)p[2] >> 8;
    const uint8_t* order = e->layerOrder(rec, (uint8_t)p[1], frame);
    if (!order) e->fatal(0x625);
    uint32_t mode = 5;
    if (((m[5] & p[0]) && (m[4] & p[0])) || (m[13] & p[0])) mode = 1;
    for (int32_t i = 0; i < (int32_t)*cnt; i++) {
        const uint8_t layer = order[i];
        memset(s, 0, 0x48);
        s[16] = e->layerFlags(((const uint8_t*)p)[4], ((const uint8_t*)p)[5]);
        s[0] = (uint32_t)frame; s[13] = 0;
        ((uint8_t*)s)[5] = layer;
        e->layerSetup(p, s);
        s[17] |= 2;
        const uint32_t idx = ((const uint8_t*)p)[0x28 + layer];
        const uint32_t pal = idx != 0xff ? e->palette(idx) : 0;
        e->draw(s, (int32_t)p[5], (int32_t)p[6], gamma, mode, pal);
    }
    p[2] = p[2] + p[4];
    if (((int32_t)p[2] >> 8) >= (int32_t)(p[3] - 1)) p[2] = 0;
    const uint32_t now = e->tick();
    if (now > *e->g38) {
        *e->g38 = now + 0x42;
        if (!((int32_t)++*e->g44 < (int32_t)*e->g40)) *e->g44 = 0;
        if (!((int32_t)++*e->g50 < (int32_t)*e->g4c)) *e->g50 = 0;
    }
    if (m[9] & p[0]) {                                       // overlay
        if (!*e->g3c) e->fatal(0x666);
        memset(s, 0, 0x48);
        s[13] = *e->g3c; s[16] = 0; s[0] = *e->g44;
        e->draw(s, (int32_t)p[5], (int32_t)p[6] + 10, 0xffffffffu, 3, 0);
    }
    return 1;
}
