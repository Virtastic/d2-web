// 1.13c D2Client+0x99440: per-frame bottom-of-screen panel / HUD draw. No arguments;
// called from D2Client+0xC39E0. The globals are the state struct below (one dword per retail global, the RVA in the
// comment: the retail capture tool/fn/d2client-99440.gen.py reads these comments to pin them). Every callee is one `hook` call
// with the register / stack-word values retail passes it (rva, eax, ecx, edx, ebx, esi, s0..s3): a D2Win cell draw is
// (rva, frame, cellfile, last | x, y, -1, mode). Pointers are opaque dwords: a local buffer is BUF, a module string is
// MOD(rva), a blob / unit pointer is the handle the host gave. No float. The Unicode ctor loops over the local
// buffers (IAT 0xCEB00) only clear stack words nothing reads back, so they are not modelled.
#include <cstddef>
#include <cstdint>

struct D2Client99440State {
    uint32_t state;      // 0x11bc34
    uint32_t mouseX;     // 0x11b828
    uint32_t mouseY;     // 0x11b824
    uint32_t a0;         // 0x11b9a0 screen origin x
    uint32_t a4;         // 0x11b9a4 screen origin y
    uint32_t W;          // 0xdbc48
    uint32_t H;          // 0xdbc4c
    uint32_t c9854;      // 0x119854
    uint32_t c9721;      // 0x119721 (dword read at an odd address)
    uint32_t c971d;      // 0x11971d (unit id)
    uint32_t plr;        // 0x11bbfc player pointer (opaque handle)
    uint32_t bc48;       // 0x11bc48 len 1
    uint32_t bc4c;       // 0x11bc4c
    uint32_t bc54;       // 0x11bc54
    uint32_t bc58;       // 0x11bc58
    uint32_t bc5c;       // 0x11bc5c
    uint32_t bc60;       // 0x11bc60
    uint32_t bc64;       // 0x11bc64
    uint32_t bc70;       // 0x11bc70
    uint32_t bc74;       // 0x11bc74
    uint32_t bc78;       // 0x11bc78
    uint32_t bc84;       // 0x11bc84
    uint32_t bc94;       // 0x11bc94
    uint32_t a7c8;       // 0x11a7c8
    uint32_t b35c;       // 0x10b35c
    uint32_t be5c;       // 0x11be5c
    uint32_t be60;       // 0x11be60
    uint32_t be64;       // 0x11be64
    uint32_t be68;       // 0x11be68
    uint32_t be74;       // 0x11be74
    uint32_t be84;       // 0x11be84
    uint32_t be8c;       // 0x11be8c
    uint32_t be94;       // 0x11be94
    uint32_t beb4;       // 0x11beb4
    uint32_t beb8;       // 0x11beb8
    uint32_t bebc;       // 0x11bebc
    uint32_t bec0;       // 0x11bec0
    uint32_t bec4;       // 0x11bec4
    uint32_t bec8;       // 0x11bec8
    uint32_t bed0;       // 0x11bed0
    uint32_t bedc;       // 0x11bedc
    uint32_t c3a4;       // 0x11c3a4
    uint32_t c3c4;       // 0x11c3c4
    uint32_t c414;       // 0x11c414
    uint32_t c418;       // 0x11c418
    uint32_t c434;       // 0x11c434
    uint32_t aade8;      // 0xfade8
    uint32_t d5d0;       // 0xdd5d0 (must be 0x26)
    uint32_t a9e14;      // 0xf9e14
    uint32_t a9e18;      // 0xf9e18
    uint32_t d3498;      // 0x123498
    uint32_t d349c;      // 0x12349c
    uint32_t c77ec;      // 0x1177ec
    uint32_t c77fc;      // 0x1177fc
    uint32_t hud[24];    // 0xe1118 button origins (x,y pairs)
    uint32_t bcb4;       // 0x11bcb4
    uint32_t bcc0;       // 0x11bcc0
    uint32_t bcc8;       // 0x11bcc8
    uint32_t bcd4;       // 0x11bcd4
    uint32_t bcdc;       // 0x11bcdc
    uint32_t bce8;       // 0x11bce8
    uint32_t bcf0;       // 0x11bcf0
    uint32_t bcfc;       // 0x11bcfc
    uint32_t bd04;       // 0x11bd04
    uint32_t bd10;       // 0x11bd10
    uint32_t bd18;       // 0x11bd18
    uint32_t bd24;       // 0x11bd24
    uint32_t bd2c;       // 0x11bd2c
    uint32_t bd38;       // 0x11bd38
    uint32_t bd40;       // 0x11bd40
    uint32_t bd4c;       // 0x11bd4c
    uint32_t bd54;       // 0x11bd54
    uint32_t bd60;       // 0x11bd60
    uint32_t bd68;       // 0x11bd68
    uint32_t bd74;       // 0x11bd74
};
static_assert(sizeof(D2Client99440State) == (54 + 24 + 20) * 4, "state layout");
static_assert(offsetof(D2Client99440State, hud) == 54 * 4, "hud table follows the scalar globals");

