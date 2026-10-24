// D2Client batch n85: STALLED rows recovered from the decompile. 32-bit game addresses are
// uint32_t handles read/written through seams (rd32/wr32/rd8/wr8); Win32/CRT/Ordinal services
// are function-pointer seams too. No game data lives here.
#include <cstdint>
#include <cstddef>
#include <cstring>

struct N85Env {
    uint32_t (*rd32)(uint32_t a);
    void (*wr32)(uint32_t a, uint32_t v);
    uint8_t (*rd8)(uint32_t a);
    void (*wr8)(uint32_t a, uint8_t v);
    uint32_t (*ord)(uint32_t ordinal, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f); // Ordinal_<n> / D2 import thunks
    uint32_t (*call)(uint32_t rva, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g); // D2Client+rva internal callee
    uint32_t (*tick)(void);               // GetTickCount
    void (*fatal)(uint32_t strId);        // Ordinal_10265(id) -> Ordinal_10024 -> _exit(-1)
};
static N85Env g_n85;
extern "C" void d2c_n85_bind(const N85Env* e) { g_n85 = *e; }
#define R(a) g_n85.rd32((uint32_t)(a))
#define W(a, v) g_n85.wr32((uint32_t)(a), (uint32_t)(v))
#define ORD(n, ...) g_n85.ord(n, __VA_ARGS__)

// --- unit / LCG helpers (seed u64 at +0x20: lo*0x6ac690c5 + hi) ---
static uint32_t n85_rng(uint32_t unit) {
    uint64_t s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
    W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
    return (uint32_t)s;
}
static void n85_setMode(uint32_t unit, uint32_t v) {
    if (unit && R(unit) == 1 && R(unit + 0x14)) W(R(unit + 0x14) + 0x30, v);
}
static bool n85_dynPos(uint32_t t) { return t == 2 || (t > 3 && t < 6); }
static uint32_t n85_posX(uint32_t u) { // 0x10 / Ordinal_10750
    uint32_t p = R(u + 0x2c);
    if (n85_dynPos(R(u))) return R(p + 0x10);
    return p ? ORD(10750, p, 0, 0, 0, 0, 0) : 0;
}
static uint32_t n85_posY(uint32_t u) { // 0xc / Ordinal_10867
    uint32_t p = R(u + 0x2c);
    if (n85_dynPos(R(u))) return R(p + 0xc);
    return p ? ORD(10867, p, 0, 0, 0, 0, 0) : 0;
}

// 1.13c D2CLIENT+0x0BE84
extern "C" uint32_t d2c_BE84_thunk511(void) { return ORD(511, 0, 0, 0, 0, 0, 0); }
// 1.13c D2CLIENT+0x0D1D4
extern "C" uint32_t d2c_D1D4_thunk10003(void) { return ORD(10003, 0, 0, 0, 0, 0, 0); }

