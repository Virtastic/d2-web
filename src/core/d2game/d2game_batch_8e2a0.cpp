// src/core/d2game/d2game_batch_8e2a0.cpp -- 1.13c D2Game.dll +0x8E2A0..+0x92DF0, transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Batch lane: implementation + marker only.
// Callees (D2Common ordinals, D2Game/D2Common-local thunks) are seams in retail order, reached through
// D2GameB8E2Env. Hidden register inputs (unaff_*/in_EAX/extraout_*) are explicit parameters, named for the
// register. Data-table access takes the sgptDataTables base `dt` (+0xB98/+0xBA0 = skills rows/count, stride 0x23C;
// +0xBD4 = states count; +0xBC0 = missiles count).
#include <cstdint>
#include <emscripten.h>

struct D2GameB8E2Env {
    // D2Common ordinals
    int      (*o10680)(void* unit, int stat, int layer);
    int      (*o10630)(int v);
    int      (*o10306)(void* unit, int v);
    int      (*o10786)(void* unit, int a, int b, void* c);
    int      (*o10392)(uint32_t id);
    int      (*o10057)(int v);
    int      (*o10331)(void* unit);
    int      (*o10511)(void* unit);
    void     (*o10243)(void* unit, int v);
    int      (*o11138)(void* a, void* b);
    int      (*o10653)(void);
    int      (*o10973)(void* unit, int stat, int layer);
    int      (*o11027)(void* unit);
    int      (*o10826)(void);
    int      (*o10522)(int v);
    int      (*o10265)(void);
    void     (*o10024)(int v);
    int      (*o11104)(void);
    int      (*o10830)(void);
    // D2Game-local thunks / functions
    int      (*f6AD10)(int v);          // 0x6AD10
    int      (*f8E110)(void* a, int b); // 0x8E110
    void     (*f8A3E0)(void* buf, int n); // 0x8A3E0
    uint32_t (*f21100)(void* unit);     // 0x21100
    uint32_t (*f21130)(void* unit);     // 0x21130
    int      (*f21B10)(int v, int w);   // 0x21B10
    int      (*f21BC0)(void);           // 0x21BC0
    uint32_t (*f21180)(void* seed);     // 0x21180
    int      (*f9F120)(void);           // 0x9F120
    void     (*f9F210)(void);           // 0x9F210
    int      (*f9FA30)(void* p, int* out); // 0x9FA30
    int      (*f91340)(void* p);        // 0x91340
    void     (*fA0000)(void* p);        // 0xA0000
    void     (*fE07A0)(int v);          // 0xE07A0
    uint32_t (*fE0BD0)(int v);          // 0xE0BD0
    int      (*fDF780)(void);           // 0xDF780
    void     (*o10689)(uint32_t a, int b, int c);
    uint32_t (*fFE5A0)(void* a, int b, int c); // 0xFE5A0
    void     (*fFDDE0)(uint8_t b, void* a, void* c); // 0xFDDE0
    void     (*f92360)(void* a, void* b, void* c, void* d, int e); // 0x92360
    void     (*f8E6F0)(void);           // 0x8E6F0
    void     (*fexit)(int code);        // _exit
};

static inline const uint8_t* b8e2Row(const uint8_t* dt, int idx)
{
    const int cnt = *(const int*)(dt + 0xBA0);
    if (idx < 0 || idx >= cnt) return nullptr;
    return *(const uint8_t* const*)(dt + 0xB98) + (size_t)idx * 0x23C;
}

// 1.13c D2Game+0x8E2A0
// unit is edi. Null unit: report (#10265, #10024) and _exit(-1). Else the message buffer (4 bytes) is sent
// via 0x8A3E0(buf,10) when 0x8E110 accepts; if byte1 of the buffer is set and #10522 > 0x32, a 13-byte packet
// built from 0x21100/0x21130 is sent.
EMSCRIPTEN_KEEPALIVE void d2_game_8e2a0(void* unit, const D2GameB8E2Env* env)
{
    uint8_t buf[4];
    if (!unit) {
        env->o10265();
        env->o10024(0);
        env->fexit(-1);
        return;
    }
    const int r0 = env->o11027(unit);
    env->o10331(unit);
    env->o10826();
    void* game = (void*)(intptr_t)env->f6AD10(0);
    if (env->f8E110(game, r0) != 0) env->f8A3E0(buf, 10);
    if (buf[1] != 0) {
        const int g = env->f6AD10(0);
        if (env->o10522(g) > 0x32) {
            buf[0] = (uint8_t)(env->f21100(unit) >> 24);
            const uint32_t v = env->f21130(unit);
            buf[1] = (uint8_t)v;
            buf[2] = (uint8_t)(v >> 8);
            buf[3] = (uint8_t)(v >> 16);
            env->f8A3E0(buf, 0xD);
        }
    }
}

