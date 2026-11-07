// D2Client+0xC0D30: hover-text dispatcher (caller 0xC39E0); env-seam style. Every ordinal and internal callee is a seam
// (register inputs named as the disassembly passes them); the position rule, the per-type routing and the string
// assembly of types 1, 2 and 5 are in this file.
#include <cstdint>
#include <emscripten.h>
typedef uint16_t W;
struct D2cC0D30Env {
    int32_t mode;               // D2Client+0x11C414 (3 = hidden; 1/2 shift the anchor by W/4)
    int32_t scrW;               // DAT_6fb8bc48
    int32_t mouseX, mouseY;     // DAT_6fbc995c / DAT_6fbc9960
    int32_t datCc418;           // DAT_6fbcc418
    int32_t cc394;              // DAT_6fbcc394
    int32_t flag9721, flag3a68; // DAT_6fbc9721 / DAT_6fbc3a68 (type 2 suppressors)
    int32_t baadb4;             // DAT_6fbaadb4 (type 4 gate)
    int32_t c0a0;               // DAT_6fbcc0a0 (table-loaded flag)
    uint8_t byte5591;           // DAT_6fbc5591
    int32_t player;             // DAT_6fbcbbfc
    int32_t* outX; int32_t* outY;   // DAT_6fb8bc54 / DAT_6fb8bc58
    uint16_t* wCa30; uint16_t* wC578; uint16_t* wC780;   // cleared by type 1
    int32_t (*bubbleLimit)(const int32_t* p, int32_t pl);                 // 0xC190 (Ordinal_10522)
    int32_t (*kindSwitch)();                                              // 0xD07E (Ordinal_10013)
    int32_t (*anchorA)(const int32_t* p);                                 // 0xC1AE (Ordinal_11080)
    int32_t (*anchorB)(const int32_t* p);                                 // 0xC232 (Ordinal_11168)
    int32_t (*anchorOk)(int32_t a, int32_t b);                            // 0xD09C (Ordinal_10012)
    void (*anchorPos)(int32_t a, int32_t b, int32_t z, int32_t* x, int32_t* y);   // 0xD084 (Ordinal_10023)
    int32_t (*posX)(const int32_t* p);                                    // 0xC1D2 (Ordinal_10651)
    int32_t (*posY)(const int32_t* p);                                    // 0xC1F6 (Ordinal_11142)
    void (*b70450)(const int32_t* p, int32_t* out1, int32_t* out2, W* buf, int32_t n);   // 0xC0450: EAX=&out2, ECX=p
    void (*drawBox)(const W* s, int32_t x, int32_t y, int32_t out1, int32_t one, int32_t out2);   // 0xD360 (Ordinal_10070)
    void (*b6fec0)(const int32_t* p, int32_t a, int32_t b);               // 0xBFEC0: EAX=p
    const uint8_t* (*f1230)();                                            // 0xAB1230
    const W* (*getString)(int32_t id);                                    // 0xD1EC (Ordinal_10003)
    void (*c22c)(const int32_t* p, const W* s);                           // 0xC22C (Ordinal_10971): ECX=p, EDX=string
    void (*b707c0)(const int32_t* p, int32_t z);                          // 0xC07C0: EAX=p
    int32_t (*b70390)(const int32_t* p, int32_t* out1, W* buf, int32_t* out2);   // 0xC0390: ECX=p, EDI=&out1
    void (*b70850)(const int32_t* p);                                     // 0xC0850
    const W* (*f55d90)(const int32_t* p);                                 // 0xA5D90: EAX=p
    int32_t (*c1b4)(const int32_t* p);                                    // 0xC1B4 (Ordinal_11059)
    const uint8_t* (*c226)(int32_t idx);                                  // 0xC226 (Ordinal_10688)
    const W* (*b6e240)();                                                 // 0xBE240
    const char* (*f4ce00)(int32_t id);                                    // 0x9CE00 (ECX=p[3])
    void (*win2uni)(W* dst, const char* src, int32_t n);                  // Unicode::win2Unicode
    void (*drawText)(const W* s, int32_t x, int32_t y, int32_t zero, int32_t one);   // 0xD384 (Ordinal_10085): ECX=string, EDX=x
    int32_t (*c1f0)(const int32_t* p);                                    // 0xC1F0 (Ordinal_10331)
    int32_t (*c1de)(int32_t pl);                                          // 0xC1DE (Ordinal_10826)
    const uint8_t* (*c214)(int32_t lvl);                                  // 0xC214 (Ordinal_10014)
    const W* (*lvlStr)(const uint8_t* lvlRec);                            // 0xD1DA (Ordinal_10011): ECX=rec+0xf5
    int32_t (*c1a2)(int32_t pl, int32_t id);                              // 0xC1A2 (Ordinal_10093)
    const int32_t* (*c1e4)(int32_t pl, const int32_t* p);                 // 0xC1E4 (Ordinal_10633)
    void (*fatal)(int32_t code);                                          // Ordinal_10265 + 10024 + exit(-1)
    const W* (*lvlWName)(const uint8_t* lvlRec);                          // level record + 0x1be wide name
};
static void wcpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
static void wcat(W* d, const W* s) { while (*d) d++; wcpy(d, s); }
static void wncpy(W* d, const W* s, int32_t n) { int32_t i = 0; for (; i < n && s[i]; i++) d[i] = s[i]; for (; i < n; i++) d[i] = 0; }
extern "C" {
// 1.13c D2Client+0xC0D30
EMSCRIPTEN_KEEPALIVE void d2c_c0d30(const D2cC0D30Env* e, const int32_t* p) {
    if (e->mode == 3 || !p) return;
    int32_t t = p[0];
    if (t == 5 && e->bubbleLimit(p, e->player) > 100) return;
    int32_t x, y;
    if (!e->kindSwitch()) {
        x = e->posX(p) - (e->mouseY - e->datCc418);
        y = e->posY(p) - (e->mouseX - 8);
    } else {
        int32_t a = e->anchorA(p), b = e->anchorB(p);
        if (!e->anchorOk(a, b)) return;
        int32_t ox = 0, oy = 0;
        e->anchorPos(a, b, 0, &ox, &oy);
        int32_t q = (e->scrW + ((e->scrW >> 31) & 3)) >> 2;
        if (e->mode == 1) ox -= q; else if (e->mode == 2) ox += q;
        x = ox; y = oy + 0x10;
    }
    *e->outX = x; *e->outY = y;
    switch (t) {
    case 0: {
        W buf[0x80] = {0};
        int32_t o1 = 0, o2 = 0;
        e->b70450(p, &o1, &o2, buf, 0x80);
        e->drawBox(buf, x, y - 0x48, o1, 1, o2);
        return;
    }
    case 1: {
        int32_t id = p[1];
        if (id == 0x1a1 || id == 0x1a2) {
            const uint8_t* r = e->f1230();
            if (!r) { *e->wC578 = 0; *e->wC780 = 0; *e->wCa30 = 0; return; }
            e->c22c(p, e->getString((int32_t)(uint16_t)(r[6] | (r[7] << 8))));
        } else if (((uint32_t)p[0xc4 / 4] >> 9 & 1) == 0) {
            if (id == 0x164) {
                W buf[0x80] = {0};
                int32_t o1 = 3, o2 = 0;
                if (e->b70390(p, &o1, buf, &o2)) {
                    if (o1 == 3) return;
                    e->drawBox(buf, x, y - 0x48, o1, 1, o2);
                    return;
                }
            }
            e->b70850(p);
            return;
        }
        e->b707c0(p, 0);
        *e->wCa30 = 0; *e->wC578 = 0; *e->wC780 = 0;
        return;
    }
    case 2: {
        W bufA[0x80] = {0}, bufB[0x100] = {0}, bufC[0x80] = {0};
        if (e->flag9721 || e->flag3a68) return;
        const W* s = e->f55d90(p);
        int32_t dy = e->c1b4(p);
        const uint8_t* rec = e->c226(p[1] & 0xffff);
        uint8_t fl = rec[0x167];
        if (fl & 4) {
            W w[0x80] = {0};
            wcpy(bufC, e->b6e240());
            if (e->cc394 != 0 && e->cc394 != 1) {
                const char* nm = e->f4ce00(p[3]);
                if (!nm) {
                    nm = (const char*)(intptr_t)p[5] + 0x28;
                    if (!nm[0]) { e->drawText(bufC, *e->outX, *e->outY + dy, 0, 1); return; }
                }
                char tmp[0x80]; int32_t n = 0;
                tmp[n++] = ' '; tmp[n++] = '(';
                for (; *nm && n < 0x7c; nm++) tmp[n++] = *nm;
                tmp[n++] = ')'; tmp[n++] = ' '; tmp[n] = 0;
                e->win2uni(w, tmp, 0x80);
                wcat(bufC, w);
            }
            e->drawText(bufC, *e->outX, *e->outY + dy, 0, 1);
            return;
        }
        if ((int8_t)fl < 0) return;
        if (!(fl & 0x40)) { e->drawText(s, *e->outX, *e->outY + dy, 0, 1); return; }
        int32_t pl = e->c1f0(p);
        if (!pl) return;
        const uint8_t* lv = e->c214(e->c1de(pl));
        wcpy(bufB, s);
        wcat(bufB, e->getString(0xf9b));
        wcat(bufB, e->getString(0xf9e));
        wcat(bufB, e->lvlStr(lv));
        e->drawText(bufB, *e->outX, *e->outY + dy, 0, 1);
        (void)bufA;
        return;
    }
    case 4:
        if (e->baadb4) return;
        e->b6fec0(p, 0, 0);
        return;
    case 5: {
        int32_t pl = e->c1f0(p);
        int32_t a = e->c1a2(pl, p[1]);
        const uint8_t* lv = e->c214(a);
        if (!lv) return;
        const int32_t* r = e->c1e4(pl, p);
        if (!r) return;
        int32_t px = (int32_t)((uint32_t)r[3] >> 1) + r[1] + *e->outX;
        int32_t py = (int32_t)((uint32_t)r[4] >> 1) + r[2] + *e->outY;
        W buf[0xc8] = {0};
        const W* src = e->lvlWName(lv);
        if (a == 8) {
            if (!e->c0a0) { e->fatal(0x60); return; }
            src = e->getString(e->byte5591 ? 0x13de : 0x13dd);
        }
        wncpy(buf, src, 0xc7);
        e->drawText(buf, px, py, 0, 1);
        return;
    }
    default: return;
    }
}
}