// 1.13c D2CLIENT+0x09752  MSVCRT ___sbh_resize_block(header, block, newsize) -> 1 resized in place / 0 not
// header: [0]=bitvecHi [1]=bitvecLo [3]=region base [4]=group array.
extern "C" uint32_t d2c_9752_sbh_resize_block(uint32_t hdr, uint32_t blk, int32_t newSize) {
    uint32_t grp = R(hdr + 16);
    uint32_t idx = (blk - R(hdr + 12)) >> 15;
    uint32_t list = idx * 0x204 + 0x144 + grp;
    uint32_t want = ((uint32_t)newSize + 0x17) & 0xfffffff0;
    int32_t have = (int32_t)R(blk - 4) - 1;
    uint32_t nxt = R(blk - 4) - 5 + blk;
    uint32_t nsz = R(nxt);
    auto unlink_acct = [&](uint32_t bucket) {
        // block `nxt` leaves its free list; clear group/region bits when the bucket empties
        if (R(nxt + 4) == R(nxt + 8)) {
            uint32_t m = ~(0x80000000u >> ((bucket < 0x20 ? bucket : bucket - 0x20) & 0x1f));
            uint32_t bm = (bucket < 0x20 ? 0x44 : 0xc4) + idx * 4 + grp;
            W(bm, R(bm) & m);
            uint32_t cnt = grp + 4 + bucket;
            g_n85.wr8(cnt, (uint8_t)(g_n85.rd8(cnt) - 1));
            if (g_n85.rd8(cnt) == 0) { uint32_t h = hdr + (bucket < 0x20 ? 0 : 4); W(h, R(h) & m); }
        }
    };
    auto link_free = [&](uint32_t node, uint32_t bucket) {
        uint32_t head = list + bucket * 8;
        W(node + 4, R(head + 4)); W(node + 8, head); W(head + 4, node); W(R(node + 4) + 8, node);
        if (R(node + 4) == R(node + 8)) {
            uint8_t c = g_n85.rd8(grp + 4 + bucket);
            g_n85.wr8(grp + 4 + bucket, (uint8_t)(c + 1));
            uint32_t b = bucket, bm;
            if (b < 0x20) { if (!c) W(hdr, R(hdr) | (0x80000000u >> (b & 0x1f))); bm = grp + 0x44 + idx * 4; }
            else { if (!c) W(hdr + 4, R(hdr + 4) | (0x80000000u >> ((b - 0x20) & 0x1f))); bm = grp + 0xc4 + idx * 4; b -= 0x20; }
            W(bm, R(bm) | (0x80000000u >> (b & 0x1f)));
        }
    };
    if (have < (int32_t)want) {
        if ((nsz & 1) || (int32_t)(nsz + have) < (int32_t)want) return 0;
        uint32_t b = ((int32_t)nsz >> 4) - 1; if (b > 0x3f) b = 0x3f;
        unlink_acct(b);
        W(R(nxt + 8) + 4, R(nxt + 4)); W(R(nxt + 4) + 8, R(nxt + 8));
        int32_t rest = (int32_t)(nsz + (have - want));
        if (rest > 0) {
            uint32_t nb = ((rest >> 4) - 1); if (nb > 0x3f) nb = 0x3f;
            uint32_t node = blk - 4 + want;
            link_free(node, nb);
            W(node, rest); W(node + rest - 4, rest);
        }
        W(blk - 4, want + 1); W(blk - 8 + want, want + 1);
    } else if ((int32_t)want < have) {
        int32_t rest = have - (int32_t)want;
        W(blk - 4, want + 1);
        uint32_t node = blk - 4 + want;
        W(node - 4, want + 1);
        if (!(nsz & 1)) {
            uint32_t b = ((int32_t)nsz >> 4) - 1; if (b > 0x3f) b = 0x3f;
            unlink_acct(b);
            W(R(nxt + 8) + 4, R(nxt + 4)); W(R(nxt + 4) + 8, R(nxt + 8));
            rest += (int32_t)nsz;
        }
        uint32_t nb = ((rest >> 4) - 1); if (nb > 0x3f) nb = 0x3f;
        link_free(node, nb);
        W(node, rest); W(node + rest - 4, rest);
    }
    return 1;
}

// --- 0x143E0 outgoing-packet de-duplication (retry window per packet class) ---
struct N85Pkt { uint32_t lastTick; uint8_t last[0x200]; uint32_t bytes; uint32_t count; };
static N85Pkt g_pkt;
// 1.13c D2Client+0x143E0  (buf, len<=0x1ff); sendOrd = Ordinal_10024 seam
extern "C" void d2c_143E0_send_packet(const uint8_t* buf, uint32_t len, uint32_t (*send)(const uint8_t*, uint32_t)) {
    if (len > 0x1ff) g_n85.fatal(0x26d);
    uint32_t now = g_n85.tick();
    bool fast = buf[0] >= 5 && buf[0] <= 0x11 && buf[0] != 0x0b;
    if (buf[0] != ':') {
        uint32_t win = fast ? 0x32 : 200;
        if (now - g_pkt.lastTick < win && memcmp(g_pkt.last, buf, len) == 0) return;
    }
    memcpy(g_pkt.last, buf, len);
    g_pkt.lastTick = now;
    if (send(buf, len)) { g_pkt.bytes += len; g_pkt.count++; }
}

// 1.13c D2CLIENT+0x16A90  (centred text line while in-game state 4, modes 6..7)
struct N85Hud { uint32_t on, stateObj, mode, route, w, y, xoff; };
extern "C" void d2c_16A90_hud_line(const N85Hud* h, uint32_t (*reqFn)(uint32_t), uint32_t (*lookup)(uint32_t obj, uint32_t* rec),
                                   uint32_t (*drawNext)(uint32_t x, uint32_t ctx)) {
    if (!h->on) return;
    if (!h->stateObj || R(h->stateObj) != 4 || h->mode <= 5 || h->mode > 7) { reqFn(h->route); return; }
    uint32_t rec[18] = {0}; ((uint8_t*)rec)[5] = 1;
    if (!lookup(h->stateObj, rec)) return;
    if (!ORD(10005, (uint32_t)(uintptr_t)rec, 0, 1, 0, 0, 0)) return;
    if (!rec[16]) g_n85.fatal(0x275);
    uint32_t half = ORD(10000, rec[16], 0, 0, 0, 0, 0) >> 1;
    uint32_t x = ORD(10060, rec[17], h->w - half, 0, 0, 0, 0) >> 1;
    drawNext((h->y - x) + h->xoff, rec[17]);
}

