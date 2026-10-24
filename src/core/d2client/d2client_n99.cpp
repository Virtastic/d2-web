// D2Client batch n99: STALLED rows rebuilt from the cached decompile. Game addresses are uint32_t
// handles read through seams; D2Client+rva callees and Ordinal_* imports are function-pointer seams.
// Register-passed inputs the decompiler shows as unaff_*/in_* are explicit parameters.
// The 0x17-dword zeroed block handed to the spawn callee (0xB30F0) is b[23].
#include <cstdint>
#include <cstddef>

struct N99Env {
    uint32_t (*rd32)(uint32_t a);
    void (*wr32)(uint32_t a, uint32_t v);
    uint32_t (*ord)(uint32_t ordinal, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    uint32_t (*call)(uint32_t rva, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    uint32_t (*callb)(uint32_t rva, uint32_t* blk); // 0xB30F0 spawn: block in a register
    uint32_t (*fatal)(uint32_t strId);              // Ordinal_10265 -> Ordinal_10024 -> _exit(-1)
    uint32_t dataTables;                            // *sgptDataTables_exref
    uint32_t g_cbbfc, g_cbc14, g_a16b8, g_bba608;   // D2Client globals 0x1CBBFC/0x1CBC14/0x1A16B8/0x1BA608 (rva bases)
    uint32_t tab_c8c, tab_c90;                      // dir tables 0x185C8C / 0x185C90
};
static N99Env g99;
extern "C" void d2c_n99_bind(const N99Env* e) { g99 = *e; }
#define R(a) g99.rd32((uint32_t)(a))
#define W(a, v) g99.wr32((uint32_t)(a), (uint32_t)(v))
#define CALL(rva, ...) n99_call(rva, ##__VA_ARGS__)
static uint32_t n99_call(uint32_t rva, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0, uint32_t g = 0) {
    return g99.call(rva, a, b, c, d, e, f, g);
}
static uint32_t ORD(uint32_t n, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) { return g99.ord(n, a, b, c, d); }

// missile-table record (0x23c stride at dt+0xb98) and its short at +0xea; 0 if out of range.
static uint32_t n99_rec(uint32_t idx, int32_t* sub, bool needSub) {
    uint32_t dt = g99.dataTables;
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)R(dt + 0xba0)) return 0;
    uint32_t rec = idx * 0x23c + R(dt + 0xb98);
    if (!rec) return 0;
    if (needSub) {
        int32_t s = (int16_t)(R(rec + 0xea) & 0xffff);
        if (s < 0 || s >= (int32_t)R(dt + 0xb6c)) return 0;
        *sub = s;
    }
    return rec;
}
static void n99_mark(uint32_t unit) { if (unit) W(unit + 0xc4, R(unit + 0xc4) | 0x40); }

// 1.13c D2CLIENT+0x57BB0
extern "C" uint32_t d2c_57BB0_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; if (!n99_rec(idx, &sub, true)) return 0;
    n99_mark(unit);
    if (!CALL(0xA5F70)) return 0;
    ORD(11169, sub, p3);
    uint32_t b[23] = {0};
    b[5] = CALL(0x1630, unit); b[6] = CALL(0x1660, unit);
    for (int i = 0; i < 0x100; i += 4) {
        uint32_t m = g99.callb(0xB30F0, b);
        if (!m) continue;
        uint32_t s = R(m + 0x54);
        if (s && R(s + 0x30)) { uint32_t p = R(s + 0x30); W(p, (R(p) & 0xfffffffe) | 8); }
        s = R(m + 0x54); if (s && R(s + 0x30)) { uint32_t p = R(s + 0x30); W(p, R(p) & 0xffffffdf); }
        s = R(m + 0x54); if (s && R(s + 0x30)) { uint32_t p = R(s + 0x30); W(p + 0x10, 0); W(p + 0x14, 0); W(p + 0x18, 0xffffd000); }
        s = R(m + 0x54); if (s && R(s + 0x30)) { uint32_t p = R(s + 0x30); W(p + 0x28, 0); W(p + 0x2c, 0); W(p + 0x30, 0); }
        s = R(m + 0x54); if (s && R(s + 0x30)) { uint32_t p = R(s + 0x30); W(p + 4, 0); W(p + 8, 0); W(p + 0xc, 0x3c000); }
    }
    return 1;
}

// 1.13c D2CLIENT+0x57D50
// stepX = unaff_EDI (dx step the caller leaves in EDI); tx = local_7c (target x filled by 0xA1340 pair)
extern "C" uint32_t d2c_57D50_n99(uint32_t unit, int32_t idx, uint32_t p3, int32_t stepX) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    n99_mark(unit);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, (uint32_t)(uintptr_t)out)) return 0;
    int32_t ux = (int32_t)CALL(0x1630, unit), uy = (int32_t)CALL(0x1660, unit);
    int32_t dx = out[0] - ux, dy = out[1] - uy, step = 0;
    if (dx == 0 && dy == 0) return 0;
    if (dx >= 0 || dy >= 0) {
        if (dx == 0) goto have;
        if (dx > 0) { step = 2; goto have; }
        if (dx >= 0) goto have;
        if (dy > 0) { step = -2; goto have; }
        if (dy != 0) goto have;
    }
    step = -2;
