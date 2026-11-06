// D2CMP sprite-cache entry fill (SpriteCache.cpp), 1.13c D2CMP+0xD9F0, transcribed from the disassembly. Raw 32-bit-layout byte offsets; the callees are injected.
// Retail convention: EDX = hdr (the sprite header), EDI = ent (the cache entry), stack (cache, dirs, frames, s3), ret 0x10. s3 reaches Fog 10091 in EDX.
#include <cstdint>
#include <cstring>

struct D2CmpD9f0Env {
    uint32_t cacheWord;                                                                      // [0x67240] (ECX of the allocator)
    uint32_t (*tbl12240)(uint32_t cacheField, uint32_t hdrField);                            // 0x12240 (stack cache[0x40], hdr[0x14]; ret 8)
    void (*c670)(const uint8_t* cache);                                                      // 0xC670 (EAX = &name, EDX = cache)
    void (*c2d0)(const uint8_t* cache);                                                      // 0xC2D0 (EBX = &name, ESI = cache, stack &name2)
    uint32_t (*alloc)(uint32_t cacheWord, uint32_t s3, uint32_t off, uint32_t len, bool cb); // Fog #10091 via 0x8DDE (ECX = cache word, EDX = s3, stack 0, off, len, 0, cbstruct|0, -2, file, line)
    uint32_t (*post)(uint32_t handle);                                                       // Fog #10095 via 0x8DD2 (ECX = handle)
};
static inline uint32_t d9_rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline void d9_wr(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// Fatal paths (index > 0x20, negative offset delta, null handle) call _exit(-1) in retail and are not reproduced.
extern "C" uint32_t d2cmp_d9f0(const uint8_t* hdr, uint8_t* ent, const uint8_t* cache, uint32_t dirs, uint32_t frames, uint32_t s3, const D2CmpD9f0Env* e) {
    if (!hdr || !ent || !cache) return 0;
    const uint32_t kind = d9_rd(cache, 8);
    uint32_t a, b;
    if ((kind == 6 && (cache[4] & 2)) || kind == 3) {
        if (frames != 1) return 0;
        a = e->tbl12240(d9_rd(cache, 0x40), d9_rd(hdr, 0x14)); b = a + 1;
    } else if (kind == 2) { a = 0; b = 1; }
    else if (kind == 4) { a = d9_rd(cache, 0x40); b = a + 1; }
    else {
        if (kind == 1 && frames == 1) a = e->tbl12240(d9_rd(cache, 0x40), d9_rd(hdr, 0x14));
        else a = dirs * frames;
        b = a + frames;
    }
    const uint32_t base = d9_rd(hdr, a * 4 + 0x1c);
    const uint32_t len = d9_rd(hdr, b * 4 + 0x1c) - base;
    if (len == 0) { e->c670(cache); e->c2d0(cache); return 0; }
    if (d9_rd(ent, 0xb0)) return 0;
    ent[0x22] = (uint8_t)((dirs & 0xff) * (frames & 0xff)); ent[0x21] = (uint8_t)frames;
    ent[0x24] = hdr[0x18]; ent[0x23] = (uint8_t)(hdr[0x14] + 1);
    for (uint32_t i = a; i <= b; i++) {
        d9_wr(ent, 0x2c + i * 4, d9_rd(hdr, i * 4 + 0x1c) - base);
    }
    const bool cb = (hdr[0] & 2) != 0;
    uint32_t r = len;
    if (cb) {
        ent[0x20] |= 4;
        const uint32_t h = e->alloc(e->cacheWord, s3, base, len, true);
        d9_wr(ent, 0xb0, h);
        r = e->post(h);
    } else {
        d9_wr(ent, 0xb0, e->alloc(e->cacheWord, s3, base, len, false));
    }
    return r;
}