// 1.13c D2Game+0x8E8C0
// in_EAX = list owner, unaff_EBX = type byte, param_1 = id. Walk the chain at owner+0xDC (next at +0x24);
// call 0x8E6F0 on every node whose byte0 == type and +0x2C == id.
EMSCRIPTEN_KEEPALIVE void d2_game_8e8c0(int id, uint8_t* owner, uint32_t type, const D2GameB8E2Env* env)
{
    if (!owner) return;
    uint8_t* n = *(uint8_t**)(owner + 0xDC);
    while (n) {
        uint8_t* cur = n;
        n = *(uint8_t**)(cur + 0x24);
        if (cur[0] == type && id == *(int*)(cur + 0x2C)) env->f8E6F0();
    }
}

// 1.13c D2Game+0x91040
// Walks a random path of param_1[0xB] (u16 x,y pairs) points. Ordinals: 10534/10558/10970/10614/10647/
// 10168/10817/10867/10750/10764/10983/10192/10266/10165 (all seams, retail order); tables at 0x6FD1BA60/80
// (dx/dy per direction), 0x6FD2F144/48 (turn base / run length).
struct D2GameB91040Env {
    int      (*o10534)(void* u);
    void     (*o10558)(void* u, int v);
    void     (*o10970)(void* u, int v);
    void     (*o10614)(void);
    void     (*o10647)(int path, int v);
    void     (*o10168)(int path, int n);
    void     (*o10817)(int path);
    void     (*o10867)(int v);
    int      (*o10750)(int v);
    uint32_t (*o10764)(void);
    uint32_t (*o10983)(int v, uint32_t w);
    uint32_t (*o10192)(void* u, uint32_t w);
    uint32_t (*o10266)(uint32_t v);
    void     (*o10165)(uint32_t a, int b);
    const int* dxTab;   // 0x6FD1BA80
    const int* dyTab;   // 0x6FD1BA60
    int turnBase;       // 0x6FD2F144
    int runLen;         // 0x6FD2F148
};
EMSCRIPTEN_KEEPALIVE void d2_game_91040(int32_t* u, int32_t* pathBuf, int32_t startX,
                                        const D2GameB91040Env* e)
{
    if (!u) return;
    int n = e->o10534(u);
    if (n > 0x4D) { e->o10558(u, 0x4D); e->o10970(u, 0x4D); n = 0x4D; }
    const int path = u[0xB];
    uint64_t* seed = (uint64_t*)(u + 8);
    e->o10614();
    e->o10647(path, 10);
    e->o10168(path, n);
    e->o10817(path);
    int type = *u;
    if (type != 2 && (type < 4 || type > 5) && u[0xB] != 0) e->o10867(u[0xB]);
    type = *u;
    int y;
    if (type == 2 || (type > 3 && type < 6)) y = *(int*)((intptr_t)u[0xB] + 0x10);
    else y = u[0xB] == 0 ? 0 : e->o10750(u[0xB]);
    const int cnt = u[0xB];
    uint32_t h = e->o10983(u[0xB], e->o10764());
    uint32_t dir = e->o10192(u, h) & 0xFF;
    uint64_t s = (uint64_t)(uint32_t)*seed * 0x6AC690C5ull + (uint32_t)u[9];
    *(uint32_t*)seed = (uint32_t)s;
    int sign = (int)((s & 1) != 0) * 2 - 1;
    s = (s & 0xFFFFFFFFull) * 0x6AC690C5ull + (s >> 32);
    *seed = s;
    int step = (int)((s & 0xFFFFFFFFull) % 3) + e->turnBase;
    int x = startX;
    for (int i = 0; i < cnt; ++i) {
        dir += step * sign;
        const uint32_t d = e->o10266(dir & 0xFFFFFF3F) & 0xFF;
        x += e->dxTab[d];
        y += e->dyTab[d];
        ((int16_t*)(intptr_t)path)[i * 2] = (int16_t)x;
        ((int16_t*)(intptr_t)path)[i * 2 + 1] = (int16_t)y;
        if (i % e->runLen == 0) {
            sign = -sign;
            s = (uint64_t)(uint32_t)*seed * 0x6AC690C5ull + (uint32_t)u[9];
            *seed = s;
            step = (int)((s & 0xFFFFFFFFull) % 3) + e->turnBase;
        }
    }
    e->o10165(10, cnt);
}

