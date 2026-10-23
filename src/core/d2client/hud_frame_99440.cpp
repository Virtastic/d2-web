// D2Client rva 0x99440: per-frame bottom-of-screen UI frame draw
// (loading/menu panel quads, button row, status text, hover tooltip). Called from 0xC39E0.
// Globals are a flat state struct, every callee is injected (env-seam style). Retail ctx for the
// D2Win cell-draw ordinals is a zeroed 0x12-dword block: [0]=frame, [13]=cellfile; kept as such.
#include <cstdint>
#include <cstring>

struct D2Client99440Btn { uint32_t cell, xa, xb, ya, yb; };  // draw at (xa+xb, ya+yb)

struct D2Client99440State {
    uint32_t state;            // 0x18BC34
    uint32_t tickDeadline;     // 0x18BC4C
    uint32_t c54;              // 0x18BC54
    uint32_t c9721;            // 0x189721 (dword read)
    uint32_t c971d;            // 0x18971D (dword read, unit id)
    uint32_t bc48;             // 0x18BC48 (byte)
    const void* plr;           // 0x18BBFC (host pointer, may be null)
    uint32_t plr60;            // dword at plr+0x60 (read when plr != null)
    uint32_t mouseX, mouseY;   // 0x18B828, 0x18B824
    uint32_t c3a4;             // 0x18C3A4
    uint32_t aade8;            // 0x16ADE8
    uint32_t d5d0;             // 0x14D5D0 (must be 0x26)
    uint32_t bd349c, bd3498;   // 0x193... flags
    uint32_t c418, c414, c434; // 0x18C418, 0x18C414, 0x18C434
    uint32_t a9e14, a9e18, c3c4;
    uint32_t bc74, bc84, bc78; // cellfiles 0x18BC74, 0x18BC84, 0x18BC78
    uint32_t c9854;            // 0x189854
    uint32_t bc58, bc5c, bc60, bc64, bc70; // loading-spinner state
    uint32_t be8c, be94, be84; // frame bases
    uint32_t b9a0, b9a4;       // 0x18B9A0, 0x18B9A4 (screen origin)
    uint32_t screenW, screenH; // 0x14BC48, 0x14BC4C
    uint32_t a7c8;             // cellfile 0x16A7C8
    uint32_t bc94;             // 0x18BC94
    uint32_t bec8;             // cellfile
    uint32_t bbb35c;           // cellfile 0x17B35C
    uint32_t be60, be74, be5c, be64, be68, bedc;
    uint32_t c77fc, c77ec;
    uint16_t bfed8[0x17];      // unicode name string at 0x18FED8 (copied, 0x17 elems)
    D2Client99440Btn btnEd0a, btnEd0b;  // c94 pair (frame 0 and 1)
    struct { uint32_t id, frame; D2Client99440Btn b; } btn[8]; // test-id buttons
    uint32_t aXa, aXb, aYa, aYb;   // block A  : (91160+bd04, 91164+bd10)
    uint32_t bXa, bXb, bYa, bYb;   // block B  : (91158+bcf0, 9115c+bcfc)
};

struct D2Client99440Callees {
    uint32_t (*tick)();
    void (*c3f710)(uint32_t bc48);
    void (*c45c60)();
    void (*c3cfb0)(uint32_t);
    void (*c458a0)(const void* plr);
    void (*c453b0)();
    void (*c3f260)();
    void (*c43450)(const void* unitOrPlr, uint32_t flag);
    uint32_t (*c6eaf0)();
    int32_t (*c3beb0)();
    int32_t (*c3bee0)();
    int32_t (*c3b650)(int32_t x, int32_t y);
    int32_t (*c3b600)(int32_t x, int32_t y);
    void (*c408c0)();
    void (*cadb360)();
    int32_t (*ord10227)();
    int32_t (*ord10051)(const void* plr, uint32_t plr60);       // == 2 test
    int32_t (*testFlag)(uint32_t plr60, uint32_t n);            // Ordinal_11139
    uint32_t (*levelId)(const void* plr);                       // Ordinal_11060 (0xC910)
    const uint16_t* (*getString)(uint32_t id);                  // Ordinal_10003
    int32_t (*textWidth)(const uint16_t*);                      // Ordinal_10028
    void (*drawText)(const uint16_t*, int32_t x, int32_t y, uint32_t a, uint32_t b);   // Ordinal_10150
    void (*drawText2)(const uint16_t*, int32_t x, int32_t y, uint32_t a, uint32_t b);  // Ordinal_10085
    void (*setFont)(uint32_t);                                  // Ordinal_10184
    void (*ord10006)(uint32_t);
    void (*usprintf)(uint16_t* buf, uint32_t n, const uint16_t* fmt, uint32_t arg);
    void (*csprintf)(char* buf, uint32_t n, uint32_t fmtVa, uint32_t arg);  // Ordinal_578
    void (*win2Unicode)(uint16_t* dst, const char* src, uint32_t n);
    const char* (*c4d850)();
    // fn: 10041 or 10019
    void (*drawCell)(uint32_t fn, const uint32_t* ctx, int32_t x, int32_t y, int32_t color, uint32_t mode, uint32_t last);
    int32_t (*c54d40)();
    void (*c3a4d0)(uint32_t ecx, uint32_t eax, uint32_t esi);
    void (*c47840)();
    uint32_t (*ord10048)(uint32_t strVa);
    void (*ecc60)(uint32_t eax, uint32_t handle);
    void (*eccc0)(uint32_t eax, uint32_t handle);
    void (*c6e2e0)();
    void (*c49a0)(uint32_t al, uint32_t ecx);
    const void* (*findUnit)(uint32_t id, uint32_t* type);       // 0x1BA808[id&0x7f] chain by +0xE4
    void (*fatal)(uint32_t code);
};

