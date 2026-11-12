// D2CMP Codec.cpp: decode a run of DCC directions into DC6-style sprite blocks (the DCC driver).
#include <cstring>
#include "d2cmp_dcc.h"

// The globals and imports the retail body reaches, passed in. Sizes are retail's .data blocks.
struct DccDecodeEnv {
    void (*lock)();                                   // EnterCriticalSection(.data 0x6FE6FBA8)
    void (*unlock)();                                 // LeaveCriticalSection
    uint8_t* (*alloc)(void* pool, uint32_t size);     // thunk 0x6FE18D84 (Codec.cpp, line 0x56F)
    D2CmpFatalFn fatal;                               // thunk 0x6FE18D90
    void (*log)(const char*);                         // Ordinal_10029 (13620's message)
    void (*fileError)(const char* name);              // Ordinal_10029 + sprintf before fatal 0x66E
    DccFrame* frames;                                 // .data 0x6FF10FD8
    DccCell* cells;                                   // .data 0x6FEF5860, 0x6DDD dwords
    uint8_t* pixbuf;                                  // .data 0x6FEB5860
    uint8_t* canvas;                                  // .data 0x6FE983A0
    uint32_t ext32;                                   // .rdata 0x6FE2D538[32]: the dword after the
                                                      // sign table (0x6FE208F0 at the preferred base)
};

static const int32_t kDccWidth[16] = {0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32};

static uint32_t rd_u(DccBits* b, int32_t w) { return w == 0 ? 0u : dcc_read(b, w); }
static uint32_t rd_s(DccBits* b, int32_t w, uint32_t ext32) {
    if (w == 0) return 0;
    uint32_t v = dcc_read(b, w);
    if (v & (1u << (w - 1))) v |= (w == 32) ? ext32 : ~dcc_mask(w);
    return v;
}
static void bits_at(DccBits* s, const DccBits& base, uint32_t bits) {
    *s = base;
    s->cur = base.cur + (bits >> 5);
    s->pos = (int32_t)(bits & 31);
}

