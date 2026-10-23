// 1.13c D2Client+0x9F270 (374 B): per-cell draw-list walk, from the cached decompile.
// ctx = retail 32-bit layout (byte-addressed, EDI implicit arg); imports are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientCellWalk9f270Env {
    uint32_t arg;                                   // DAT 0x6FBCC414
    void (*sub666d0)(uint32_t unit);                // D2Client+0x666D0
    uint32_t (*ord10013)();
    void (*sub1ba90)();                             // D2Client+0x1BA90
    void (*ord10069)(uint32_t a, int32_t x, int32_t y, uint32_t mode, uint32_t arg);
    void (*sub9e970)(uint32_t room, uint32_t one, uint32_t ctx);  // D2Client+0x9E970
};

static inline uint32_t rd(const uint8_t* b, uint32_t o) { uint32_t v; memcpy(&v, b + o, 4); return v; }

// 1.13c D2Client+0x9F270; mem = 32-bit guest base, ctx = guest address of EDI struct
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f270(const uint8_t* mem, uint32_t ctx, D2ClientCellWalk9f270Env* e) {
    uint32_t room = rd(mem, ctx + 0xeaa8);
    uint32_t n = rd(mem, ctx + 0xeab0); n *= n;
    for (; n; --n, room += 0x24) {
        for (uint32_t p = rd(mem, room + 0x10); p; p = rd(mem, p + 0x10)) {
            uint32_t fl = rd(mem, p);
            if (!(fl & 3)) { e->sub666d0(rd(mem, p + 0xc)); continue; }
            uint32_t r = e->ord10013();
            if (!(fl & 1)) { if (fl & 2) e->sub1ba90(); }
            else if (!r) {
                uint32_t q = rd(mem, p + 0xc);
                e->ord10069(rd(mem, q + 0x18), (int32_t)(rd(mem, ctx + 4) - rd(mem, ctx + 0x24) + rd(mem, q)),
                            (int32_t)(rd(mem, q + 4) - rd(mem, ctx + 0x28) + rd(mem, ctx + 8)), 4, e->arg);
            } else
                e->ord10069(rd(mem, rd(mem, p + 0xc) + 0x18), (int32_t)rd(mem, p + 4), (int32_t)rd(mem, p + 8), 4, e->arg);
        }
        if (rd(mem, room + 0x14)) e->sub9e970(room, 1, ctx);
    }
}
