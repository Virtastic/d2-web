// Alpha batch D2Game+0x68D70..0x6A220 (quest/AI glue). Written from the cached 1.13c decompile;
// no test runner run. Offsets are 1.13c 32-bit layouts; Ordinal_N are D2Common/D2Win imports, bound weakly
// until the names are mapped. REG? = input arrives in a register the decompiler could not name.
#include <cstdint>
#include <cstdlib>

#define ORD(n, ret, ...) namespace ordns_68d70 { __attribute__((weak, import_module("env"), import_name("Ordinal_" #n))) ret Ordinal_##n(__VA_ARGS__); } using namespace ordns_68d70
ORD(11090, void, void*, int);     // UNITS_SetMode
ORD(10688, void*, int);
ORD(10331, void*, void*);         // UNITS_GetGame
ORD(10826, int, void*);           // difficulty/level of game
ORD(10118, void, void*, int);
ORD(10920, void*, void*);
ORD(10460, void*, void*);         // inventory first item
ORD(10464, void*, void*);         // inventory next item
ORD(11147, void*, void*);
ORD(10744, int, void*);
ORD(10296, void*, void*);         // MONSTATS record
ORD(10158, void, void*);
ORD(10181, void**, void);
ORD(10141, void, void*, void*);
ORD(10589, int, void*);
ORD(10904, int, void*);
ORD(10303, short, int, void*, int, int, int, int);
ORD(10265, int, int);
ORD(10024, void, const char*, int);
ORD(10045, void*, const char*, int, int);
namespace ordns_68d70 { __attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void* FUN_6fcaea80(void* game, void* unit, int mode, int frame, int, int, int); } using namespace ordns_68d70; // EventQueue_Add
namespace ordns_68d70 { __attribute__((weak, import_module("env"), import_name("FUN_6fc54b80"))) void* FUN_6fc54b80(void* unit, int); } using namespace ordns_68d70;
namespace ordns_68d70 { __attribute__((weak, import_module("env"), import_name("FUN_6fc55eb0"))) void FUN_6fc55eb0(); } using namespace ordns_68d70;
namespace ordns_68d70 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21780"))) int FUN_6fc21780(void*); } using namespace ordns_68d70;
namespace ordns_68d70 { __attribute__((weak, import_module("env"), import_name("FUN_6fc31a70"))) int FUN_6fc31a70(void*, int, int*, int, int, int); } using namespace ordns_68d70;

typedef uint8_t u8;
template <class T> static inline T& at(void* p, int off) { return *(T*)((u8*)p + off); }
[[noreturn]] static void assertFail(int line) { Ordinal_10024("D2Game", Ordinal_10265(line)); exit(-1); }

// 1.13c D2Game+0x68D70  (REG: param is ecx; ctx = {game, unit, ?, ?, unitType})
int D2GAME_68D70_Quest(int* ctx)
{
    void* unit = (void*)ctx[1];
    if (!unit || at<int>(unit, 0x10) == 0) {
        Ordinal_11090(unit, 1);
        void* rec = Ordinal_10688(ctx[4]);
        FUN_6fcaea80((void*)ctx[0], (void*)ctx[1], 1,
                     (at<uint32_t>(rec, 0xdc) >> 8) + at<int>((void*)ctx[0], 0xa8), 0, 0, 0);
    }
    void* q = at<void**>((void*)ctx[0], 0x10f4);
    int* p = q ? *(int**)q : nullptr;
    if (!p) return 0;
    while (*p != 5) { p = (int*)p[0x3d]; if (!p) return 0; }
    if (at<char>(p, 9) && (FUN_6fc54b80((void*)ctx[1], 0x7f), at<u8>(p, 12) < 2) &&
        (FUN_6fc55eb0(), at<char>(p, 0xb) == 0))
        at<char>((void*)p[6], 0x11b) = 1;
    return 0;
}

