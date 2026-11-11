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
    uint32_t (*callStk)(uint32_t rva, uint32_t eax, uint32_t esi, const uint32_t* stk, uint32_t n); // stack args, first = top
    uint32_t (*callR)(uint32_t rva, uint32_t ecx, const uint32_t* edi, uint32_t stack0); // register ecx/edi + one stack arg
    uint32_t (*fatal)(uint32_t strId);              // Ordinal_10265 -> Ordinal_10024 -> _exit(-1)
    uint32_t dataTables;                            // *sgptDataTables_exref
    uint32_t g_cbbfc, g_cbc14, g_a16b8, g_bba608;   // D2Client globals 0x1CBBFC/0x1CBC14/0x1A16B8/0x1BA608 (rva bases)
    uint32_t tab_ab8, tab_9b8;                      // 0xD5AB8 / 0xD59B8 dword tables (64 entries, spawn x/y offsets)
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

// 1.13c D2CLIENT+0x57BB0  fastcall(ecx=unit, edx=idx) + stack p3 (ret 4). sub = (short)rec[0xEA].
extern "C" uint32_t d2c_57BB0_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; if (!n99_rec(idx, &sub, true)) return 0;
    n99_mark(unit);
    if (!CALL(0xA5F70)) return 0;
    uint32_t r = ORD(11169, sub, p3);   // 6FABCB5C(sub, p3)
    uint32_t b[23] = {0};
    b[0] = 7; b[1] = unit; b[4] = (uint32_t)sub;
    b[5] = CALL(0x1630, unit); b[6] = CALL(0x1660, unit);
    b[10] = r; b[11] = (uint32_t)idx; b[12] = p3;
    for (uint32_t i = 0; i < 0x100; i += 4) {
        b[7] = R(g99.tab_ab8 + i); b[8] = R(g99.tab_9b8 + i);
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

// 1.13c D2CLIENT+0x57D50  fastcall(ecx=unit, edx=idx) + stack p3. 51340 writes *edi=X, *stack=Y (target).
// stepX = -2*sign(dy), stepY = 2*sign(dx) (the retail branch ladder), n = 10786(unit, rec[0x138], idx, p3).
extern "C" uint32_t d2c_57D50_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    n99_mark(unit);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, unit, (uint32_t)(uintptr_t)out)) return 0;
    int32_t ux = (int32_t)CALL(0x1630, unit), uy = (int32_t)CALL(0x1660, unit);
    int32_t dx = out[0] - ux, dy = out[1] - uy;
    if (dx == 0 && dy == 0) return 0;
    int32_t stepX = dy > 0 ? -2 : (dy < 0 ? 2 : 0);
    int32_t step = dx > 0 ? 2 : (dx < 0 ? -2 : 0);
    int32_t n = (int32_t)ORD(10786, unit, R(rec + 0x138), idx, p3);
    dy = dy - (n / 2) * step;
    dx = dx - (n / 2) * stepX;
    uint32_t b[23] = {0};
    b[0] = 3;
    for (; n > 0; n--) {
        b[1] = unit; b[4] = (uint32_t)sub;
        b[5] = (uint32_t)(dx / 6 + ux);
        b[6] = (uint32_t)(dy / 6 + uy);
        b[7] = (uint32_t)dx; b[8] = (uint32_t)dy; b[12] = p3;
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
    b[0] = 3; b[1] = unit; b[2] = unit; b[4] = (uint32_t)sub;
    b[5] = CALL(0x1630, unit); b[6] = CALL(0x1660, unit);
    b[11] = (uint32_t)idx; b[12] = p3;
    b[13] = ORD(10786, unit, R(rec + 0x138), (uint32_t)idx, p3);
    uint32_t t = CALL(0xA5F70);
    uint32_t path = R(unit + 0x2c);
    int32_t y;
    if (t) {
        ORD(10739, path, CALL(0x1630, t), CALL(0x1660, t), 0);   // 6FABC478(path, X(t), Y(t), 0), result unused
        b[7] = CALL(0x1630, t) - CALL(0x1630, unit);
        y = (int32_t)CALL(0x1660, t);
    } else {
        int32_t r = (int32_t)ORD(10983, path);                    // 6FABC31C(path)
        b[7] = (uint32_t)(r - (int32_t)CALL(0x1630, unit));
        y = (int32_t)ORD(10764, path);                            // 6FABC340(path)
    }
    b[8] = (uint32_t)(y - (int32_t)CALL(0x1660, unit));
    uint32_t m = g99.callb(0xB30F0, b);
    if (m) ORD(10637, m, p3);                                     // 6FABC448(m, p3)
    return 1;
}

