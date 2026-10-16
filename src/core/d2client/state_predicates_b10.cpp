// Globals-only leaf lookups; each global is a parameter (the caller reads the live global).
#include <emscripten.h>
#include <cstdint>
#include <cstring>


// 1.13c D2Client+0x5DA50: idx = EAX, base = [0x11C24C], count = [0x11C250]; element stride 0x8E.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_RecordAddr5DA50(int32_t idx, uint32_t base, int32_t count) { return (base && idx >= 0 && idx < count) ? (uint32_t)idx * 0x8Eu + base : 0u; }

// 1.13c D2Client+0x45EB0: unit = EAX (null -> -1), count = [0xF42F0], table = bytes at 0xF3BA0 (stride 0x27, key dword at +0).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Client_FindSlot45EB0(const uint8_t* unit, int32_t count, const uint8_t* table) {
    if (!unit) return -1;
    uint32_t k; memcpy(&k, unit + 4, 4);
    for (int32_t i = 0; i < count; ++i) { uint32_t t; memcpy(&t, table + i * 0x27, 4); if (t == k) return i; }
    return 0;
}

// 1.13c D2Client+0x38E50: a = ESI, b = EDX, table = 114 x 10 bytes at 0x11A2E0 {dword A, word R, dword B}; low 16 bits of the result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Lookup38E50(uint32_t a, uint32_t b, const uint8_t* table) {
    for (int i = 0; i < 114; ++i) {
        uint32_t ea, eb; uint16_t r; const uint8_t* e = table + i * 10;
        memcpy(&ea, e, 4); memcpy(&r, e + 4, 2); memcpy(&eb, e + 6, 4);
        if (ea == a && eb == b) return r;
    }
    return 0xFFFF;
}
