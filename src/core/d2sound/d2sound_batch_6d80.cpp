// src/core/d2sound/ -- 1.13c D2Sound.dll batch 25 (6D80 8160 8710 9A70 9AD0), from the decompile.
// Host seams (Storm #426/#10042/#10043, 0x6FA0, 0x8020, 0x8830, 0xACCC, DirectSound vtbl) are function pointers.
#include <cstdint>
#include <cstdlib>

struct D2SndBatch25Host {
    void (*regSave)(const char* sect, const char* key, uint32_t type, uint32_t val); // Storm #426
    void* (*alloc)(uint32_t size, uint32_t zero);                                    // Storm #10042
    void (*freeSlot)(void* p);                                                      // 0x6FA0
    void (*stopAll)(void);                                                          // 0x8020
    void (*ordinal10043)(uint32_t a, uint32_t b);                                   // Fog free-ish #10043
    uint16_t (*attenuate)(float a, float b, float c);                               // 0x8830
    uint64_t (*mixValue)(void);                                                     // 0xACCC
    void (*setPan)(void* dsBuf, int32_t v);                                         // vtbl +0x40
    void (*fatal)(void);                                                            // #10265/#10024 + _exit(-1)
};
static D2SndBatch25Host g_h;
static uint32_t g_positionalBias, g_masterVolume;
static uint32_t g_initFlag, g_critSec;  // 0x165B8 / 0x16548 (CS elided: single-threaded wasm)
static void* g_slots[8];                // 0x16560..0x1657F
static uint8_t* g_chain;                // 0x165BC, node: +0x50 flag, +0x5C next
void D2Sound_Batch25SetHost(const D2SndBatch25Host& h) { g_h = h; }

// 1.13c D2sound+0x9A70 ordinal 10022: stores "Positional Bias", identity return.
uint32_t D2Sound_PositionalBiasSet(uint32_t v) { g_h.regSave("Diablo II", "Positional Bias", 0, v); g_positionalBias = v; return v; }
// 1.13c D2sound+0x9AD0 ordinal 10029: stores "Master Volume", identity return.
uint32_t D2Sound_MasterVolumeSet(uint32_t v) { g_h.regSave("Diablo II", "Master Volume", 0, v); g_masterVolume = v; return v; }

// 1.13c D2sound+0x8710 ordinal 10034: push-front {value, next} (0xD4 alloc, only 8 bytes used).
void D2Sound_ListPush(uint32_t** head, uint32_t value) {
    uint32_t* n = (uint32_t*)g_h.alloc(0xD4, 0);
    uint32_t* old = *head;
    n[0] = value; n[1] = (uint32_t)(uintptr_t)old;
    *head = n;
}

// 1.13c D2sound+0x6D80 ordinal 10028: pan from 0..255 (0x80 centre); null buffer is fatal.
void D2Sound_SetPan(void* buf, int32_t p) {
    if (!buf) { g_h.fatal(); return; }
    int32_t v = 0;
    if (p > 0x80) v = (int32_t)g_h.attenuate(0.0f, 127.0f, (float)(0xff - p)), v = (int32_t)g_h.mixValue();
    else if (p < 0x80) g_h.attenuate(0.0f, 127.0f, (float)p), v = (int32_t)g_h.mixValue();
    g_h.setPan(buf, v);
}

// 1.13c D2sound+0x8160 ordinal 10017: shutdown; no-op unless initialised.
void D2Sound_Shutdown(void) {
    if (!g_initFlag) return;
    g_h.stopAll();
    for (int i = 0; i < 8; i++) { g_h.freeSlot(g_slots[i]); g_slots[i] = 0; }
    while (g_chain) {
        uint8_t* next = *(uint8_t**)(g_chain + 0x5c);
        if (*(uint32_t*)(g_chain + 0x50)) g_h.ordinal10043(0x67, 0);
        g_h.ordinal10043(0x68, 0);
        g_chain = next;
    }
    g_initFlag = 0;
}