// 1.13c D2Game+0x68E20  (caller 0x36C50)
void D2GAME_68E20_GoldTick(void* cb, int* unit)
{
    void* d = at<void*>(cb, 0x18);
    if (at<char>(d, 0x90) == 0) {
        at<char>(d, 0x90) = 1;
        void* qs = at<void*>(at<void*>(cb, 4), 0x10f4);
        if (!qs) assertFail(0x1fb);
        at<int>(d, 0x8c) = FUN_6fc21780((u8*)qs + 0x18);
        unit[0x2e] = 0x20646c67; // "gld "
    }
    if (at<int>(d, 0x8c)) {
        int* out = nullptr;
        if (FUN_6fc31a70(unit, 2, (int*)&out, 0, -1, 0) && --at<int>(d, 0x8c) > 0)
            FUN_6fcaea80(at<void*>(cb, 4), unit, 7, at<int>(at<void*>(cb, 4), 0xa8) + 10, 0, 0, 0);
    }
}

// 1.13c D2Game+0x68F00  (REG?: ebx=cb, edi=unit; partial-body tail of 0x36C50 caller frame)
void D2GAME_68F00_Tail(void* cb /*ebx*/, int* unit /*edi*/)
{
    int mode = unit ? unit[4] : 0;
    bool kill = false;
    if (mode == 1 || mode == 3) kill = true;
    else if (mode == 2) {
        void* d = at<void*>(cb, 0x18);
        if (Ordinal_10826(Ordinal_10331(nullptr /*REG? eax*/)) == 1) {
            if (++at<int>(d, 0x80) > 5) at<char>(d, 0x92) = 1;
            kill = at<char>(d, 0x92) != 0;
        } else kill = at<char>(d, 0x91) != 0;
    }
    if (kill) Ordinal_11090(nullptr /*REG? eax*/, 0);
    FUN_6fcaea80(at<void*>(cb, 4), unit, 7, at<int>(at<void*>(cb, 4), 0xa8) + 0x19, 0, 0, 0);
}

// 1.13c D2Game+0x69200  (a1q5 quest init; function-table slots at +0xa0..; callbacks are D2Game RVAs)
void D2GAME_69200_A1Q5Init(void* q)
{
    for (int i = 0; i < 15; i++) at<int>(q, 0xa0 + i * 4) = 0;
    // TODO callbacks: +0xa0=0x81030 +0xac=0x842d0 +0xcc=0x84430 +0xc8=0x811b0 +0xc0=0x88b90
    //   +0xd4=0x80e00 +0xec=0x80f20 +0xf0=0x801b0 ; +0xdc=table 0x729c38 ; +0xe0=5
    at<u8>(q, 0xb) = 0; at<u8>(q, 10) = 1; at<u8>(q, 0xc) = 0; at<int>(q, 0xe8) = 0; at<u8>(q, 0xd) = 4;
    void* d = Ordinal_10045("..\\Source\\D2Game/Quests/a1q5.cpp", 0x40f, 0);
    at<void*>(q, 0x18) = d;
    at<int>(q, 0x10) = 3;
    for (int i = 0; i < 0x48; i++) ((int*)d)[i] = 0;
    at<uint16_t>(d, 0x10c) = 0;
}

// 1.13c D2Game+0x695F0  (REG?: esi=key, caller-frame retaddr holds unit; table at 0x727e50, count 0x727f68)
void D2GAME_695F0_TableLookup(void* unit, int key, const int (*tbl)[2], int count)
{
    void** a = (void**)at<void*>(Ordinal_10920(nullptr /*REG? eax*/), 0x60 + at<u8>(unit, 0x6d) * 4);
    for (int i = 0; i < count; i++)
        if (tbl[i][0] == key) { Ordinal_10118(**(void***)a, tbl[i][1]); return; }
    Ordinal_10118(**(void***)a, 0);
}

// 1.13c D2Game+0x698E0  (caller 0x6A7A0; REG: eax=obj)
int D2GAME_698E0_HasItem(void* obj)
{
    if (!at<void*>(obj, 0x60)) assertFail(0x6d3);
    for (void* it = Ordinal_10460(at<void*>(obj, 0x60)); it; it = Ordinal_10464(it))
        if (Ordinal_10744(Ordinal_11147(it))) return 1;
    return 0;
}

// 1.13c D2Game+0x69970  (dispatch by nAnimMode; handler tables at 0x727b8c / default 0x727b94)
void D2GAME_69970_Dispatch(void* game, void* unit, const void* const* handlers, int nHandlers)
{
    int lvl = Ordinal_10826(Ordinal_10331(unit));
    u8 k = at<u8>(at<void*>(unit, 0x14), 4) & 0x7f;
    if (k >= nHandlers) return;
    // REG?: 8 => lvl>0x4a, 3 => lvl<0x28 && lvl!=0x19, 1|4 => lvl<0x28 select alt handler [0x727b94]
    // (handler arity unknown; body not translated)
    (void)game; (void)handlers; (void)lvl;
}