static void trim(const D2Client99440Callees& k, uint16_t* b, uint32_t len) {
    if (k.textWidth(b) > 0x70) {
        uint16_t* p = b + len;
        do { *p = 0; --p; } while (k.textWidth(b) > 0x70);
    }
}

// 1.13c D2Client+0x99440
extern "C" void d2_client_99440(D2Client99440State& s, const D2Client99440Callees& k) {
    const int32_t mx = (int32_t)s.mouseX, my = (int32_t)s.mouseY;
    if (s.state == 10) return;
    if (s.state >= 1 && s.state <= 9) {
        if (k.tick() > s.tickDeadline) s.tickDeadline = 0;
        if (s.c54 == 1 && s.tickDeadline == 0 && s.c9721 != 0) { k.c3f710(s.bc48 & 0xff); s.c54 = 0; }
        k.c45c60();
        k.c3cfb0(2);
    }
    uint32_t ctx[0x12] = {};
    auto draw = [&](uint32_t frame, int32_t x, int32_t y, uint32_t mode = 5, uint32_t last = 0) {
        ctx[0] = frame; k.drawCell(10041, ctx, x, y, -1, mode, last); };
    auto draw19 = [&](uint32_t frame, int32_t x, int32_t y, uint32_t last) {
        ctx[0] = frame; k.drawCell(10019, ctx, x, y, -1, 5, last); };
    const int32_t a0 = (int32_t)s.b9a0, a4 = (int32_t)s.b9a4, W = (int32_t)s.screenW, H = (int32_t)s.screenH;
    auto quad = [&](uint32_t cell) {  // 4 frames: TL TR BL BR of the 512x? panel
        ctx[13] = cell;
        draw(0, a0, a4 + H - 0xe0); draw(1, a0 + 0x100, a4 + H - 0xe0);
        draw(2, a0, a4 + H - 0x30); draw(3, a0 + 0x100, a4 + H - 0x30);
    };
    auto withBc48 = [&](uint32_t v) { const uint32_t o = s.bc48; s.bc48 = v; k.c458a0(s.plr); s.bc48 = o; };
    const uint32_t st = s.state;
    auto labelAt = [&](uint32_t id, int32_t x, int32_t y) {  // centred-ish tooltip via 10085
        (void)id; (void)x; (void)y; };
    (void)labelAt;
    bool skipRest = false;
    if (st >= 0xc && st <= 0xd) {
        if (s.bc74 != 0) quad(s.c9854 == 0 ? s.bc74 : s.bc84);
        const uint32_t lvl = k.levelId(s.plr);
        uint16_t buf[100] = {};
        k.usprintf(buf, 100, k.getString(0xfd3), lvl);
        k.drawText(buf, a0 + 0x4e, s.c9854 ? a4 + H - 0x19f : a4 + H - 0xdb, 0, 0);
        withBc48(4);
        ctx[13] = k.c6eaf0();
        ctx[0] = s.be84 + 10;
        k.drawCell(10041, ctx, a0 + (s.c9854 ? 0x110 : 0x113), k.c3beb0(), -1, 5, 0);
        k.c3cfb0(0);
        const int32_t lo = a0 + (s.c9854 ? 0x110 : 0x113);
        if (!(mx < lo) && !(mx > k.c3bee0() + 0x28)) {
            const int32_t yy = k.c3beb0();
            if (!(my < yy - 0x23) && !(my > k.c3beb0() + 5)) {
                const uint16_t* t = k.getString(0x1030);
                k.drawText2(t, 0xc - (k.textWidth(t) - (k.textWidth(t) >> 31)) / 2 + k.c3bee0(), k.c3beb0() - 0x23, 0, 0);
            }
        }
    } else if (st == 0xe) {
        if (s.c3a4 != 0 || k.c54d40() != 0) {
            const uint32_t sv = s.aade8;
            k.ord10227();
            if (s.d5d0 != 0x26) { k.fatal(0x139b); return; }
            s.aade8 = 0;
            if (sv != 0) {
                s.c418 = 0; s.c414 = 0;
                k.ord10006(0);
                k.c3a4d0(s.a9e14, s.a9e18, s.c3c4);
                k.c47840();
                k.ecc60(7, k.ord10048(0x6fb8be18));
                k.c6e2e0();
                s.c434 = 1;
                if (s.bd349c == 0) {
                    s.bd349c = 1;
                    if (s.bd3498 != 0) { k.ecc60(2, k.ord10048(0x6fba51ec)); s.bd3498 = 0; }
                    k.eccc0(4, k.ord10048(0x6fba51bc));
                }
            }
            s.c418 = 0; s.c414 = 0;
            k.ord10006(0);
            k.c3a4d0(s.a9e14, s.a9e18, s.c3c4);
            k.c49a0(0x4f, 0x17);
            return;
        }
        if (s.bc78 != 0) quad(s.bc78);
        if (s.bc58 == 0 || s.bc5c >= 0xe) withBc48(3);
        ctx[13] = k.c6eaf0();
        draw(s.be8c + 10, a0 + 0x113, a4 + H - 0x41);
        ctx[13] = s.bc70;
        draw(s.be94, a0 + 0x90, a4 + H - 0xbc);
        if (s.bc58 != 0) {
            const uint32_t t = k.tick();
            bool d = true;
            if (!(t - s.bc60 <= 0x46)) {
                s.bc60 = t; s.bc5c++;
                if (s.bc5c >= 0x1e) {
                    s.bc58 = 0;
                    if (s.bc64 != 0) { k.cadb360(); s.bc64 = 0; }
                    d = false;
                }
            }
            if (d) {
                ctx[0] = s.bc5c; ctx[13] = s.bc64;
                k.drawCell(10041, ctx, (W - (W >> 31)) >> 1, ((H - (H >> 31)) >> 1) - 1, -1, 3, 0);
            }
        }
        k.setFont(1);
        if (k.c3b650(mx, my)) {
            const uint16_t* t = k.getString(0x1030);
            k.drawText2(t, a0 - (k.textWidth(t) - (k.textWidth(t) >> 31)) / 2 + 0x121, a4 + H - 0x64, 0, 0);
        } else if (k.c3b600(mx, my)) {
            const uint16_t* t = k.getString(0xd0d);
            k.drawText2(t, a0 - (k.textWidth(t) - (k.textWidth(t) >> 31)) / 2 + 0x9e, a4 + H - 0xdf, 0, 0);
        }
    }
    (void)skipRest;
    // bottom bar quads, frames 4..7
    ctx[13] = s.a7c8;
    draw(4, W - a0 - 0x140, a4 + H - 0xe0); draw(5, W - a0 - 0x40, a4 + H - 0xe0);
    draw(6, W - a0 - 0x140, a4 + H - 0x30); draw(7, W - a0 - 0x40, a4 + H - 0x30);
    k.c408c0();
    if (s.bc94 != 0 && k.ord10227() != 0 && s.c9854 != 0) {
        ctx[13] = s.btnEd0a.cell; draw(0, s.btnEd0a.xa + s.btnEd0a.xb, s.btnEd0a.ya + s.btnEd0a.yb);
        ctx[13] = s.btnEd0b.cell; draw(1, s.btnEd0b.xa + s.btnEd0b.xb, s.btnEd0b.ya + s.btnEd0b.yb);
    }
    for (auto& b : s.btn)
        if (!k.testFlag(s.plr60, b.id)) {
            ctx[13] = b.b.cell; draw(b.frame, b.b.xa + b.b.xb, b.b.ya + b.b.yb);
        }
    int32_t ox = 0, oy = 0;
    if (s.c9854 != 0 && s.bc94 != 0) { ox = 4; oy = -4; }
    const int32_t r4 = k.testFlag(s.plr60, 4), r5 = k.testFlag(s.plr60, 5);
    const bool two = k.ord10051(s.plr, s.plr60) == 2;
    const int32_t Ax = s.aXa + s.aXb, Ay = s.aYa + s.aYb, Bx = s.bXa + s.bXb, By = s.bYa + s.bYb;
    auto cellA = [&](int32_t dx, int32_t dy) { ctx[13] = s.bec8; draw(0, Ax + dx, Ay + dy); };
    auto cellB = [&](int32_t dx, int32_t dy) { ctx[13] = s.bec8; draw(0, Bx + dx, By + dy); };
    if (!k.testFlag(s.plr60, 5) && (r4 == 0 || !two)) cellA(ox, oy);
    if (!k.testFlag(s.plr60, 4) && (r5 == 0 || !two)) cellB(ox, oy);
    if (r4 == 0) { if (r5 != 0 && !k.testFlag(s.plr60, 5)) cellA(0, 0); }
    else if (r5 == 0) { if (!k.testFlag(s.plr60, 4)) cellB(0, 0); }
    else if (!two) {
        if (!k.testFlag(s.plr60, 5)) cellA(0, 0);
        if (!k.testFlag(s.plr60, 4)) cellB(0, 0);
    }
    if (s.state == 0xb && s.bbb35c != 0) {
        quad(s.bbb35c);
        ctx[13] = k.c6eaf0();
        const int32_t px = a0 + 0x113, py = a4 + H - 0x116;
        if (s.be60 == 0) draw(0x10, px, py); else draw19(0x11, px, py, 2);
        const int32_t bx = a0 + 0xdf, by = a4 + H - 0x40;
        if (s.be74 != 0) draw19(0x10, bx, by, 1);
        else if (s.be5c == 0 && s.be64 == 0) draw(0x10, bx, by);
        else draw19(0x11, bx, by, 2);
        draw(s.be68 == 1 ? 0xb : 0xa, a0 + 0x113, a4 + H - 0x40);
        withBc48(1); withBc48(2);
        k.setFont(1);
        uint16_t n1[0x17] = {}, n2[0x17 + 0x19] = {};
        if (s.plr) { const char* p = k.c4d850(); if (p) k.win2Unicode(n1, p, 0x17); }
        std::memcpy(n2, s.bfed8, sizeof(s.bfed8));
        trim(k, n2, 0x17);
        k.drawText(n2, a0 + 0x19, 0x1b - a4, 0, 0);
        std::memset(n2, 0, sizeof(n2)); std::memcpy(n2, n1, sizeof(n1));
        trim(k, n2, 0x17);
        k.drawText(n2, a0 + 0x19, 0xf2 - a4, 0, 0);
        char cb[0x10]; uint16_t nb[0xf] = {};
        auto lab = [&](int32_t y, uint32_t val) {
            const uint16_t* t = k.getString(0x8a7); const int32_t w = k.textWidth(t);
            k.drawText(k.getString(0x8a7), a0 - ((w - (w >> 31)) >> 1) + 0xbb, y, 0, 0);
            k.csprintf(cb, 0xf, 0x6fb8057c, val);
            std::memset(nb, 0, sizeof(nb)); k.win2Unicode(nb, cb, 0xf);
            k.drawText(nb, a0 - k.textWidth(nb) + 0x124, y, 0, 0);
        };
        lab(0x1b - a4, s.c77fc);
        lab(0xf2 - a4, s.c77ec);
    }
    withBc48(0);
    k.c453b0();
    if (s.state >= 1 && s.state <= 9) k.c3f260();
    k.c3cfb0(1);
    ctx[13] = k.c6eaf0();
    draw((s.bedc != 0) + 10, W - a0 - 0x12e, a4 + H - 0x40);
    const void* tgt = s.plr; uint32_t flag = 0;
    if (s.state >= 1 && s.state <= 9 && mx < ((W - (W >> 31)) >> 1) && s.c9721 != 0) {
        uint32_t type = 0;
        const void* u = k.findUnit(s.c971d, &type);
        if (!u) { k.c43450(s.plr, 0); return; }
        if (type != 1) { k.fatal(0x5a); return; }
        tgt = u; flag = 1;
    }
    k.c43450(tgt, flag);
}
