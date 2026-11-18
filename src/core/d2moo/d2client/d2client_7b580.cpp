// 1.13c D2Client+0x7B580 (429 B): draw the timed message list, then free its expired nodes. void (void), plain ret.
// Globals: 0x113AAA list head (dword, unaligned), 0x113AAE node count (WORD), 0xFAD98 / 0xFADC4 panel flags, 0xDBC4C screen height.
// Node (packed, links unaligned): +0x00 six wide-string pointers, +0x1A colour, +0x1E expiry tick, +0x22 next.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c7b580Callees {
    uint32_t (*setFont)(uint32_t font);                                  // D2Win 10184 (thunk 0xD3A8, ECX): returns the previous font
    int32_t (*strLen)(uint32_t s);                                       // D2Lang Unicode::strlen (ECX)
    int32_t (*width)(uint32_t s, int32_t len);                           // D2Win 10171 (thunk 0xD3E4, ECX, EDX)
    void (*rect)(int32_t l, int32_t t, int32_t r, int32_t b, uint32_t c, uint32_t mode);   // D2Gfx 10014 (thunk 0xD0BA, stdcall 6)
    void (*text)(uint32_t s, int32_t x, int32_t y, uint32_t col, uint32_t z);              // D2Win 10150 (thunk 0xD37E, ECX, EDX, 3 stack)
    void (*fogFree)(uint32_t p, uint32_t line);                          // Fog 10043 (thunk 0xBF86, ECX = p, EDX = source file, line, 0)
    uint32_t (*tick)();                                                  // kernel32 GetTickCount
};

static inline uint32_t rd32(uintptr_t a) { uint32_t v; memcpy(&v, (const void*)a, 4); return v; }
static inline void wr32(uintptr_t a, uint32_t v) { memcpy((void*)a, &v, 4); }

// list = the 6 bytes at 0x113AAA (head dword, count WORD at +4)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_7b580(uint8_t* list, uint32_t flagA, uint32_t flagB, int32_t screenH, const D2c7b580Callees* cb) {
    uint16_t count; memcpy(&count, list + 4, 2);
    if (!count) return;
    const uint32_t oldFont = cb->setFont(0xD);
    const int32_t x = (flagA || flagB) ? 100 : 15;
    int32_t row = 0;
    for (uintptr_t n = rd32((uintptr_t)list); n; n = rd32(n + 0x22), row -= 0x19) {
        for (int32_t k = 0, off = 0; k < 6; ++k, off += 0x19) {
            const uint32_t s = rd32(n + 4 * k);
            if (!s) continue;
            const int32_t w = cb->width(s, cb->strLen(s));
            const int32_t half = off >> 1;
            const int32_t y = row - half + screenH - 0x81;
            cb->rect(x - 5, y, w + 0xA + x - 5, y + 0x10, 0, 1);
            cb->text(rd32(n + 4 * k), x, row - half + screenH - 0x73, rd32(n + 0x1A), 0);
        }
    }
    const uint32_t now = cb->tick();
    uintptr_t prev = 0;
    for (uintptr_t n = rd32((uintptr_t)list); n;) {
        if (rd32(n + 0x1E) < now) {
            memcpy(&count, list + 4, 2); --count; memcpy(list + 4, &count, 2);
            const uint32_t nx = rd32(n + 0x22);
            if (rd32((uintptr_t)list) == n) wr32((uintptr_t)list, nx);
            for (int k = 0; k < 6; ++k) if (const uint32_t s = rd32(n + 4 * k)) cb->fogFree(s, 0x6AF);
            cb->fogFree((uint32_t)n, 0x6B2);
            if (prev) wr32(prev + 0x22, nx);
            n = nx;
        } else { prev = n; n = rd32(n + 0x22); }
    }
    cb->setFont(oldFont);
}