// 1.13c D2Game+0x69B00  (REG?: ecx=param_1, eax=item, edx=unit, stack=out)
void D2GAME_69B00_Calc(int p1, int* unit, int* out, void* item);

// 1.13c D2Game+0x69CA0  (REG: edx=unit)
void D2GAME_69CA0_StepFrame(void* unit)
{
    u8 cur = at<u8>(at<void*>(unit, 0x14), 4);
    void* rec = Ordinal_10296(unit);
    int step = at<int>(rec, 0x180);
    if ((uint32_t)cur / step > 1) assertFail(0xdc);
    u8 n = cur + 1;
    at<u8>(at<void*>(unit, 0x14), 4) = n;
    u8 lim = (u8)(step << 1);
    if ((int)(lim - cur) < 0) {
        at<u8>(at<void*>(unit, 0x14), 4) = lim;
        if ((int)((u8)(at<u8>(rec, 0x180) << 1) - cur) < 0) assertFail(0xe3);
    } else {
        if (n <= lim && (uint32_t)n % (at<u8>(rec, 0x180) & 0x7f) == 0)
            Ordinal_11090(unit, 2 - (int)((uint32_t)n / step));
        Ordinal_10158(unit);
        at<uint32_t>(unit, 0xc4) |= 1;
    }
}

// 1.13c D2Game+0x69E50  (caller 0x6E680; REG?: edi=monster, ecx=idx; table stride 12 at 0x727d50)
int D2GAME_69E50_RollAction(uint32_t idx, void* mon, int nEntries, uint32_t (*tbl)[3])
{
    idx &= 0xff;
    if ((int)idx >= nEntries) return 0;
    uint64_t s = (uint64_t)at<uint32_t>(mon, 0x20) * 0x6ac690c5ull + at<uint32_t>(mon, 0x24);
    at<uint64_t>(mon, 0x20) = s;
    if ((uint32_t)s % 100 < tbl[idx][1]) {
        auto fn = (int (*)(uint32_t))(uintptr_t)tbl[idx][0];
        if (!fn) assertFail(0x10f5);
        return fn(tbl[idx][2]);
    }
    return 0;
}

// 1.13c D2Game+0x6A1A0  (caller 0x6F520; REG: ecx=msg, eax=game, edi=unit)
void D2GAME_6A1A0_ClearRef(void* msg, void* game, void* unit)
{
    if (!msg || at<int>(msg, 4) != 0x151) return;
    uint32_t* ref = (uint32_t*)*Ordinal_10181();
    if (unit) {
        if (!ref) return;
        if (!game) assertFail(0x2d3);
        void* it = at<void*>(game, 0x1120 + (*ref & 0x7f) * 4);
        while (it && at<uint32_t>(it, 0xc) != *ref) it = at<void*>(it, 0xe4);
        if (it != unit) return;
    }
    if (ref) *ref = 0xffffffff;
}

// 1.13c D2Game+0x6A220  (REG: ecx=unitId, eax=game, edi=arg)
void D2GAME_6A220_ModeSwitch(uint32_t id, void* game, int arg)
{
    if (!game) assertFail(0);
    void* u = at<void*>(game, 0x1520 + (id & 0x7f) * 4);
    while (u && at<uint32_t>(u, 0xc) != id) u = at<void*>(u, 0xe4);
    if (!u) assertFail(0xd37);
    int buf[3];
    void* g = Ordinal_10331(u);
    Ordinal_10141(u, buf);
    int a = Ordinal_10589(u), b = Ordinal_10904(u);
    int m = at<int>(u, 0x10);
    if (m == 0) Ordinal_10303((int)(intptr_t)g, u, arg, a, b, 0x8180);
    else if (m == 2 || m == 5) {
        short r = Ordinal_10303((int)(intptr_t)g, u, arg, a, b, 0x8180);
        if (r == 0) { if (m != 2) Ordinal_11090(u, 2); }
        else if (m != 5) Ordinal_11090(u, 5);
    }
}
