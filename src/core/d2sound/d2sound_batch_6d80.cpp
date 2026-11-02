// src/core/d2sound/ -- 1.13c D2Sound.dll batch 25 (6D80 8160 8710 9A70 9AD0), from the decompile.
// Host seams (Storm #426/#10042/#10043, 0x6FA0, 0x8020, 0x8830, 0xACCC, DirectSound vtbl) are function pointers.
#include <cstdint>
#include <cstdlib>

struct D2SndBatch25Host {
    void (*regSave)(const char* sect, const char* key, uint32_t type, uint32_t val); // Storm #426
    void* (*alloc)(uint32_t size, uint32_t zero);                                    // Storm #10042
    void (*freeSlot)(void* p);                                                      // 0x6FA0
    void (*stopAll)(void);                                                          // 0x8020
    void (*ordinal10043)(uint32_t ecx, uint32_t edx, uint32_t a, uint32_t b);       // 0x5F68 -> Fog!10043 (stdcall, 2 words)
    uint16_t (*attenuate)(float a, float b, float c);                               // 0x8830
    uint64_t (*mixValue)(void);                                                     // 0xACCC
    void (*setPan)(void* dsBuf, int32_t v);                                         // vtbl +0x40
    void (*fatal)(void);                                                            // #10265/#10024 + _exit(-1)
    void (*critEnter)(void);                                                        // EnterCriticalSection(0x16548)
    void (*critDelete)(void);                                                       // DeleteCriticalSection(0x16548)
};
static D2SndBatch25Host g_h;
static uint32_t g_positionalBias, g_masterVolume;
static uint32_t g_initFlag;             // 0x165B8 (CS 0x16548 is entered/deleted through the host seams)
static void* g_slots[8];                // 0x16560..0x1657F
static uint8_t* g_chain;                // 0x165BC, node: +0x50 flag, +0x5C next
void D2Sound_Batch25SetHost(const D2SndBatch25Host& h) { g_h = h; }

// 1.13c D2sound+0x9A70 ordinal 10022: stores "Positional Bias", identity return.
uint32_t D2Sound_PositionalBiasSet(uint32_t v) { g_h.regSave("Diablo II", "Positional Bias", 0, v); g_positionalBias = v; return v; }
// 1.13c D2sound+0x9AD0 ordinal 10029: stores "Master Volume", identity return.
uint32_t D2Sound_MasterVolumeSet(uint32_t v) { g_h.regSave("Diablo II", "Master Volume", 0, v); g_masterVolume = v; return v; }
// FN probe read-back of the module globals 0x164AC (positional bias) and 0x164A4 (master volume).
void D2Sound_VolumesGet(uint32_t* bias, uint32_t* master) { *bias = g_positionalBias; *master = g_masterVolume; }

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
// Retail (disassembly 0x8160..0x81F2): the 0x5F68 thunk (Fog!10043, stdcall, two stack words) is called with
// ECX = the node's flag dword on the 0x67 call and ECX = the node on the 0x68 call; EDX = 0x6F9C06B4 both times.
// The chain link is a 4-byte guest pointer at node+0x5C (mov esi,[eax+0x5c]), not a host-width pointer.
static const uint32_t kFogCtxEdx = 0x6F9C06B4u;
void D2Sound_Shutdown(void) {
    if (!g_initFlag) return;
    g_h.critEnter();
    g_h.stopAll();
    for (int i = 0; i < 8; i++) { g_h.freeSlot(g_slots[i]); g_slots[i] = 0; }
    while (g_chain) {
        uint32_t next = *(uint32_t*)(g_chain + 0x5c);
        uint32_t flag = *(uint32_t*)(g_chain + 0x50);
        if (flag) g_h.ordinal10043(flag, kFogCtxEdx, 0x67, 0);
        g_h.ordinal10043((uint32_t)(uintptr_t)g_chain, kFogCtxEdx, 0x68, 0);
        g_chain = (uint8_t*)(uintptr_t)next;
    }
    g_initFlag = 0;
    g_h.critDelete();
}

// State seams: load the state Shutdown owns and read it back after the call.
void D2Sound_fnSetShutdownState(uint32_t init, const uint32_t* slots, uint32_t chain) {
    g_initFlag = init;
    for (int i = 0; i < 8; i++) g_slots[i] = (void*)(uintptr_t)slots[i];
    g_chain = (uint8_t*)(uintptr_t)chain;
}
void D2Sound_fnGetShutdownState(uint32_t* init, uint32_t* slots, uint32_t* chain) {
    *init = g_initFlag;
    for (int i = 0; i < 8; i++) slots[i] = (uint32_t)(uintptr_t)g_slots[i];
    *chain = (uint32_t)(uintptr_t)g_chain;
}
