// Globals-only leaf predicates/lookups; each global is a parameter (the caller reads the live global).
#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x265A0: x = ECX, y = ESI, w = [0xDBC48], h = [0xDBC4C].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_InRect265A0(int32_t x, int32_t y, int32_t w, int32_t h) {
    return (w / 2 - 0x91 <= x && x <= w / 2 - 0x80 && h - 0x1c <= y && y <= h - 8) ? 1u : 0u;
}

// 1.13c D2Client+0x4FE70: w = [0xDBC48], h = [0xDBC4C], y = [0x11B824], x = [0x11B828].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_InRect4FE70(int32_t w, int32_t h, int32_t y, int32_t x) {
    return (w / 2 + 0xa3 < x && x < w / 2 + 0xc5 && h - 0x2a < y && y < h - 8) ? 1u : 0u;
}

// 1.13c D2Client+0x4FEC0: sel = EAX, x = ECX, y = EDI, w = [0xDBC48], h = [0xDBC4C].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_InRect4FEC0(int32_t sel, int32_t x, int32_t y, int32_t w, int32_t h) {
    int32_t b = sel == 1 ? w - w / 2 : w;
    return (b - 0x49 < x && x < b - 0x28 && h - 0x8a < y && y < h - 0x66) ? 1u : 0u;
}

// 1.13c D2Client+0x398C0: a = EDX, b = ESI, table = 114 x 10 bytes at 0x11A2E0 {dword A, word R, dword B}; first match, R != 0xFFFF.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Has398C0(uint32_t a, uint32_t b, const uint8_t* table) {
    for (int i = 0; i < 114; ++i) {
        uint32_t ea, eb; uint16_t r; const uint8_t* e = table + i * 10;
        memcpy(&ea, e, 4); memcpy(&r, e + 4, 2); memcpy(&eb, e + 6, 4);
        if (ea == a && eb == b) return r != 0xFFFF;
    }
    return 0;
}

// 1.13c D2Client+0x45C00: count = [0xF3B9C], key = [0x119725], table at 0xF37A8 (stride 0x16, key dword at +0).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Client_FindIdx45C00(int32_t count, uint32_t key, const uint8_t* table) {
    for (int32_t i = 0; i < count; ++i) { uint32_t t; memcpy(&t, table + i * 0x16, 4); if (t == key) return i; }
    return 0;
}

// 1.13c D2Client+0x5F9C0: g1b0 = [0x11C1B0], g208 = [0x11C208], g20c = [0x11C20C]; returns the new [0x11C208] (retail also stores it).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Fix5F9C0(uint32_t g1b0, uint32_t g208, uint32_t g20c) {
    return (g1b0 == 1 && g208 == 1) ? (g20c != 0 ? 0u : 2u) : g208;
}

// 1.13c D2Client+0x6D2F0: ECX -> state dword; {0,1,2,4,6} -> 4, {3,5} -> 3, else unchanged.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Next6D2F0(uint32_t s) {
    switch (s) { case 0: case 1: case 2: case 4: case 6: return 4; case 3: case 5: return 3; default: return s; }
}

// 1.13c D2Client+0x779F0: EAX = x0, ESI = w, EDX = y1, stack = h; X = [0x11B828], Y = [0x11B824] (signed).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_InRect779F0(int32_t x0, int32_t w, int32_t y1, int32_t h, int32_t X, int32_t Y) {
    return (x0 <= X && X <= x0 + w && y1 - h <= Y && Y <= y1) ? 1u : 0u;
}

// 1.13c D2Client+0x6D690: EAX = table (byte count @+0, entries @+0x1C stride 9, key byte @+0, value byte @+4), ESI = key (signed).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Find6D690(const uint8_t* t, int32_t key) {
    if (!t || key >= 0x10) return 5;
    for (int i = 0; i < t[0]; ++i) if ((int32_t)t[0x1c + i * 9] == key) return t[0x1c + i * 9 + 4];
    return 5;
}
