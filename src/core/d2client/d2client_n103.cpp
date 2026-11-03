// 1.13c D2Client+0x316C0: skill-description line formatter (stdcall, 8 stack args, ret 0x20).
// Ported case by case from the x86 (the decompile loses the register args of the sibling formatters).
// Sibling formatters, D2Lang #10003 and the string primitives stay behind host seams (Ops); the
// register arguments each sibling reads are carried in Regs and the stack arguments left to right.
#include <cstdint>
#include <initializer_list>
#include <cstring>
namespace d2client_n103 {
typedef uintptr_t R;
typedef char16_t W;
struct Regs { R eax = 0, ecx = 0, edx = 0, ebx = 0, esi = 0, edi = 0; };
struct Ops {
    void* ctx;
    const uint8_t* (*rec17F0)(void*, int16_t skill);               // 0x017F0, eax = skill id
    const uint8_t* (*rec1580)(void*, int16_t skill);               // 0x01580, eax = skill id
    R (*calc)(void*, R unit, R expr, R skill, R lvl);              // Ordinal_10435 (via 0xCEE0)
    R (*dmgMin)(void*, R unit, R skill, R lvl, R one);             // Ordinal_10567 (0xC85C)
    R (*dmgMax)(void*, R unit, R skill, R lvl, R one);             // Ordinal_10297 (0xC7F0)
    R (*sib)(void*, uint32_t rva, const Regs&, const R* stk, int n);
    const W* (*lang)(void*, uint16_t id);                          // D2Lang #10003 (0xD1EC)
    void (*strcat)(void*, W* dst, const W* src);
    void (*strcpy)(void*, W* dst, const W* src);
    int (*wsprintf)(void*, size_t cap, W* dst, const W* fmt, R a, R b); // 0x6fb7eab4
};
static const uint16_t NONE = 0x1506;
static int16_t s16(const uint8_t* p, unsigned o) { int16_t v; memcpy(&v, p + o, 2); return v; }
static uint32_t u32(const uint8_t* p, unsigned o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void zero(W* b, int n) { for (int i = 0; i < n; i++) b[i] = 0; }
// wsprintfA("%d") then widen
static void putInt(W* d, int32_t v) {
    char t[12]; int n = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[n++] = char('0' + u % 10); u /= 10; } while (u);
    int i = 0; if (v < 0) d[i++] = '-';
    while (n) d[i++] = (W)t[--n];
    d[i] = 0;
}
static void putRange(W* d, int32_t a, int32_t b) {   // "%d-%d"
    putInt(d, a); int n = 0; while (d[n]) n++; d[n++] = '-'; putInt(d + n, b);
}
struct Ctx {
    const Ops& o; R a0; W* dst; R a2, a3, a4, a5, a7; R v4, v5; uint32_t B, D;
    const W* L(uint32_t id) { return o.lang(o.ctx, (uint16_t)id); }
    void cat(W* d, const W* s) { o.strcat(o.ctx, d, s); }
    void catL(uint32_t id) { cat(dst, L(id)); }
    void catIf(uint32_t id) { if ((uint16_t)id != NONE) catL(id); }
    R sib(uint32_t rva, const Regs& r, std::initializer_list<R> s) {
        R t[8]; int n = 0; for (R x : s) t[n++] = x; return o.sib(o.ctx, rva, r, t, n);
    }
    void colour(W* buf, R v) { Regs r; r.edi = (R)buf; R t[1] = {v}; o.sib(o.ctx, 0xBF5F0, r, t, 1); }
    void tail(const W* x) { cat(dst, x); catL(0xf9e); }
    // 0x12 tail: BF5F0 v4 if >0, strcat, strcpy lang(f9e), BF5F0 a7, strcat
    void colourTail(W* buf) {
        if ((int32_t)v4 > 0) colour(buf, v4);
        cat(dst, buf);
        o.strcpy(o.ctx, buf, L(0xf9e)); colour(buf, a7); cat(dst, buf);
    }
};
}
using namespace d2client_n103;
// returns 1 on success, 0 otherwise
uint32_t d2client_sub_316C0(const Ops& o, R a0, W* dst, const uint8_t* a2, R a3, R a4, R a5, R idx, R a7) {
    if (!a2) return 0;
    const uint8_t* rec = o.rec17F0(o.ctx, (int16_t)a3);
    if (!rec) return 0;
    Ctx c{o, a0, dst, (R)a2, a3, a4, a5, a7, 0, 0, 0, 0};
    c.v4 = o.calc(o.ctx, a0, u32(rec, 0x98 + idx * 4), a3, a4);
    c.v5 = o.calc(o.ctx, a0, u32(rec, 0xdc + idx * 4), a3, a4);
    c.B = (uint16_t)s16(rec, 0x54 + idx * 2);
    c.D = (uint16_t)s16(rec, 0x76 + idx * 2);
    const uint32_t B = c.B, D = c.D;
    const R v4 = c.v4, v5 = c.v5;
    const uint8_t sw = rec[0x42 + idx];
    Regs r;
    W buf256[256], buf100[100], buf32[32];
    auto cat89 = [&]() { c.catIf(B); c.catIf(D); };
    switch (sw) {
    case 1: r.eax = a5; r.ecx = (R)a2; c.sib(0x314D0, r, {(R)dst, a3, a4, v4}); return 1;
    case 2: case 3: r.ebx = (R)dst; c.sib(0x2D9B0, r, {B, D, v4, (R)(sw == 2)}); return 1;
    case 4: case 5: r.ebx = (R)dst; c.sib(0x2DCA0, r, {B, v4, (R)(sw == 4)}); return 1;
    case 6: case 7: r.ebx = (R)dst; c.sib(0x2DBF0, r, {B, v4, (R)(sw == 6)}); return 1;
    case 8: cat89(); r.eax = a4; r.ecx = a3; r.edx = a0; c.sib(0x2EEF0, r, {(R)dst}); return 1;
    case 9: cat89(); r.eax = a4; r.ebx = v4; c.sib(0x2D000, r, {a0, (R)dst, a3, v5}); return 1;
    case 0xa: cat89(); r.ebx = a3; r.edi = a4; c.sib(0x2EF80, r, {a0, (R)dst, (R)a2, 0}); return 1;
    case 0xb: cat89(); r.eax = a4; r.ecx = a3; r.edx = a0; c.sib(0x2F2E0, r, {(R)dst, (R)a2}); return 1;
    case 0xc: c.catIf(D); r.eax = 0; c.sib(0x2E060, r, {(R)dst, B, v4}); return 1;
    case 0xd: r.eax = (R)(intptr_t)s16(a2, 0xbc); c.sib(0x2EFD0, r, {(R)dst, v4, v5}); return 1;
    case 0xe: cat89(); r.eax = a4; r.ecx = a3; c.sib(0x2EC60, r, {a0, (R)dst, (R)a2}); return 1;
    case 0xf: c.catL(B); c.catL(0xf9d); c.catL(0xf9b); c.tail(c.L(D)); return 1;
    case 0x10: r.ebx = (R)dst; c.sib(0x2DF50, r, {0x10b0, v4, v5}); return 1;
    case 0x11: c.catIf(D); r.eax = (R)dst; c.sib(0x2F6E0, r, {B, 0x10be, v4, v5, 0}); return 1;
    case 0x12:
        zero(buf256, 256); o.strcpy(o.ctx, buf256, c.L(B)); c.colourTail(buf256); return 1;
    case 0x13: c.catIf(D); r.eax = v4; c.sib(0x2F430, r, {(R)dst, B}); return 1;
    case 0x14: case 0x15: r.edi = (R)dst; c.sib(0x2D8E0, r, {B, 0x10b3, D, v4, (R)(sw == 0x14)}); return 1;
    case 0x16: r.esi = (R)(intptr_t)s16(rec, 0x3c); r.eax = a4; r.ecx = a0; c.sib(0x2EDD0, r, {(R)dst, (R)a2}); return 1;
    case 0x17: {
        const uint8_t* r2 = o.rec1580(o.ctx, s16(rec, 0x3c)); if (!r2) return 0;
        R val = (R)(intptr_t)((int32_t)s16(r2, 0x98) * (int32_t)a4 + s16(r2, 0x96));
        r.eax = 0; c.sib(0x2E060, r, {(R)dst, B, val}); return 1; }
    case 0x18: c.catIf(D); r.ebx = a3; r.edi = a4; c.sib(0x2EF80, r, {a0, (R)dst, (R)a2, B}); return 1;
    case 0x19:
        zero(buf256, 256); o.strcpy(o.ctx, buf256, c.L(B));
        if ((uint16_t)D != NONE) c.cat(buf256, c.L(D));
        c.colourTail(buf256); return 1;
    case 0x1a: cat89(); r.eax = a0; r.esi = a4; r.edi = a3; c.sib(0x2EB80, r, {(R)dst, (R)a2}); return 1;
    case 0x1b: cat89(); r.eax = a3; r.edi = a4; c.sib(0x2ED80, r, {a0, (R)dst}); return 1;
    case 0x1c: {
        const uint8_t* r2 = o.rec1580(o.ctx, s16(rec, 0x3c)); if (!r2) return 0;
        r.eax = u32(r2, 0x38); c.sib(0x2F430, r, {(R)dst, 0x10ae}); return 1; }
    case 0x1d: {
        const uint8_t* r2 = o.rec1580(o.ctx, s16(rec, 0x3c)); if (!r2) return 0;
        int32_t val = ((int32_t)s16(r2, 0x98) * ((int32_t)a4 - 1) + s16(r2, 0x96)) * 2 / 3;
        r.ebx = (R)dst; c.sib(0x2DBF0, r, {0x10b6, (R)(intptr_t)val, 0}); return 1; }
    case 0x1e: {
        const uint8_t* r2 = o.rec1580(o.ctx, s16(rec, 0x3c)); if (!r2) return 0;
        const uint8_t* r3 = o.rec1580(o.ctx, s16(r2, 0x18)); if (!r3) return 0;
        R val = (R)(intptr_t)((int32_t)s16(r3, 0x98) * (int32_t)a4 + s16(r3, 0x96));
        r.eax = 0; c.sib(0x2E060, r, {(R)dst, B, val}); return 1; }
    case 0x1f: {
        R val = c.sib(0x2E330, r, {});
        r.eax = 0; c.sib(0x2E060, r, {(R)dst, B, val}); return 1; }
    case 0x20: c.catL(B); r.ebx = (R)dst; c.sib(0x2D9B0, r, {D, 0x10b3, v4, 1}); return 1;
    case 0x21: if ((int32_t)a4 <= 1) return 1; c.catL(B); c.catL(D); c.catL(0xf9e); return 1;
    case 0x22: c.sib(0x2E410, r, {a0, (R)dst, a3, (R)a2, a4}); return 1;
    case 0x23:
        c.catL(B); c.catL(0xf9d); c.catL(0xf9b);
        zero(buf32, 32); putRange(buf32, (int32_t)v4, (int32_t)v5);
        c.cat(dst, buf32); c.catL(0xf9e); return 1;
    case 0x24: r.ebx = v4; c.sib(0x2DA70, r, {(R)dst, B, D, 0}); return 1;
    case 0x25: r.eax = v4; c.sib(0x2F340, r, {(R)dst, B}); return 1;
    case 0x26: r.eax = (R)dst; c.sib(0x2F6E0, r, {B, D, v4, v5, 0}); return 1;
    case 0x27: c.sib(0x2E900, r, {a0, (R)dst, a3, (R)a2, a4}); return 1;
    case 0x28: case 0x2e:
        if (sw == 0x2e && v5 == 0) return 0;
        if ((uint16_t)B == NONE) return 0;
        zero(buf256, 256); o.wsprintf(o.ctx, 0x100, buf256, c.L(B), (R)c.L(D), 0);
        c.colourTail(buf256); return 1;
    case 0x29: c.sib(0x2E770, r, {(R)dst, (R)a2, v4, v5}); return 1;
    case 0x2a: r.edi = (R)dst; c.sib(0x2CD20, r, {D, v4, v5, 1, 0}); return 1;
    case 0x2b: r.ebx = 0xa; c.sib(0x2DE00, r, {(R)dst, B, D, v4, v5}); return 1;
    case 0x2c: r.ebx = 0x64; c.sib(0x2DE00, r, {(R)dst, B, D, v4, v5}); return 1;
    case 0x2d: r.eax = a3; r.ecx = a4; r.edx = (R)dst; c.sib(0x2E6E0, r, {}); return 1;
    case 0x2f: r.eax = (R)dst; c.sib(0x2F520, r, {B, v4, v5, 0, 0}); return 1;
    case 0x30: r.eax = a4; r.esi = a0; c.sib(0x30270, r, {(R)dst, a3, (R)a2}); return 1;
    case 0x31: {
        R lo = (R)(intptr_t)((int32_t)o.dmgMin(o.ctx, a0, a3, a4, 1) >> 8);
        R hi = (R)(intptr_t)((int32_t)o.dmgMax(o.ctx, a0, a3, a4, 1) >> 8);
        r.eax = (R)dst; c.sib(0x2F520, r, {B, lo, hi, 1, 1}); return 1; }
    case 0x32: r.eax = a4; r.ebx = (R)(intptr_t)s16(rec, 0x3c); c.sib(0x2EF20, r, {a0, (R)dst, B}); return 1;
    case 0x33: r.ebx = (R)dst; c.sib(0x2DD50, r, {B, v4}); return 1;
    case 0x34: r.eax = (R)dst; c.sib(0x2F6E0, r, {B, D, v4, v5, 1}); return 1;
    case 0x35: case 0x36: case 0x37: case 0x38: {
        static const uint32_t f[4] = {0x3CF50, 0x3CF00, 0x3CEB0, 0x3CE60};
        r.eax = a0; r.ebx = a3;
        R v = c.sib(f[sw - 0x35], r, {B});
        c.sib(0x2C950, Regs(), {v}); return 1; }
    case 0x39: r.eax = 1; c.sib(0x2E060, r, {(R)dst, B, v4}); return 1;
    case 0x3a: r.eax = (R)dst; c.sib(0x2F600, r, {B, D, v4, v5, 1}); return 1;
    case 0x3e: r.eax = (R)dst; c.sib(0x2F600, r, {B, D, v4, v5, 0}); return 1;
    case 0x3b: c.catIf(D); r.eax = B; c.sib(0x2D220, r, {(R)dst, v4, v5, (R)(uint16_t)s16(a2, 0x1dc)}); return 1;
    case 0x3c: case 0x3d: {
        R x = v5 ? v5 : 10; r.ebx = v4; c.sib(0x2F870, r, {(R)dst, B, D, x, (R)(sw == 0x3c)}); return 1; }
    case 0x3f: r.edi = (R)dst; c.sib(0x2CE10, r, {B, D, v4, 1, 1}); return 1;
    case 0x40: r.edi = (R)dst; c.sib(0x2CC00, r, {B, D, v4, v5, 1}); return 1;
    case 0x41: r.edi = (R)dst; c.sib(0x2CB60, r, {B, D}); return 1;
    case 0x42: r.ebx = (R)dst; c.sib(0x2C9F0, r, {B, v4}); return 1;
    case 0x43: r.edi = (R)dst; c.sib(0x2CE10, r, {B, D, v4, 1, 0}); return 1;
    case 0x44: r.ebx = (R)dst; c.sib(0x2DB30, r, {B, D, v4, 0}); return 1;
    case 0x45: r.edi = (R)dst; c.sib(0x2CA80, r, {D, v4, 0, 0}); return 1;
    case 0x46: c.catL(B); r.ebx = (R)dst; c.sib(0x2DCA0, r, {D, v4, 1}); return 1;
    case 0x47:
        if ((uint16_t)B == NONE || (uint16_t)D == NONE) return 0;
        zero(buf100, 100); o.wsprintf(o.ctx, 100, buf100, c.L(D), v4, 0);
        c.catL(B); c.catL(0xf9d); c.catL(0xf9b); c.tail(buf100); return 1;
    case 0x48: case 0x49: {
        if (v4 == 0 || (int32_t)v5 <= 0) return 0;
        zero(buf32, 32);
        if (sw == 0x48) { r.eax = v4; r.ecx = 1; c.sib(0x2C7E0, r, {(R)buf32}); }
        else putInt(buf32, (int32_t)v4);
        c.cat(dst, buf32); c.catL(0xfa0);
        zero(buf32, 32); putInt(buf32, (int32_t)v5); c.cat(dst, buf32);
        c.catL(0xf9b); c.tail(c.L(B)); return 1; }
    case 0x4a: case 0x4b:
        if ((uint16_t)B == NONE) return 0;
        zero(buf100, 100); o.wsprintf(o.ctx, 100, buf100, c.L(B), v4, sw == 0x4b ? v5 : 0);
        c.tail(buf100); return 1;
    default: return 0;
    }
}