// unit X/Y as the retail inline ladder: type 2/4/5 read the path (unit+0x2c) +0xC/+0x10, else ordinal 10867/10750 (0 if no path).
static uint32_t n99_ux(uint32_t unit, bool y) {
    uint32_t t = R(unit), path = R(unit + 0x2c);
    if (t == 2 || (t >= 4 && t <= 5)) return R(path + (y ? 0x10 : 0xc));
    return path ? ORD(y ? 10750 : 10867, path) : 0;
}

// 1.13c D2CLIENT+0x58370  EAX = idx (entry index), ESI = unit, stack A0..A4 (ret 0x14).
// A0 = dword table of 5, A1/A2 go to b[7]/b[8], A3 to b[11], A4 to b[12]; 56E10 gets eax=A3, stack (unit, A4).
extern "C" uint32_t d2c_58370_n99(uint32_t unit, int32_t idx, uint32_t A0, uint32_t A1, uint32_t A2, uint32_t A3, uint32_t A4) {
    uint32_t b[23] = {0};
    uint64_t s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
    W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
    b[0] = 0x25; b[1] = unit;
    if (((uint32_t)s & 3) > 0) b[0] = 0x4025;
    b[4] = (uint32_t)idx;
    b[5] = n99_ux(unit, false); b[6] = n99_ux(unit, true);
    b[7] = A1; b[8] = A2; b[11] = A3; b[12] = A4;
    uint32_t dt = g99.dataTables;
    if (idx < 0 || idx >= (int32_t)R(dt + 0xb6c)) return 0;
    uint32_t ent = (uint32_t)idx * 0x1a4 + R(dt + 0xb64);
    if (!ent) return 0;
    s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
    W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
    b[10] = (R(ent + 0x9a) & 0xff) + R(A0 + ((uint32_t)s % 5) * 4);
    uint32_t m = g99.callb(0xB30F0, b);
    if (!m) return 0;
    CALL(0x56E10, A3, unit, A4);
    return m;
}

// 1.13c D2CLIENT+0x584D0  fastcall(ecx=unit, edx=idx) + stack p3 (ret 4).
// 51340 (ebx=unit, edi=&X, stack=&Y). B6D40: EAX=sub, ESI=unit, stack (X, Y, r1, idx, p3, r2, 1).
extern "C" uint32_t d2c_584D0_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    n99_mark(unit);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, unit, (uint32_t)(uintptr_t)out)) return 0;
    int32_t h = (int32_t)ORD(10786, unit, R(rec + 100), (uint32_t)idx, p3) - 6;
    if (h < 1) h = 1;
    uint32_t v = ORD(10786, unit, R(rec + 0x114), (uint32_t)idx, p3);
    uint32_t stk[7] = {(uint32_t)out[0], (uint32_t)out[1], (uint32_t)h, (uint32_t)idx, p3, v, 1};
    g99.callStk(0xB6D40, (uint32_t)sub, unit, stk, 7);
    return 1;
}

// 1.13c D2CLIENT+0x585C0  fastcall(ecx=unit, edx=idx) + stack p3 (ret 4).
// The 0xD5C90 / 0xD5C8C tables are BYTE tables (movzx / mov cl), not dwords.
extern "C" uint32_t d2c_585C0_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; if (!n99_rec(idx, &sub, true)) return 0;
    n99_mark(unit);
    uint32_t t = CALL(0xA5F70);
    if (!t) return 0;
    int32_t tx = (int32_t)CALL(0x1630, t), ty = (int32_t)CALL(0x1660, t);
    int32_t ux = (int32_t)CALL(0x1630, unit), uy = (int32_t)CALL(0x1660, unit);
    int32_t ax = tx - ux; if (ax < 0) ax = -ax;
    int32_t ay = ty - uy; if (ay < 0) ay = -ay;
    int32_t px, py;
    if (ax < ay) {              // x steps four times toward the target; y is the target's
        px = ux; py = ty;
        for (int i = 0; i < 4; i++) { if (px > tx) px--; else if (px < tx) px++; }
    } else {                    // y steps four times toward the target; x is the target's
        px = tx; py = uy;
        for (int i = 0; i < 4; i++) { if (py > ty) py--; else if (py < ty) py++; }
    }
    int32_t pdx = px - ux, pdy = py - uy;
    uint32_t b[23] = {0};
    b[0] = 3; b[1] = unit; b[4] = (uint32_t)sub;
    b[5] = (uint32_t)(pdx / 6 + ux - 1);
    b[6] = (uint32_t)(pdy / 6 + uy - 1);
    b[7] = (uint32_t)pdx; b[8] = (uint32_t)pdy;
    b[11] = (uint32_t)idx; b[12] = p3;
    uint32_t dir = ORD(10192, unit, (uint32_t)px, (uint32_t)py);
    uint32_t k = R(g99.tab_c90 + (dir & 0x3f)) & 0xff;
    uint32_t v = R(g99.tab_c8c + (k & 3)) & 0xff;
    ORD(10706, R(unit + 0x2c), v);
    g99.callb(0xB30F0, b);
    return 1;
}

