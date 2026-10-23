// D2CMP sprite-cache entry fill (SpriteCache.cpp). Raw 32-bit-layout byte offsets; Fog/helpers injected.
#include <cstdint>
#include <cstring>

struct D9F0Env {
    uint32_t (*tbl12240)(int k, uint32_t mask);                          // 0x12240
    void     (*noPayload)(uint32_t idx, const uint8_t* cache);           // 0xC670 + 0xC2D0 (name build + log)
    uint32_t (*alloc)(const uint8_t* src, uint32_t len, bool cb, const uint8_t* hdr, int dir, int frame); // Fog #10091 (+cb 0xD760)
    int      (*post)();                                                  // Fog #10095
    void     (*fatal)(uint32_t id);                                      // Fog #10265 + #10024, _exit(-1)
};
static int32_t  rd(const uint8_t* p, int o) { int32_t v; memcpy(&v, p + o, 4); return v; }
static void     wr(uint8_t* p, int o, int32_t v) { memcpy(p + o, &v, 4); }

// 1.13c D2CMP+0xD9F0
// hdr = sprite header (EDX), ent = cache entry (EDI), cache = param_3.
int d2cmp_d9f0(const D9F0Env& e, const uint8_t* hdr, uint8_t* ent, const uint8_t* cache, int dirs, int frames)
{
    if (!hdr || !ent || !cache) return 0;
    int kind = rd(cache, 8);
    uint32_t a, b;
    if ((kind == 6 && (cache[4] & 2)) || kind == 3) {
        if (frames != 1) return 0;
        a = e.tbl12240(rd(cache, 0x40), (uint32_t)rd(hdr, 0x14)); b = a + 1;
    } else if (kind == 2) { a = 0; b = 1; }
    else if (kind == 4) { a = rd(cache, 0x40); b = a + 1; }
    else {
        a = (kind == 1 && frames == 1) ? e.tbl12240(rd(cache, 0x40), (uint32_t)rd(hdr, 0x14)) : (uint32_t)(dirs * frames);
        b = a + frames;
    }
    int len = rd(hdr, b * 4 + 0x1c) - rd(hdr, a * 4 + 0x1c);
    const uint8_t* src = hdr + a * 4 + 0x1c;
    if (len == 0) { e.noPayload(a, cache); return 0; }
    if (rd(ent, 0xb0)) return 0;
    ent[0x22] = (uint8_t)(dirs * frames); ent[0x21] = (uint8_t)frames;
    ent[0x24] = hdr[0x18]; ent[0x23] = hdr[0x14] + 1;
    int32_t base = rd(src, 0);
    for (uint32_t i = a; i <= b; i++) {
        if (i > 0x20) { e.fatal(0x355); return 0; }
        int32_t d = rd(hdr, i * 4 + 0x1c) - base;
        wr(ent, 0x2c + i * 4, d);
        if (d < 0) { e.fatal(0x357); return 0; }
    }
    bool cb = (hdr[0] & 2) != 0;
    if (cb) ent[0x20] |= 4;
    wr(ent, 0xb0, (int32_t)e.alloc(hdr + base, len, cb, hdr, dirs * frames, frames)); // 0x382 / 0x372
    if (cb) len = e.post();
    if (!rd(ent, 0xb0)) { e.fatal(0x386); return 0; }
    return len;
}