have:;
    int32_t n = (int32_t)ORD(10786, unit, R(rec + 0x138), idx, p3);
    dy = dy - (n / 2) * step;
    dx = dx - (n / 2) * stepX;
    uint32_t b[23] = {0};
    b[0] = 3;
    for (; n > 0; n--) {
        b[5] = (uint32_t)(dx / 6 + (int32_t)unit);
        b[6] = (uint32_t)(dy / 6 + out[1]);
        b[12] = 0; b[4] = idx; b[7] = dx; b[8] = dy;
        g99.callb(0xB30F0, b);
        dx += stepX; dy += step;
    }
    return 1;
}

// 1.13c D2CLIENT+0x58070
extern "C" uint32_t d2c_58070_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    uint32_t dt = g99.dataTables;
    if (!(sub * 0x1a4 + R(dt + 0xb64))) return 0;
    n99_mark(unit);
    uint32_t b[23] = {0};
    b[0] = 3; b[1] = unit; b[2] = unit; b[4] = sub;
    b[5] = CALL(0x1630, unit); b[6] = CALL(0x1660, unit);
    b[11] = idx; b[12] = p3; b[13] = ORD(10786, unit);
    uint32_t t = CALL(0xA5F70);
    int32_t r;
    if (!t) {
        r = (int32_t)ORD(10983, R(unit + 0x2c));
        b[2] = (uint32_t)(r - (int32_t)CALL(0x1630, unit));
        r = (int32_t)ORD(10764, R(unit + 0x2c));
    } else {
        ORD(10739, R(unit + 0x2c), CALL(0x1630, t), CALL(0x1660, t), 0);
        CALL(0x1630, t); CALL(0x1630, unit);
        r = (int32_t)CALL(0x1660, t);
    }
    b[0] = (uint32_t)(r - (int32_t)CALL(0x1660, unit));
    uint32_t m = g99.callb(0xB30F0, b);
    if (m) ORD(10637, m, b[16]);
    return 1;
}

// 1.13c D2CLIENT+0x58370
// esi = unit, eax = missile index, stack = second arg of 0x56E10
extern "C" uint32_t d2c_58370_n99(uint32_t unit, int32_t idx, uint32_t stackArg) {
    uint32_t b[23] = {0};
    uint64_t s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
    b[0] = 0x25;
    W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
    if (s & 3) b[0] = 0x4025;
    uint32_t t = R(unit), pos = R(unit + 0x2c);
    if (t != 2 && (t < 4 || t > 5) && pos) b[6] = ORD(10867, pos);
    if (t != 2 && (t < 4 || t > 5) && pos) b[5] = ORD(10750, pos);
    uint32_t dt = g99.dataTables;
    if (idx >= 0 && idx < (int32_t)R(dt + 0xb6c) && (idx * 0x1a4 + R(dt + 0xb64))) {
        s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
        W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
        uint32_t m = g99.callb(0xB30F0, b);
        if (m) { CALL(0x56E10, unit, stackArg); return m; }
    }
    return 0;
}

// 1.13c D2CLIENT+0x584D0
extern "C" uint32_t d2c_584D0_n99(uint32_t unit, int32_t idx, uint32_t p3, uint32_t edi) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    n99_mark(unit);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, (uint32_t)(uintptr_t)out)) return 0;
    uint32_t a = R(rec + 100);
    int32_t h = (int32_t)ORD(10786, unit, a, idx, p3) - 6;
    if (h < 1) h = 1;
    uint32_t v = ORD(10786, unit, R(rec + 0x114), idx, (uint32_t)out[0]);
    CALL(0xB6D40, unit, a, unit, (uint32_t)h, (uint32_t)idx, edi, v);
    return 1;
}

// 1.13c D2CLIENT+0x585C0
extern "C" uint32_t d2c_585C0_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; if (!n99_rec(idx, &sub, true)) return 0;
    n99_mark(unit);
    uint32_t t = CALL(0xA5F70);
    if (!t) return 0;
    int32_t tx = (int32_t)CALL(0x1630, t), ty = (int32_t)CALL(0x1660, t);
    int32_t ux = (int32_t)CALL(0x1630, unit), uy = (int32_t)CALL(0x1660, unit);
    int32_t ax = tx - ux; if (ax < 0) ax = -ax;
    int32_t ay = ty - uy; if (ay < 0) ay = -ay;
    int32_t px = ux, py = uy;
    if (ax < ay) {
        for (int i = 0; i < 4; i++) { if (tx < px) px--; else if (px < tx) px++; }
    } else {
        for (int i = 0; i < 4; i++) { if (ty < py) py--; else if (py < ty) py++; }
        px = tx;
    }
    uint32_t b[23] = {0};
    b[10] = (uint32_t)(px - ux); b[9] = (uint32_t)(py - uy);
    b[6] = (uint32_t)((px - ux) / 6 - 1 + ux);
    b[5] = (uint32_t)((py - uy) / 6 - 1 + uy);
    b[0] = 3; b[11] = idx; b[12] = p3; b[4] = sub; b[1] = unit;
    uint32_t dir = ORD(10192, unit, (uint32_t)px, (uint32_t)py);
    uint32_t tbl = R(g99.tab_c8c + (R(g99.tab_c90 + (dir & 0x3f)) & 3) * 4) ;
    ORD(10706, R(unit + 0x2c), tbl);
    g99.callb(0xB30F0, b);
    return 1;
}