// 1.13c D2Game+0x91400
// param_1 = unit (ecx), param_2 = skill-holder (edx), unaff_ESI = target unit, record = unaff register (iVar6=0x15F
// kept as in the decompile: the adds target the caller-frame accumulator at +0x38/+0x3C).
EMSCRIPTEN_KEEPALIVE void d2_game_91400(void* p1, void* p2, void* esi, uint8_t* acc, const uint8_t* dt,
                                        const D2GameB8E2Env* e)
{
    const uint8_t* row = b8e2Row(dt, e->o10680(p2, 0x15E, 0));
    if (!row) return;
    int v = e->o10680(p2, 0x15F, 0);
    const int w = e->o10306(p1, e->o10630(1));
    if (v < w) v = w;
    if (v <= 0) return;
    const int16_t st = *(const int16_t*)(row + 0x54);
    if (st < 0 || st >= *(const int*)(dt + 0xBD4)) return;
    const int lvl = e->o10680(esi, st, 0);
    if (lvl <= 0) return;
    v = e->f21B10(0, v);
    if (lvl != 1) {
        if (lvl != 2) {
            if (lvl != 3) return;
            v *= 2;
        }
        *(int*)(acc + 0x38) += v;
        *(int*)(acc + 0x3C) += v;
        return;
    }
    *(int*)(acc + 0x38) += v;
}

// 1.13c D2Game+0x91610
// unaff_ESI = unit, unaff_retaddr = threshold. True when unit type==1, #11104 and #10830 are 0, 0x9F120
// is true and 0x21180(seed at +0x20) <= threshold.
EMSCRIPTEN_KEEPALIVE bool d2_game_91610(int32_t* unit, int threshold, const D2GameB8E2Env* e)
{
    if (unit && *unit == 1 && e->o11104() == 0 && e->o10830() == 0 && (e->f9F120() & 0xFF) != 0)
        return (int)e->f21180(unit + 8) <= threshold;
    return false;
}