// 1.13c D2CLIENT+0x176A0
extern "C" void d2c_176A0_unit_action(uint32_t unit) { // 17390 has the identical body
    uint32_t u = R(unit + 0x2c);
    uint32_t a = ORD(10471, 0, 0, 0, 0, 0, 0);
    ORD(11058, u, 0, 0, 0, 0, 0);
    uint32_t arg = u;
    g_n85.call(0xFDC70, a, unit, (uint32_t)(uintptr_t)&arg, 0, 0, 0, 0);
}
// 1.13c D2CLIENT+0x17390 (same body as 176A0)
extern "C" void d2c_17390_unit_action(uint32_t unit) { d2c_176A0_unit_action(unit); }

// AI-state helpers (callee RVAs) used by the 0x17xxx monster-AI handlers
enum { C_7270 = 0x17270, C_7300 = 0x17300, C_73E0 = 0x173E0, C_7500 = 0x17500, C_6C20 = 0x16C20, C_6CB0 = 0x16CB0,
       C_B1330 = 0x1330, C_B1C10 = 0x1C10 };
static uint32_t CALL(uint32_t rva, uint32_t a = 0, uint32_t b = 0) { return g_n85.call(rva, a, b, 0, 0, 0, 0, 0); }

// 1.13c D2CLIENT+0x17710
extern "C" void d2c_17710_ai_idle(uint32_t u, uint32_t flag, uint32_t ecx) {
    uint32_t type = u ? R(u + 16) : 0;
    if (ORD(10648, 0, 0, 0, 0, 0, 0) != 0 || type != 1) return;
    if (g_n85.call(C_B1C10, ecx, 2, 0, 0, 0, 0, 0) == 0) { n85_setMode(u, 0x14); CALL(C_7270, u); return; }
    uint32_t r = n85_rng(u) % 100;
    if (r < 0x28) { n85_setMode(u, 0x14); CALL(C_7270, u); return; }
    if (r < 0x3c && flag) { d2c_176A0_unit_action(u); return; }
    if (r < 0x50 && flag) { CALL(C_7500, u); return; }
    CALL(C_73E0, 8, 1);
}

// 1.13c D2CLIENT+0x17800  (melee approach: blocked -> 17300; else nearest-target search, step toward or wander)
// 6C20 args (seam): unit, predicate rva 0x16E50, &outTarget, &outDist. Walk args: (unit,6,1,0x78) / (6,1,0x78).
extern "C" void d2c_17800_ai_path(uint32_t u) {
    if (!u || R(u + 16) != 1) return;
    uint32_t x = n85_posY(u), y = n85_posX(u); // arg order per asm: +0xC value, then +0x10 value
    uint32_t h = ORD(10331, u, x, y, 0x1c0, 0, 0);
    if ((int16_t)ORD(10482, h, 0, 0, 0, 0, 0) != 0) { CALL(C_7300, u); return; }
    uint32_t out[2] = {0, 0x7fffffff};
    g_n85.call(C_6C20, u, 0x16e50, (uint32_t)(uintptr_t)&out[0], (uint32_t)(uintptr_t)&out[1], 0, 0, 0);
    if (!out[0]) return;
    uint32_t p = R(u + 0x2c);
    ORD(11056, p, 0x3c01, 0, 0, 0, 0);
    if ((int32_t)u < 6) { // as compiled: unit pointer compared with 6 (never true)
        ORD(11056, p, 0xc01, 0, 0, 0, 0);
        if (g_n85.call(C_7500, u, 6, 1, 0x78, 0, 0, 0) == 0) g_n85.call(C_73E0, 6, 1, 0x78, 0, 0, 0, 0);
    } else {
        if ((int32_t)g_n85.call(C_B1330, u + 0x20, 100, 0, 0, 0, 0, 0) < 0x1e) { g_n85.call(C_73E0, 2, 1, 0, 0, 0, 0, 0); return; }
        n85_setMode(u, 5);
    }
}

// 1.13c D2Client+0x17950  (monster AI attack step; ECX = unit, EAX = target unit, stack arg = engage flag)
// Seam call(rva, EAX, ECX, ESI, EDI, stack0, stack1, stack2) for register-convention callees.
extern "C" void d2c_17950_ai_attack(uint32_t u, uint32_t target, uint32_t engage) {
    if (ORD(10648, u, 0, 0, 0, 0, 0) != 0) return;
    if (g_n85.call(C_6CB0, u, 0, 0, target, 0, 0, 0)) { g_n85.call(C_7300, 0, 0, u, target, 0, 0, 0); return; }
    if (!u || R(u + 16) != 1) return;
    if (engage < 2) { g_n85.call(C_7500, target, 0, 0, 0, u, 4, 1); return; }
    if ((int32_t)g_n85.call(C_B1330, 100, u + 0x20, 0, 0, 0, 0, 0) < 0x4b) {
        n85_setMode(u, 0x20);
        g_n85.call(C_7270, 0, 0, u, target, 0, 0, 0);
        return;
    }
    g_n85.call(C_73E0, 0, 0, 0, 0, 4, 1, 0);
}