// 1.13c D2CLIENT+0x58A70
extern "C" uint32_t d2c_58A70_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    n99_mark(unit);
    uint32_t b[23] = {0};
    b[0] = 0x2021; b[1] = unit; b[2] = unit;
    b[5] = CALL(0x1630, unit); b[6] = CALL(0x1660, unit);
    b[12] = p3; b[4] = sub; b[11] = idx;
    CALL(0xA2380, 1, 2);
    int32_t s2 = (int16_t)(R(rec + 0xec) & 0xffff);
    if (s2 >= 0 && s2 < (int32_t)R(g99.dataTables + 0xb6c)) CALL(0xA2380, 5, 4);
    return 1;
}

// 1.13c D2CLIENT+0x58B80
// ecx = unit (param_1), edx = idx, stack0 = retaddr slot read as unaff_retaddr (the counter arg)
extern "C" uint32_t d2c_58B80_n99(uint32_t unit, int32_t idx, int32_t count) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    uint32_t h = ORD(10511);
    if (!h) return 0;
    uint32_t v = R(rec + 0x114);
    ORD(10786, unit, v);
    int32_t k = (int32_t)ORD(10980, h);
    if (k + 1 < count) { ORD(10243, h, (uint32_t)(k + 1)); return 0; }
    ORD(10243, h, 0);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, (uint32_t)(uintptr_t)out)) return 0;
    ORD(10739, R(unit + 0x2c), v, (uint32_t)out[0], 0);
    CALL(0x58370, unit, idx);
    CALL(0x56DA0, rec);
    return 1;
}

// 1.13c D2CLIENT+0x58C90
// ebx = count (unaff_EBX)
extern "C" uint32_t d2c_58C90_n99(uint32_t unit, int32_t idx, int32_t ebx) {
    int32_t sub; if (!n99_rec(idx, &sub, true)) return 0;
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, (uint32_t)(uintptr_t)out)) return 0;
    ORD(10739, R(unit + 0x2c), (uint32_t)out[1], (uint32_t)out[0], 0);
    for (int i = 2; i; i--) {
        if (ebx > 0) {
            uint64_t s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
            W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
        }
        CALL(0x58370, unit, idx);
    }
    return 1;
}

// 1.13c D2CLIENT+0x62410
// units = ESI array of unit handles, n = EDI count; the 0x619E0 call takes the unit in ECX.
extern "C" void d2c_62410_n99(uint32_t units, int32_t n) {
    uint32_t prev = R(g99.g_cbc14);
    uint32_t o1 = 0, o2 = 0;
    ORD(10383, ORD(10331, R(g99.g_cbbfc), (uint32_t)(uintptr_t)&o1, (uint32_t)(uintptr_t)&o2));
    for (int i = 0; i < n; i++) {
        uint32_t u = R(units + i * 4);
        for (uint32_t c = R(u + 0x74); c; c = R(c + 0xe8)) CALL(0x619E0, u);
    }
    uint32_t cur = R(g99.g_cbc14);
    if (!prev || !cur) return;
    int16_t s1 = (int16_t)(R(prev + 0x22) & 0xffff);
    int cnt = 0;
    for (uint32_t p = cur; p; p = R(p + 0x80)) if ((int16_t)(R(p + 0x22) & 0xffff) == s1) cnt++;
    if (cnt < 2 || s1 == -1 || !R(g99.g_a16b8)) return;
    for (uint32_t q = R(prev + 0x80); q; q = R(q + 0x80)) {
        uint32_t key = R(q + 0x10);
        bool hit = false;
        for (uint32_t p = R(g99.g_bba608 + (key & 0x7f) * 4); p; p = R(p + 0xe4)) {
            if (R(p + 0xc) == key) {
                if (R(p)) { g99.fatal(0x5a); return; }
                hit = true; break;
            }
        }
        if (hit) continue;
        int c2 = 0; int16_t s4 = -1;
        if (cur) {
            s4 = (int16_t)(R(q + 0x22) & 0xffff);
            for (uint32_t p = cur; p; p = R(p + 0x80)) if ((int16_t)(R(p + 0x22) & 0xffff) == s4) c2++;
            if (c2 < 2) s4 = -1;
        }
        if (s1 == s4) { CALL(0x60AF0); cur = R(g99.g_cbc14); }
    }
}