// 1.13c D2CMP+0x140A0
// Retail: ECX = DCC header, EDX = direction data, EAX = &total; stack (pool, arg1, offsets out,
// first direction, direction count, file name), ret 0x18. Under the codec lock: sizes each
// direction from the dword at data + dirOff[d] - dirOff[first] (dirOff at header+0x1C), writes
// the running offset to out[3+d] and the sum to *total, allocates the block. Per direction
// (read sequentially from data): skip 32 bits, 2 flag bits, seven 4-bit width codes, the frame
// headers (var0, w, h, signed xoff, signed yoff, optional bytes, coded size, bottom-up bit), the
// byte-aligned optional bytes, the box (13220, 14020), the stream sizes (20 bits: equal-cell if
// flag 2, pixel-mask, encoding and raw if flag 1), the 256-bit colour map (13A30); stage 1
// builds the pixel buffer (13B70) and maps it through the colour map; frames get canvas
// pointers (fatal 0x62B / 0x62C off the box); stage 2 fills cells (13780) and emits a sprite
// header + 13620 body per finished frame (fatal 0x65A no pixels, 0x66E encode error). The next
// direction starts at the byte-aligned end of the pixel-code stream.
uint8_t* d2cmp_140a0(const uint8_t* hdr, const uint8_t* data, uint32_t* total, void* pool,
                     int32_t arg1, uint32_t* out, int32_t first, int32_t count, const char* name,
                     const DccDecodeEnv* env) {
    env->lock();
    int32_t lh[4];
    memcpy(&lh[0], hdr + 4, 4); memcpy(&lh[1], hdr + 8, 4);
    memcpy(&lh[2], hdr + 0x14, 4); memcpy(&lh[3], hdr + 0x18, 4);
    int32_t end = first + count;
    *total = 0;
    for (int32_t d = first; d < end; d++) {
        uint32_t od, o0, sz;
        memcpy(&od, hdr + 0x1c + d * 4, 4); memcpy(&o0, hdr + 0x1c + first * 4, 4);
        memcpy(&sz, data + (od - o0), 4);
        out[3 + d] = *total;
        *total += sz;
    }
    uint8_t* block = env->alloc(pool, *total);
    uint8_t* dst = block;
    DccFrame* frames = env->frames;
    DccBits s = {(const uint32_t*)data, (const uint32_t*)data, (uint32_t)arg1 * 8u, 0};
    for (int32_t d = first; d < end; d++) {
        dcc_read(&s, 32);
        uint32_t flags = dcc_read(&s, 2);
        uint32_t wc = dcc_read(&s, 28);
        int32_t w[7];
        for (int32_t i = 0; i < 7; i++) w[i] = kDccWidth[(wc >> (i * 4)) & 0xf];
        int32_t nf = lh[3];
        uint32_t optSum = 0;
        if (nf > 0) {   // retail: frames + nf*0x2C > frames
            for (int32_t i = 0; i < nf; i++) {
                DccFrame* f = &frames[i];
                f->var0 = (int32_t)rd_u(&s, w[0]);
                f->w = (int32_t)rd_u(&s, w[1]);
                f->h = (int32_t)rd_u(&s, w[2]);
                f->xoff = (int32_t)rd_s(&s, w[3], env->ext32);
                f->yoff = (int32_t)rd_s(&s, w[4], env->ext32);
                f->optBytes = rd_u(&s, w[5]);
                f->opt = nullptr;
                f->coded = rd_u(&s, w[6]);
                f->pix = nullptr;
                f->stride = 0;
                f->bottomUp = (int32_t)dcc_read(&s, 1);
                optSum += f->optBytes;
            }
            if (optSum != 0) {
                uint32_t p = ((uint32_t)s.pos + 7u) & ~7u;
                uint32_t sub = p & 31;
                s.cur += p >> 5;
                const uint8_t* q = (const uint8_t*)s.cur + sub / 8;
                for (int32_t i = 0; i < nf; i++)
                    if (frames[i].optBytes != 0) { frames[i].opt = q; q += frames[i].optBytes; }
                uint32_t bits = sub + optSum * 8u;
                s.cur += bits >> 5;
                s.pos = (int32_t)(bits & 31);
            }
        }
        // 13220 reads and writes retail's raw layout (frame stride 0x2C, bottomUp at +0x28; box nframes at +0x1C), so it runs
        // on a raw copy; the native DccFrame / DccBox (8-byte pointers) take its result. Reading the native box there read nframes
        // at the wrong offset and skipped the whole frame walk.
        uint8_t rawFrames[256 * 0x2C];
        uint8_t rawBox[0x30];
        memset(rawBox, 0, sizeof rawBox);
        for (int32_t i = 0; i < nf; i++) {
            uint8_t* rf = rawFrames + i * 0x2C;
            const DccFrame* f = &frames[i];
            memcpy(rf + 0x00, &f->var0, 4); memcpy(rf + 0x04, &f->w, 4); memcpy(rf + 0x08, &f->h, 4);
            memcpy(rf + 0x0C, &f->xoff, 4); memcpy(rf + 0x10, &f->yoff, 4);
            memcpy(rf + 0x28, &f->bottomUp, 4);
        }
        d2cmp_13220(lh, d, reinterpret_cast<DccFrame*>(rawFrames), reinterpret_cast<DccBox*>(rawBox), env->fatal);
        DccBox box;
        box.hdr = lh; box.frames = frames;
        memcpy(&box.xmin, rawBox + 0x08, 4); memcpy(&box.ymin, rawBox + 0x0C, 4);
        memcpy(&box.w, rawBox + 0x10, 4); memcpy(&box.h, rawBox + 0x14, 4);
        memcpy(&box.dir, rawBox + 0x18, 4); memcpy(&box.nframes, rawBox + 0x1C, 4);
        d2cmp_14020(&box);
        memset(env->cells, 0, (size_t)box.cells * sizeof(DccCell));
        uint32_t eqSize = 0, encSize = 0, rawSize = 0;
        if (flags & 2) eqSize = dcc_read(&s, 20);
        uint32_t maskSize = dcc_read(&s, 20);
        if (flags & 1) { encSize = dcc_read(&s, 20); rawSize = dcc_read(&s, 20); }
        uint32_t cm[8];
        for (int32_t i = 0; i < 8; i++) cm[i] = dcc_read(&s, 32);
        uint8_t mapA[0x104], mapB[0x104];
        d2cmp_13a30(cm, mapA, mapB);

        DccBits eq = s, eq1 = s, mask, enc, raw, code;
        uint32_t b = (uint32_t)s.pos + eqSize;
        bits_at(&mask, s, b);
        b = (uint32_t)mask.pos + maskSize; bits_at(&enc, mask, b);
        b = (uint32_t)enc.pos + encSize;   bits_at(&raw, enc, b);
        b = (uint32_t)raw.pos + rawSize;   bits_at(&code, raw, b);
        int32_t bitsUsed = 0;
        DccPixBuf pb = {env->pixbuf, 0};
        d2cmp_13b70(&code, &box, env->cells, eqSize ? &eq1 : nullptr, &mask, encSize ? &enc : nullptr,
                    rawSize ? &raw : nullptr, &pb, &bitsUsed);
        // The pixel-code stream after stage 1: its start position + bitsUsed (esp+0x44 struct).
        DccBits code2;
        { DccBits c0; bits_at(&c0, raw, (uint32_t)raw.pos + rawSize);
          bits_at(&code2, c0, (uint32_t)c0.pos + (uint32_t)bitsUsed); }
        for (int32_t i = 0; i < pb.len; i++) env->pixbuf[i] = mapB[env->pixbuf[i]];

        for (int32_t i = 0; i < nf; i++) {
            DccFrame* f = &frames[i];
            int32_t x = f->xoff - box.xmin;
            int32_t y = f->yoff - (f->bottomUp ? 0 : f->h) - box.ymin;
            if (x < 0) env->fatal(0x62b);
            if (y < 0) env->fatal(0x62c);
            f->pix = env->canvas + box.w * y + x;
            f->stride = box.w;
        }
        uint8_t* cursor = env->pixbuf;
        if (box.nframes != 0) {
            DccIter it;
            memset(&it, 0, sizeof it);
            it.frame = -1;
            if (d2cmp_13300(&it, &box)) {
                memset(env->cells, 0, 0x6ddd * 4);
                DccBits* eq2 = eqSize ? &eq : nullptr;
                int32_t last = 0, more;
                do {
                    uint8_t* before = cursor;
                    DccCell* c = &env->cells[it.cell];
                    DccCell tmp = {before, it.pix + it.off, it.stride, it.cw, it.ch};
                    d2cmp_13780(&code2, eq2, c, &tmp, &cursor);
                    *c = tmp;
                    c->colors = before;
                    more = d2cmp_13300(&it, &box);
                    if (it.frame != last) {
                        DccFrame* f = &frames[last];
                        int32_t h32[8] = {f->var0, f->w, f->h, f->xoff, f->yoff, 0, 0, (int32_t)f->coded};
                        memcpy(dst, h32, sizeof h32);
                        if (f->pix == nullptr) env->fatal(0x65a);
                        if (!d2cmp_13620(dst + 0x20, f, dst, env->log)) {
                            env->fileError(name);
                            env->fatal(0x66e);
                        }
                        uint32_t a, c7;
                        memcpy(&a, dst + 0x14, 4); memcpy(&c7, dst + 0x1c, 4);
                        dst += a + c7 + 0x23;
                        last = it.frame;
                    }
                } while (more);
            }
        }
        uint32_t p = ((uint32_t)code2.pos + 7u) & ~7u;
        s.start = code2.start;
        s.cur = code2.cur + (p >> 5);
        s.pos = (int32_t)(p & 31);
    }
    env->unlock();
    return block;
}