// 1.13c D2Game+0x91660
// in_EAX = skill-holder record, param_2 = unit, param_3 = skills.txt row, param_4 = level. Raises unit
// stats 7/6 by a percentage of the unit's base (#10913), then applies states from the record.
// Seams are carried as raw function pointers; the 0x23260/0xF44B0 calls keep their decompile arguments.
struct D2GameB91660Env {
    uint32_t (*o10913)(void* u);
    void     (*o10887)(void* u, int stat, uint32_t v, int layer);
    int      (*o11013)(int a, int b, int c, int d, int e);
    void     (*o10807)(void* u, int h, int v);
    int      (*o10786)(void* u, int a, int b, void* c);
    void     (*o10188)(int h, int state, int v, int z);
    int64_t  (*alldiv)(int64_t a, int64_t b);
    void     (*f43260)(void);
    const uint32_t* statesCountPtr; // sgptDataTables+0xBD4 value pointer
    void (*const* cb)(int);         // 0x6FD2E550 callback table
};
EMSCRIPTEN_KEEPALIVE void d2_game_91660(const uint8_t* rec, int32_t* unit, int skillRow, int level,
                                        const uint8_t* dt, int ret1C, const D2GameB91660Env* e)
{
    if (level < 2) return;
    const uint8_t* srow = b8e2Row(dt, skillRow);
    const int per = srow ? *(const int*)(srow + 0x148) : 0;
    const uint32_t uVar7 = (uint32_t)((level - 1) * per);
    const uint32_t base = e->o10913(unit);
    int add;
    if ((int)base < 0x100001) {
        if ((int)uVar7 < 0x10001) add = (int)(base * uVar7) / 100;
        else if ((int)(uVar7 & 0xFFFFFFF0) < 0x640) add = (int)e->alldiv((int64_t)(int)base * (int)uVar7, 100);
        else add = ((int)uVar7 / 100) * base;
    } else if ((int)(base & 0xFFFFFFF0) < 0x640) add = (int)e->alldiv((int64_t)(int)base * (int)uVar7, 100);
    else add = ((int)base / 100) * uVar7;
    e->o10887(unit, 7, base + add, 0);
    e->o10887(unit, 6, base + add, 0);
    const int uid = unit ? unit[3] : -1;
    const int utype = unit ? *unit : 6;
    const int h = e->o11013(ret1C, 0, 0, utype, uid);
    if (!h) return;
    e->o10807(unit, h, 1);
    const int16_t* ps = (const int16_t*)(rec + 0x54);
    const int nst = *(const int*)(dt + 0xBD4);
    for (int i = 0; i < 6; ++i, ++ps)
        if (*ps > -1 && *ps < nst) e->o10188(h, *ps, e->o10786(unit, *(const int*)(rec + 0x6C), uid, unit), 0);
    ps = (const int16_t*)(rec + 0x98);
    for (int i = 0; i < 5; ++i, ++ps)
        if (*ps > -1 && *ps < nst) e->o10188(h, *ps, e->o10786(unit, *(const int*)(rec + 0xA8), uid, unit), 0);
    const int16_t s = *(const int16_t*)(rec + 0xE4);
    if (s > 0 && s < 0x2B && unit && *unit == 1 && unit[5] != 0) {
        uint8_t* name = (uint8_t*)(intptr_t)unit[5] + 0x1C;
        e->f43260();
        int k = 0;
        while (name[k] != 0) { if (++k > 8) return; }
        if (k < 9) {
            name[k] = (uint8_t)s;
            if (e->cb[s]) e->cb[s](1);
        }
    }
}

// 1.13c D2Game+0x91A30
// Same prologue as +0x91400; on a hit sets flag 0x40 at acc+5 and (for skill-class byte +0x1DC == 4, level
// 2..3, nonzero 0x21BC0) adds acc[0x30]/divisor to acc[0x34].
EMSCRIPTEN_KEEPALIVE void d2_game_91a30(void* p1, void* p2, void* ebp, uint8_t* acc, const uint8_t* dt,
                                        const D2GameB8E2Env* e)
{
    const uint8_t* row = b8e2Row(dt, e->o10680(p2, 0x15E, 0));
    if (!row) return;
    int v = e->o10680(p2, 0x15F, 0);
    const int w = e->o10306(p2, e->o10630(1));
    if (v < w) v = w;
    if (v <= 0) return;
    const int16_t st = *(const int16_t*)(row + 0x54);
    if (st < 0 || st >= *(const int*)(dt + 0xBD4)) return;
    const int lvl = e->o10680(ebp, st, 0);
    if (lvl <= 0) return;
    e->f9F210();
    if (row[0x1DC] == 4 && lvl > 1 && lvl < 4) {
        const int d = e->f21BC0();
        if (d != 0) {
            acc[5] |= 0x40;
            *(int*)(acc + 0x34) += *(int*)(acc + 0x30) / d;
            return;
        }
    }
    acc[5] |= 0x40;
}