// 1.13c D2CLIENT+0x58A70  fastcall(ecx=unit, edx=idx) + stack p3 (ret 4). 52380: ecx=kind, edi=&b, stack=arg.
extern "C" uint32_t d2c_58A70_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    n99_mark(unit);
    uint32_t b[23] = {0};
    b[0] = 0x2021; b[1] = unit; b[2] = unit;
    b[5] = CALL(0x1630, unit); b[6] = CALL(0x1660, unit);
    b[12] = p3; b[11] = (uint32_t)idx; b[4] = (uint32_t)sub;
    g99.callR(0xA2380, 1, b, 2);
    int32_t s2 = (int16_t)(R(rec + 0xec) & 0xffff);
    if (s2 >= 0 && s2 < (int32_t)R(g99.dataTables + 0xb6c)) { b[4] = (uint32_t)s2; g99.callR(0xA2380, 5, b, 4); }
    return 1;
}

// 1.13c D2CLIENT+0x58B80
// fastcall(ecx=unit, edx=idx) + stack count (ret 4). The Ordinal_10786 result is the limit the thunk-10980 counter is compared against.
// Sub-calls: the unit-info lookup fills X and Y, the stack call takes the table offset then X, Y, idx and count, and the record is released last.
extern "C" uint32_t d2c_58B80_n99(uint32_t unit, int32_t idx, int32_t count) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    uint32_t h = ORD(10511, unit);
    if (!h) return 0;
    int32_t lim = (int32_t)ORD(10786, unit, R(rec + 0x114), (uint32_t)idx, (uint32_t)count);
    int32_t k = (int32_t)ORD(10980, h);
    if (k + 1 < lim) { ORD(10243, h, (uint32_t)(k + 1)); return 0; }
    ORD(10243, h, 0);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, unit, (uint32_t)(uintptr_t)out)) return 0;
    ORD(10739, R(unit + 0x2c), (uint32_t)out[0], (uint32_t)out[1], 0);
    uint32_t stk[5] = { 0xD5BE0, (uint32_t)out[0], (uint32_t)out[1], (uint32_t)idx, (uint32_t)count };
    g99.callStk(0x58370, (uint32_t)sub, unit, stk, 5);
    CALL(0x56DA0, unit, rec);
    return 1;
}

// 1.13c D2CLIENT+0x58C90
// fastcall(ecx=unit, edx=idx) + stack p3 (ret 4). T = short rec[0xEC] (not range-checked); twice: if T > 0 the unit RNG steps and an odd low bit
// picks T as the 58370 entry index, else the sub index (short rec[0xEA]).
extern "C" uint32_t d2c_58C90_n99(uint32_t unit, int32_t idx, uint32_t p3) {
    int32_t sub; uint32_t rec = n99_rec(idx, &sub, true); if (!rec) return 0;
    int32_t T = (int16_t)(R(rec + 0xec) & 0xffff);
    int32_t out[2] = {0, 0};
    if (!CALL(0xA1340, unit, (uint32_t)(uintptr_t)out)) return 0;
    ORD(10739, R(unit + 0x2c), (uint32_t)out[0], (uint32_t)out[1], 0);
    for (int i = 2; i; i--) {
        int32_t e = sub;
        if (T > 0) {
            uint64_t s = (uint64_t)R(unit + 0x20) * 0x6ac690c5ull + R(unit + 0x24);
            W(unit + 0x20, (uint32_t)s); W(unit + 0x24, (uint32_t)(s >> 32));
            if ((uint32_t)s & 1) e = T;
        }
        uint32_t stk[5] = { 0xD5C78, (uint32_t)out[0], (uint32_t)out[1], (uint32_t)idx, p3 };
        g99.callStk(0x58370, (uint32_t)e, unit, stk, 5);
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