struct D2Client99440Callees {
    uint32_t (*hook)(uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);   // one retail callee, returns eax
    uint32_t (*tick)();                                                       // GetTickCount
    uint32_t (*rd32)(uint32_t handle, uint32_t off);                          // dword at handle+off ([plr+0x60], unit fields)
    uint32_t (*hashHead)(uint32_t idx);                                       // unit hash table 0x10a808[idx]
    void (*fatal)(uint32_t code);                                             // Fog 10265 + exit: not reached by a valid state
};

namespace {
constexpr uint32_t BUF = 0xB3000000u;              // a local buffer (stack pointer)
constexpr uint32_t MOD(uint32_t rva) { return 0xB2000000u | rva; }
}

// 1.13c D2Client+0x99440
extern "C" void d2_client_99440(D2Client99440State& s, const D2Client99440Callees& k) {
    typedef uint32_t U;
    auto H = [&](U rva, U a = 0, U c = 0, U d = 0, U b = 0, U esi = 0, U s0 = 0, U s1 = 0, U s2 = 0, U s3 = 0) -> U {
        return k.hook(rva, a, c, d, b, esi, s0, s1, s2, s3); };
    const int32_t mx = (int32_t)s.mouseX, my = (int32_t)s.mouseY;   // saved at entry
    if (s.state == 10) return;
    const U st = s.state;
    if (st >= 1 && st <= 9) {
        if (k.tick() > s.bc4c) s.bc4c = 0;
        if (s.bc54 == 1 && s.bc4c == 0 && s.c9721 != 0) { H(0x8f710, 0, 0, 0, 0, 0, s.bc48 & 0xff); s.bc54 = 0; }
        H(0x95c60);
        H(0x8cfb0, 0, 0, 0, 0, 0, 2);
    }
    U ctxFrame = 0, ctxCell = 0;
    // D2Win cell draw: ordinal 10041 (RVA 0xd096) or 10019 (0xd0e4), ctx = {[0]=frame, [13]=cellfile}
    auto cell = [&](U fn, U x, U y, U mode, U last) { H(fn, ctxFrame, ctxCell, last, 0, 0, x, y, 0xFFFFFFFFu, mode); };
    auto draw = [&](U frame, U x, U y, U mode = 5, U last = 0) { ctxFrame = frame; cell(0xd096, x, y, mode, last); };
    auto draw19 = [&](U frame, U x, U y, U last) { ctxFrame = frame; cell(0xd0e4, x, y, 5, last); };
    const U a0 = s.a0, a4 = s.a4, W = s.W, Hh = s.H;
    auto quad = [&](U c) {
        ctxCell = c;
        draw(0, a0, a4 + Hh - 0xe0); draw(1, a0 + 0x100, a4 + Hh - 0xe0);
        draw(2, a0, a4 + Hh - 0x30); draw(3, a0 + 0x100, a4 + Hh - 0x30);
    };
    auto withBc48 = [&](U v) { const U o = s.bc48 & 0xff; s.bc48 = v; H(0x958a0, 0, 0, 0, 0, 0, s.plr); s.bc48 = o; };
    auto half = [](int32_t w) { return (U)((w - (w >> 31)) >> 1); };
    auto getString = [&](U id) { return H(0xd1ec, 0, id); };
    auto textWidth = [&](U str) { return (int32_t)H(0xd35a, 0, str); };
    auto drawText = [&](U str, U x, U y) { H(0xd37e, 0, str, x, 0, 0, y, 0, 0); };
    auto drawText2 = [&](U str, U x, U y) { H(0xd384, 0, str, x, 0, 0, y, 0, 0); };
    auto setFont1 = [&]() { H(0xd3a8, 0, 1); };
    if (st == 0xc || st == 0xd) {
        if (s.bc74 != 0) quad(s.c9854 ? s.bc84 : s.bc74);
        const U lvl = H(0xc910, 0, 0, 0, 0, 0, s.plr);
        const U str = getString(0xfd3);
        H(0xceab4, 0, 0, 0, 0, 0, 0x64, BUF, str, lvl);
        drawText(BUF, a0 + 0x4e, s.c9854 ? a4 + Hh - 0x19f : a4 + Hh - 0xdb);
        withBc48(4);
        ctxCell = H(0xbeaf0);
        ctxFrame = s.be84 + 10;
        const U bx = a0 + (s.c9854 ? 0x110 : 0x113);
        cell(0xd096, bx, H(0x8beb0), 5, 0);
        H(0x8cfb0, 0, 0, 0, 0, 0, 0);
        const int32_t lo = (int32_t)(a0 + (s.c9854 ? 0x110 : 0x113));
        if (!(mx < lo)) {
            const int32_t r = (int32_t)H(0x8bee0);
            if (!(mx > r + 0x28)) {
                const int32_t y = (int32_t)H(0x8beb0);
                if (!(my < y - 0x23)) {
                    const int32_t y2 = (int32_t)H(0x8beb0);
                    if (!(my > y2 + 5)) {
                        const U t = getString(0x1030);
                        const U ty = H(0x8beb0) - 0x23;
                        const int32_t w = textWidth(t);
                        drawText2(t, (0xc - half(w)) + H(0x8bee0), ty);
                    }
                }
            }
        }
    } else if (st == 0xe) {
        if (s.c3a4 != 0 || H(0xa4d40) != 0) {
            const U sv = s.aade8;
            H(0xbf8c);
            if (s.d5d0 != 0x26) { k.fatal(0x139b); return; }
            s.aade8 = 0;
            if (sv != 0) {
                s.c418 = 0; s.c414 = 0;
                H(0xd1b0, 0, 0);
                H(0x8a4d0, s.a9e18, s.a9e14, 0, 0, s.c3c4, 0);
                H(0x97840);
                U h = H(0xd0b4, 0, 0, 0, 0, 0, MOD(0xdbe18));
                H(0x3cc60, 7, 0, 0, 0, 0, h);
                H(0xbe2e0);
                s.c434 = 1;
                if (s.d349c == 0) {
                    s.d349c = 1;
                    if (s.d3498 != 0) {
                        h = H(0xd0b4, 0, 0, 0, 0, 0, MOD(0xf51ec));
                        H(0x3cc60, 2, 0, 0, 0, 0, h);
                        s.d3498 = 0;
                    }
                    h = H(0xd0b4, 0, 0, 0, 0, 0, MOD(0xf51bc));
                    H(0x3ccc0, 4, 0, 0, 0, 0, h);
                }
            }
            s.c418 = 0; s.c414 = 0;
            H(0xd1b0, 0, 0);
            H(0x8a4d0, s.a9e18, s.a9e14, 0, 0, s.c3c4, 0);
            H(0x149a0, 0x4f, 0x17, 0, 0, 0, 0);
            return;
        }
        if (s.bc78 != 0) quad(s.bc78);
        if (s.bc58 == 0 || s.bc5c >= 0xe) withBc48(3);
        ctxCell = H(0xbeaf0);
        draw(s.be8c + 10, a0 + 0x113, a4 + Hh - 0x41);
        ctxCell = s.bc70;
        draw(s.be94, a0 + 0x90, a4 + Hh - 0xbc);
        if (s.bc58 != 0) {
            const U t = k.tick();
            bool d = true;
            if (!(t - s.bc60 <= 0x46)) {
                s.bc60 = t; s.bc5c++;
                if (!(s.bc5c < 0x1e)) {
                    s.bc58 = 0;
                    if (s.bc64 != 0) { H(0x2b360); s.bc64 = 0; }
                    d = false;
                }
            }
            if (d) {
                ctxCell = s.bc64;
                draw(s.bc5c, half((int32_t)W), half((int32_t)Hh) - 1, 3, 0);
            }
        }
        setFont1();
        if (H(0x8b650, s.mouseX, 0, s.mouseY)) {
            const U t = getString(0x1030);
            const int32_t w = textWidth(t);
            drawText2(t, a0 - half(w) + 0x121, a4 + Hh - 0x64);
        } else if (H(0x8b600, s.mouseX, 0, s.mouseY)) {
            const U t = getString(0xd0d);
            const int32_t w = textWidth(t);
            drawText2(t, a0 - half(w) + 0x9e, a4 + Hh - 0xdf);
        }
    }
    // bottom-bar quad, frames 4..7
    ctxCell = s.a7c8;
    draw(4, W - a0 - 0x140, a4 + Hh - 0xe0); draw(5, W - a0 - 0x40, a4 + Hh - 0xe0);
    draw(6, W - a0 - 0x140, a4 + Hh - 0x30); draw(7, W - a0 - 0x40, a4 + Hh - 0x30);
    H(0x908c0);
    const U plr = s.plr;
    const U plr60 = k.rd32(plr, 0x60);
    auto flag = [&](U id) { return H(0xc262, 0, 0, 0, 0, 0, plr60, id); };
    if (s.bc94 != 0 && H(0xbf8c) != 0 && s.c9854 != 0) {
        ctxCell = s.bed0;
        draw(0, s.hud[22] + s.bd04, s.hud[23] + s.bd10);
        draw(1, s.hud[20] + s.bcf0, s.hud[21] + s.bcfc);
    }
    struct Btn { U id, cell, frame, x, y; };
    const Btn btn[8] = {
        { 3, s.beb4, 0, s.hud[0] + s.bcdc, s.hud[1] + s.bce8 },
        { 8, s.beb8, 0, s.hud[2] + s.bd40, s.hud[3] + s.bd4c },
        { 9, s.bebc, 0, s.hud[4] + s.bd54, s.hud[5] + s.bd60 },
        { 1, s.bec0, 1, s.hud[6] + s.bcb4, s.hud[7] + s.bcc0 },
        { 0xa, s.bec0, 0, s.hud[8] + s.bd68, s.hud[9] + s.bd74 },
        { 2, s.bec4, 0, s.hud[10] + s.bcc8, s.hud[11] + s.bcd4 },
        { 6, s.bec4, 1, s.hud[14] + s.bd18, s.hud[15] + s.bd24 },
        { 7, s.bec4, 1, s.hud[12] + s.bd2c, s.hud[13] + s.bd38 },
    };
    for (const Btn& b : btn)
        if (flag(b.id) == 0) { ctxCell = b.cell; draw(b.frame, b.x, b.y); }
    // flag block: two cells A (id 5) / B (id 4), offset (4,-4) when both c9854 and bc94
    const U ox = (s.c9854 != 0 && s.bc94 != 0) ? 4u : 0u, oy = (s.c9854 != 0 && s.bc94 != 0) ? (U)-4 : 0u;
    const U Ax = s.hud[18] + s.bd04, Ay = s.hud[19] + s.bd10, Bx = s.hud[16] + s.bcf0, By = s.hud[17] + s.bcfc;
    auto cellA = [&](U dx, U dy) { ctxCell = s.bec8; draw(0, Ax + dx, Ay + dy); };
    auto cellB = [&](U dx, U dy) { ctxCell = s.bec8; draw(0, Bx + dx, By + dy); };
    const U r4 = flag(4), r5 = flag(5);
    const bool two = H(0xc89e, 0, 0, 0, 0, 0, plr, plr60) == 2;
    if (flag(5) == 0 && (r4 == 0 || !two)) cellA(ox, oy);
    if (flag(4) == 0 && (r5 == 0 || !two)) cellB(ox, oy);
    if (r4 == 0) { if (r5 != 0 && flag(5) == 0) cellA(0, 0); }
    else if (r5 == 0) { if (flag(4) == 0) cellB(0, 0); }
    else if (!two) {
        if (flag(5) == 0) cellA(0, 0);
        if (flag(4) == 0) cellB(0, 0);
    }
    if (st == 0xb && s.b35c != 0) {
        quad(s.b35c);
        ctxCell = H(0xbeaf0);
        if (s.be60 != 0) draw19(0x11, a0 + 0x113, a4 + Hh - 0x116, 2);
        else draw(0x10, a0 + 0x113, a4 + Hh - 0x116);
        if (s.be74 != 0) draw19(0x10, a0 + 0xdf, a4 + Hh - 0x40, 1);
        else if (s.be5c == 0 && s.be64 == 0) draw(0x10, a0 + 0xdf, a4 + Hh - 0x40);
        else draw19(0x11, a0 + 0xdf, a4 + Hh - 0x40, 2);
        draw(s.be68 == 1 ? 0xb : 0xa, a0 + 0x113, a4 + Hh - 0x40);
        withBc48(1); withBc48(2);
        setFont1();
        if (plr != 0) { const U p = H(0x9d850); if (p != 0) H(0xceac0, 0, BUF, p, 0, 0, 0x17); }
        // name lines: strcpy into A, trim until the width is <= 0x70 (the buffer bytes are never read back)
        auto trim = [&]() {
            U len = H(0xceab0, 0, BUF);
            if (textWidth(BUF) > 0x70) { do { --len; } while (textWidth(BUF) > 0x70); }
        };
        H(0xceab8, 0, BUF, MOD(0x11fed8));
        trim();
        drawText(BUF, a0 + 0x19, 0x1b - a4);
        H(0xceab8, 0, BUF, BUF);
        trim();
        drawText(BUF, a0 + 0x19, 0xf2 - a4);
        auto label = [&](U y) {
            const U t = getString(0x8a7);
            const int32_t w = textWidth(t);
            drawText(getString(0x8a7), a0 - half(w) + 0xbb, y);
        };
        auto value = [&](U y, U v) {
            H(0xbea2, 0, 0, 0, 0, 0, BUF, 0xf, MOD(0xd057c), v);
            H(0xceac0, 0, BUF, BUF, 0, 0, 0xf);
            const int32_t w = textWidth(BUF);
            drawText(BUF, a0 - (U)w + 0x124, y);
        };
        label(0x1b - a4);
        value(0x1b - a4, s.c77fc);
        label(0xf2 - a4);
        value(0xf2 - a4, s.c77ec);
    }
    withBc48(0);
    H(0x953b0);
    if (st >= 1 && st <= 9) H(0x8f260);
    H(0x8cfb0, 0, 0, 0, 0, 0, 1);
    ctxCell = H(0xbeaf0);
    draw((s.bedc != 0) + 10, W - a0 - 0x12e, a4 + Hh - 0x40);
    if (st >= 1 && st <= 9 && mx < (int32_t)half((int32_t)W) && s.c9721 != 0) {
        U n = k.hashHead(s.c971d & 0x7f);
        while (n != 0 && k.rd32(n, 0xc) != s.c971d) n = k.rd32(n, 0xe4);
        if (n == 0) { H(0x93450, 0, 0, 0, 0, 0, plr, 0); return; }
        if (k.rd32(n, 0) != 1) { k.fatal(0x5a); return; }
        H(0x93450, 0, 0, 0, 0, 0, n, 1);
        return;
    }
    H(0x93450, 0, 0, 0, 0, 0, plr, 0);
}