// 1.13c D2CLIENT+0x17010  (spawn a missile/effect near the unit; returns new object or roll quotient)
extern "C" uint32_t d2c_17010_ai_spawn(uint32_t u, uint32_t enabled) {
    if (!enabled) { n85_setMode(u, 5); return u && R(u) == 1 && R(u + 0x14) ? R(u + 0x14) + 0x30 : 0; }
    uint64_t s = (uint64_t)R(u + 0x20) * 0x6ac690c5ull + R(u + 0x24);
    W(u + 0x20, (uint32_t)s); W(u + 0x24, (uint32_t)(s >> 32));
    if (((uint32_t)s) % 100 > 4) return (uint32_t)s / 100;
    uint64_t s2 = (uint64_t)(uint32_t)s * 0x6ac690c5ull + (uint32_t)(s >> 32);
    W(u + 0x20, (uint32_t)s2); W(u + 0x24, (uint32_t)(s2 >> 32));
    int32_t kind = ((uint32_t)s2 & 3) + 0x23a;
    int32_t cy = (int32_t)n85_posY(u);
    uint32_t r1 = n85_rng(u);
    int32_t cx = (int32_t)n85_posX(u);
    uint32_t r2 = n85_rng(u);
    uint32_t o = g_n85.call(0xAFF10, kind, u, kind, (r1 % 0x3c) - 0x1e + cy, (r2 % 0x3c) - 0x1e + cx, 0, 1);
    if (!o) return 0;
    bool even = (n85_rng(u) & 1) == 0;
    g_n85.call(0xC7E30, even ? 300 : (uint32_t)-100, 0, 0, 0, 0, 0, 0);
    uint32_t d = R(o + 0x54) ? R(R(o + 0x54) + 0x30) : 0;
    if (d) {
        W(d + 0x10, 0); W(d + 0x14, 0); W(d + 0x18, even ? 0xffffd000u : 0x3000u);
        W(d + 0x28, 0); W(d + 0x2c, 0); W(d + 0x30, even ? 0xfffce000u : 0x96000u);
        uint32_t f = R(d); W(d, even ? (f & 0xffffffdfu) : (f | 0x20));
    }
    g_n85.call(0xC7FB0, o, 0, 0, 0, 0, 0, 0);
    ORD(10558, o, 0x3c, 0, 0, 0, 0); ORD(10970, o, 0x3c, 0, 0, 0, 0); ORD(10637, o, 100, 0, 0, 0, 0);
    return ORD(10019, o, 300, 0, 0, 0, 0);
}

// 1.13c D2CLIENT+0x125A0  (list-panel frame; geometry and per-row state selection)
// Ordinal draw calls are seams; globals arrive in N85Panel.
struct N85Panel {
    int32_t scrW, scrH, rowCount, rowBase, selRow, pressed, hover, tag, flash, frame, scrollN, scrollY, scrollHot;
    uint32_t panelTick; int32_t mx, my; uint32_t rowTable;
};
extern "C" void d2c_125A0_panel_frame(N85Panel* p, int32_t (*rowState)(uint32_t kind, int toggle)) {
    int32_t px = (p->scrH - 0x1a4) / 2, py = (p->scrW - 0x26c) / 2;
    ORD(10184, 0, 0, 0, 0, 0, 0);
    for (int i = 0, y = 0; i < p->rowCount && i < 15; i++, y += 0x12) {
        int32_t row = p->rowBase + i, yy = y + 0x22 + px;
        bool both = !(p->pressed && row == p->selRow && (p->frame & 0xf) <= 4);
        if (R(p->rowTable + row * 10) == 0x39) continue;
        if (row == p->selRow) ORD(10014, py + 0x12, yy + 1, py + 0x12 + ((p->scrW - py * 2 - 0x31) / 3) * 3, yy + 0x13, 0, 1);
        ORD(10003, yy + 0x12, row == p->selRow ? (p->pressed ? 3 : 0) : 0, 0, 0, 0, 0);
        ORD(10150, 0, 0, 0, 0, 0, 0);
        if (both) ORD(10150, yy + 0x12, (uint32_t)rowState(R(p->rowTable + row * 10), 1), 0, 0, 0, 0);
        ORD(10150, yy + 0x12, (uint32_t)rowState(R(p->rowTable + row * 10), 0), 0, 0, 0, 0);
    }
    p->frame++;
    ORD(10184, 0, 0, 0, 0, 0, 0);
}
