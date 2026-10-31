// src/core/d2win/d2win_sub_e3c0.cpp -- D2Win widget constructor (stdcall type, arg1, cols[16], tx[16]).
#include <cstdint>

struct D2WinE3C0Env {
    uint32_t* (*alloc)(uint32_t size, uint32_t line);      // Fog alloc (ecx = size, stack line, 0)
    void (*fatal)(uint32_t strId);
    void (*initColours)(void);                             // 0xD880
    uint32_t (*typeColours)(uint32_t type, const uint8_t* cols); // 0xC180 (stack type, ebx = cols); type < 7
    void (*loadColour)(uint32_t slot, const uint8_t* col);  // 0xBF60 (eax = slot, esi = &cols[slot])
    uint32_t (*ready)(uint32_t* w);                         // 0xDFF0 (esi = w)
    uint32_t (*attach)(uint32_t* w, uint32_t arg1);         // 0xC410 (ecx = w, edx = arg1)
    const uint32_t* bitMasks;                              // Fog gdwBitMasks
    const uint32_t* colourTbl;                             // 3 words per colour
    int32_t colourCount;
    uint32_t* head;                                        // widget list head (32-bit link; prev at +0xC8, next at +0xC4)
};

// 1.13c D2Win+0xE3C0
extern "C" uint32_t* d2win_sub_E3C0(const D2WinE3C0Env* e, uint32_t type, uint32_t arg1, const uint8_t* cols, const uint8_t* tx) {
    uint32_t* w = e->alloc(0x234, 0xc65);
    if (!w) { e->fatal(0xc66); return nullptr; }
    for (int i = 0; i < 0x8d; i++) w[i] = 0;
    uint32_t* s = e->alloc(4, 0x85b);
    if (!s) { e->fatal(0x85c); return nullptr; }
    *s = 0;
    w[0x2e] = (uint32_t)(uintptr_t)s;
    e->initColours();
    w[7] = type;
    if (type < 7) { w[9] = e->typeColours(type, cols); if (!w[9]) return nullptr; }
    else w[9] = 1;
    w[8] = arg1;
    uint32_t v = w[0] | e->bitMasks[0]; v |= e->bitMasks[1]; w[0] = v | e->bitMasks[2];
    for (uint32_t i = 0; i < 16; i++) {
        w[0x1e + i] = 1;
        uint8_t c = cols[i];
        if (c == 0 || (int32_t)c >= e->colourCount || c == 0xff) w[0xe + i] = 0;
        else { e->loadColour(i, cols + i); w[0xe + i] = e->colourTbl[c * 3]; w[0x1e + i] = e->colourTbl[c * 3 + 2]; }
    }
    uint8_t* p = (uint8_t*)w + 0x28;
    for (int i = 0; i < 16; i++) { uint8_t c = tx[i]; p[i] = c != 0xff ? (uint8_t)(c - 1) : c; }
    if (e->ready(w) != 0 && e->attach(w, arg1) == 0) return nullptr;
    w[0x32] = 0;
    w[0x31] = *e->head;
    if (*e->head) ((uint32_t*)(uintptr_t)*e->head)[0x32] = (uint32_t)(uintptr_t)w;
    *e->head = (uint32_t)(uintptr_t)w;
    return w;
}
