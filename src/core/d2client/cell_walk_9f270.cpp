// 1.13c D2Client+0x9F270 (374 B): per-cell draw-list walk, from the cached decompile.
// ctx = retail 32-bit layout (byte-addressed, EDI implicit arg); imports are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientCellWalk9f270Env {
    uint32_t arg;                                   // DAT 0x6FBCC414
    void (*sub666d0)(uint32_t unit, uint32_t colour);   // D2Client+0x666D0 (ECX unit, stack word = light colour)
    uint32_t (*ord10013)();
    void (*sub1ba90)(uint32_t unit);                // D2Client+0x6BA90 (EAX = node unit)
    void (*ord10069)(uint32_t a, int32_t x, int32_t y, uint32_t mode, uint32_t arg, uint32_t ecx);   // ecx: the register the call leaves (0x6fb4f38b / 0x6fb4f36e)
    void (*sub9e970)(uint32_t room, uint32_t one, uint32_t ctx);  // D2Client+0x9E970
    const uint8_t* lightGrid;   // DAT 0x6FBB5208: 48x48 entries of 8 bytes, colour bytes at +4..+7
    uint32_t colourLight;       // DAT 0x6FB8E830
    int32_t viewX, viewY;       // DAT 0x6FBB4DF0 / 0x6FBB4DF4
};

static inline uint32_t rd(const uint8_t* b, uint32_t o) { uint32_t v; memcpy(&v, b + o, 4); return v; }

// 1.13c D2Client+0x9F270; mem = 32-bit guest base, ctx = guest address of EDI struct
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f270(const uint8_t* mem, uint32_t ctx, D2ClientCellWalk9f270Env* e) {
    uint32_t room = rd(mem, ctx + 0xeaa8);
    uint32_t n = rd(mem, ctx + 0xeab0); n *= n;
    for (; n; --n, room += 0x24) {
        for (uint32_t p = rd(mem, room + 0x10); p; p = rd(mem, p + 0x10)) {
            uint32_t fl = rd(mem, p);
            if (!(fl & 3)) {
                int32_t tx = ((int32_t)rd(mem, p + 4) >> 3) - e->viewX, ty = ((int32_t)rd(mem, p + 8) >> 3) - e->viewY;
                tx = tx < 0 ? 0 : tx > 47 ? 47 : tx; ty = ty < 0 ? 0 : ty > 47 ? 47 : ty;
                const uint8_t* g = e->lightGrid + (uint32_t)(ty * 48 + tx) * 8;
                // retail builds the three high bytes from stack slots written only when colour lighting is on; off, they are frame leftovers (0 here)
                uint32_t c = e->colourLight ? rd(g, 4) : (uint32_t)g[4];
                e->sub666d0(rd(mem, p + 0xc), c); continue;
            }
            uint32_t r = e->ord10013();
            if (!(fl & 1)) { if (fl & 2) e->sub1ba90(rd(mem, p + 0xc)); }
            else if (!r) {
                uint32_t q = rd(mem, p + 0xc);
                e->ord10069(rd(mem, q + 0x18), (int32_t)(rd(mem, ctx + 4) - rd(mem, ctx + 0x24) + rd(mem, q)),
                            (int32_t)(rd(mem, q + 4) - rd(mem, ctx + 0x28) + rd(mem, ctx + 8)), 4, e->arg, rd(mem, q));   // ecx = the unit's world x
            } else
                e->ord10069(rd(mem, rd(mem, p + 0xc) + 0x18), (int32_t)rd(mem, p + 4), (int32_t)rd(mem, p + 8), 4, e->arg, rd(mem, p + 0xc));   // ecx = the node's unit pointer
        }
        if (rd(mem, room + 0x14)) e->sub9e970(room, 1, ctx);
    }
}