// 1.13c D2Game+0x91F10
// Prologue as +0x91400 with unit=param_1; on a hit adds #10786 * state to the (decompile: absolute 0x0C) field
// iRam0000000c -> `field`, then via 0xDF780/0xE0BD0 looks up a record and, if +0x108 is a valid missile
// index, calls #10689.
EMSCRIPTEN_KEEPALIVE void d2_game_91f10(void* unit, int* field, const uint8_t* dt, const D2GameB8E2Env* e)
{
    const uint8_t* row = b8e2Row(dt, e->o10680(unit, 0x15E, 0));
    if (!row) return;
    int v = e->o10680(unit, 0x15F, 0);
    e->o10630(1);
    const int w = e->o10306(nullptr, 0);
    if (v < w) v = w;
    if (v <= 0) return;
    const int16_t st = *(const int16_t*)(row + 0x54);
    if (st < 0 || st >= *(const int*)(dt + 0xBD4)) return;
    if (e->o10680(nullptr, 0, 0) <= 0) return;
    *field += e->o10786(nullptr, 0, 0, nullptr) * st;
    const int r = e->fDF780();
    const uint32_t u = e->fE0BD0(r);
    if (u != 0) {
        const int16_t m = *(const int16_t*)(row + 0x108);
        if (m > 0 && m < *(const int*)(dt + 0xBC0)) e->o10689(u, m, 0);
    }
}

// 1.13c D2Game+0x92060
// param_1 (ecx) is forwarded to 0xE07A0; param_2 = unit. Returns 1 if #10392(unit[0xB]) yields a different
// unit, neither has #10057(#10331(.)) set, and its type is 0 or 1.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_92060(int p1, int32_t* unit, const D2GameB8E2Env* e)
{
    e->fE07A0(p1);
    int32_t* o = (int32_t*)(intptr_t)e->o10392(unit[0xB]);
    if (o != unit && o) {
        if (e->o10057(e->o10331(unit)) == 0 && e->o10057(e->o10331(o)) == 0 && *o > -1 && *o < 2) return 1;
    }
    return 0;
}

// 1.13c D2Game+0x92230
// param_1 (ecx) = object, param_2 (edx), param_4 = count. Needs 0x9FA30 true, count > 0 and 0x91340(param_2) >= 1;
// then 0xA0000(param_1) and returns 1.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_92230(void* p1, void* p2, int p3, int count, const D2GameB8E2Env* e)
{
    (void)p3;
    int out[2];
    if (e->f9FA30(p1, out) != 0 && count > 0) {
        if (e->f91340(p2) < 1) return 0;
        e->fA0000(p1);
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0x92BB0
// param_2 = unit; zeroes a 0x70-byte local, calls 0xFE5A0 with (#10653 + #10973(unit,0x145,0)); when bit0 of
// the result is set calls 0x92360; then 0xFDDE0(byte at +0x1A5, or 0x80 if zero). piStack_* are the caller's
// frame slots, carried as args.
EMSCRIPTEN_KEEPALIVE void d2_game_92bb0(uint32_t p1, void* unit, void* s14, void* sC, void* ebx, int s4,
                                        const uint8_t* s8, const D2GameB8E2Env* e)
{
    (void)p1;
    const int a = e->o10653();
    const int b = e->o10973(unit, 0x145, 0);
    uint32_t local[0x1C] = {0};
    const uint16_t r = (uint16_t)e->fFE5A0(s14, a + b, 0);
    if (r & 1) e->f92360(s14, local, sC, ebx, s4);
    uint8_t bv = s8[0x1A5];
    if (bv == 0) bv = 0x80;
    e->fFDDE0(bv, s14, sC);
}

// 1.13c D2Game+0x92DF0
// param_1 (ecx) -> 0xE07A0, param_2 = unit, param_3 = skills.txt row. Returns 1 after #10243(unit, #10786-1)
// and 0x92BB0; 0 on any failed gate (#10243(.,0) when the target is self/none).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_92df0(int p1, uint8_t* unit, int skillRow, const uint8_t* dt,
                                            const D2GameB8E2Env* e)
{
    const uint8_t* row = b8e2Row(dt, skillRow);
    if (!row) return 0;
    const int h = e->o10511(unit);
    if (!h) return 0;
    e->fE07A0(p1);
    uint8_t* t = (uint8_t*)(intptr_t)e->o10392(*(uint32_t*)(unit + 0x2C));
    if (t == unit || !t) { e->o10243(unit, 0); return 0; }
    if (e->o11138(unit, t) == 0) return 0;
    const int v = e->o10786(unit, *(const int*)(row + 0x138), skillRow, nullptr);
    e->o10243(unit, v - 1);
    (void)h;
    return 1;
}
