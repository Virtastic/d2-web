// Alpha batch D2Game+0x76B20..0x79AA0 (player/client glue). Written from the cached 1.13c decompile;
// no test runner run. Register-passed (unaff_*/in_EAX) args are explicit params; GUESS = arg Ghidra dropped.
#include <cstdint>
#include <cstdlib>

#define ORD(n, ret, ...) namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("Ordinal_" #n))) ret Ordinal_##n(__VA_ARGS__); } using namespace ordns_76b20
ORD(10024, void, const void*, int);
ORD(10049, void, void*);
ORD(10057, int, int);
ORD(10084, void, void*);
ORD(10141, void);
ORD(10155, uint32_t, int);
ORD(10157, void, int, uint32_t, int);
ORD(10158, void, void*);
ORD(10202, int, int, int, int, const char*);
ORD(10265, int, int);
ORD(10284, int, int);
ORD(10331, int);
ORD(10348, int, int);
ORD(10386, void, void*, int);
ORD(10392, void, int);
ORD(10422, void, int, int);
ORD(10460, int, int);
ORD(10464, int, int);
ORD(10494, int, void*, int);
ORD(10551, void);
ORD(10578, int);
ORD(10627, void);
ORD(10654, void, int, int*, int*, int, int, int, int);
ORD(10695, void, int);
ORD(10698, int, int);
ORD(10744, int, int, int);
ORD(10865, int, int);
ORD(10887, void, void*, int);       // GUESS: (unit, mode)
ORD(10910, void);
ORD(10920, void*, void*);
ORD(10973, int, int, int, int);     // GUESS: stat query (unit, stat, layer)
ORD(11004, void);
ORD(11017, int, int);
ORD(11067, uint32_t, int);
ORD(11088, int, void*);
ORD(11090, void, void*, int);
ORD(11103, void, void*, int);
ORD(11134, uint32_t, int);
ORD(11139, int, int, int);
ORD(11147, int, int);
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21320"))) void FUN_6fc21320(); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fd007a0"))) void FUN_6fd007a0(int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc95e10"))) void FUN_6fc95e10(int, void*); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fcffa90"))) int FUN_6fcffa90(); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fcae6f0"))) void FUN_6fcae6f0(); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fd00750"))) void FUN_6fd00750(); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fcf5740"))) void FUN_6fcf5740(int*, int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc31ca0"))) int FUN_6fc31ca0(int, int, int, int*); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc97d90"))) void FUN_6fc97d90(); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc98d00"))) void FUN_6fc98d00(void*, int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc307e0"))) int* FUN_6fc307e0(void*); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21280"))) int FUN_6fc21280(); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc2e990"))) void FUN_6fc2e990(int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fc57f80"))) void FUN_6fc57f80(void*, int*); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fcbc620"))) void FUN_6fcbc620(int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fcb8250"))) void FUN_6fcb8250(int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void FUN_6fcaea80(int, int*, int, int, int, int, int); } using namespace ordns_76b20;
namespace ordns_76b20 { __attribute__((weak, import_module("env"), import_name("FUN_6fd102a0"))) int FUN_6fd102a0(); } using namespace ordns_76b20;
extern "C" void* PTR_FUN_6fd1bc1c;
extern "C" int* sgptDataTables;
extern "C" const char DAT_6fd19ff9[];

typedef uint8_t u8;
template <class T> static inline T& at(void* p, int off) { return *(T*)((u8*)p + off); }
[[noreturn]] static void fatal(int id) { Ordinal_10024(DAT_6fd19ff9, Ordinal_10265(id)); _Exit(-1); }

// 1.13c D2Game+0x76B20  (fastcall: ecx=game, edx=client)
extern "C" void D2GAME_76B20(int game, int* cl)
{
    Ordinal_11103(cl, 0);
    if (cl) FUN_6fc21320();
    FUN_6fd007a0(game);
    Ordinal_10392(cl[0xb]);
    u8 tmp[64]; // GUESS: stack out-buffer for 0x75E10
    FUN_6fc95e10(game, tmp);
    int r = Ordinal_10494(cl, 1);
    if ((r == 0 || FUN_6fcffa90() != 0) && PTR_FUN_6fd1bc1c)
        ((void (*)(int, int))PTR_FUN_6fd1bc1c)(0, 0);
}

// 1.13c D2Game+0x77A50  (REG: edi = obj)
extern "C" void D2GAME_77A50(void* edi)
{
    uint32_t a = Ordinal_10973(0, 0, 0); // GUESS args
    int b = Ordinal_10973(0, 0, 0);      // GUESS args
    int t = edi ? at<int>(edi, 0x10) : 0;
    switch (t) {
    case 1: case 5: case 6: break;
    case 2: if ((a & 0xffffff00) == 0) return; break;
    default: if (b < 1000) return; break;
    }
    if ((int)a < Ordinal_10578()) Ordinal_10887(edi, 0); // GUESS args
}

// 1.13c D2Game+0x77BB0  (REG: edi = unit)
extern "C" int D2GAME_77BB0(void* edi)
{
    if (!edi) return 0;
    if (Ordinal_10057(Ordinal_10331())) return 1;
    int i = at<int>(edi, 4);
    char* dt = (char*)*sgptDataTables;
    if (i >= 0 && i < *(int*)(dt + 0xbc8) && (uintptr_t)(*(char**)(dt + 0xbc4) + i * 0xc4)) {
        if (at<int>(edi, 0x60)) {
            int p = Ordinal_11139(at<int>(edi, 0x60), 3);
            if (p) Ordinal_10695(*(int*)(p + 4));
            Ordinal_10910();
            Ordinal_10551();
            if (Ordinal_10973(0, 0, 0) > 0) return 1; // GUESS args
            Ordinal_10887(edi, 0);                    // GUESS args
        }
    }
    return 0;
}

// 1.13c D2Game+0x77E70  (fastcall: ecx=param_1, edx=param_2; EAX=unit)
extern "C" uint32_t D2GAME_77E70(int* p1, int p2, void* eax)
{
    int id;
    if (!p1) id = 0x38c;
    else if (!eax) id = 0x38d;
    else if (*p1 == 0) {
        uint32_t u = Ordinal_11134(at<int>(eax, 0x60));
        if ((uint32_t)p1[3] == u) return 1;
        if (p2) {
            int n = *(int*)(p2 + 0x1120 + (u & 0x7f) * 4);
            for (;; n = *(int*)(n + 0xe4)) {
                if (!n) return 0;
                if (*(uint32_t*)(n + 0xc) == u) break;
            }
            return FUN_6fd102a0();
        }
        id = 0x2d3;
    } else id = 0x38e;
    fatal(id);
}

// 1.13c D2Game+0x77F00  (REG: eax=unit, ebx=game, ebp=count)
extern "C" int D2GAME_77F00(void* eax, int ebx, int ebp)
{
    int list = at<int>(eax, 0x60);
    if (!list) return 0;
    int it = Ordinal_10284(list);
    while (it) {
        int next = Ordinal_10348(it);
        uint32_t k = Ordinal_10155(it);
        if (!ebx) fatal(0x2d3);
        bool found = false;
        for (int n = *(int*)(ebx + 0x1120 + (k & 0x7f) * 4); n; n = *(int*)(n + 0xe4))
            if (*(uint32_t*)(n + 0xc) == k) { ebp++; found = true; break; }
        if (!found) Ordinal_10157(list, k, 1);
        it = next;
    }
    return ebp;
}

// 1.13c D2Game+0x78230  (REG: eax=client)
extern "C" void D2GAME_78230(int* eax)
{
    Ordinal_10887(0, 0); // GUESS args
    Ordinal_11004();
    Ordinal_10627();
    if (eax && *eax == 0) {
        int v = *(int*)((u8*)Ordinal_10920(eax) + 0x9c); // GUESS arg
        if (v) {
            *(uint32_t*)(v + 0x3d4) &= 0xfffffffd;
            Ordinal_10084(eax); // GUESS arg
            for (char* p = (char*)eax[0x37]; p;) { char c = *p; p = *(char**)(p + 0x24); if (c == 8) FUN_6fcae6f0(); }
            for (char* p = (char*)eax[0x37]; p;) { char c = *p; p = *(char**)(p + 0x24); if (c == 9) FUN_6fcae6f0(); }
            FUN_6fd00750();
            return;
        }
    }
    fatal(0x7ba);
}

// 1.13c D2Game+0x78A90  (fastcall ecx=param_1; REG: ebx=client, edi=flag)
extern "C" void D2GAME_78A90(int param_1, int* ebx, int edi)
{
    if (!ebx || *ebx != 0) return;
    int list = ebx[0x18];
    int it = Ordinal_10460(list);
    while (it) {
        if (Ordinal_10698(it) == 3) {
            int u = Ordinal_11147(it);
            if (Ordinal_10744(u, 0x2d) && Ordinal_10865(u) && Ordinal_10973(u, 0x48, 0) <= 0 &&
                Ordinal_10202(u, 0x100, 0x5e8, "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp") == 0) {
                if (!edi) { Ordinal_11090(0, 0); edi = 1; } // GUESS args
                FUN_6fcf5740(ebx, param_1);
                it = Ordinal_10460(list);
                continue;
            }
        }
        it = Ordinal_10464(it);
    }
}

// 1.13c D2Game+0x78C50  (REG: eax=a, ebx=obj, edi=n)
extern "C" void D2GAME_78C50(int eax, int ebx, int edi)
{
    if (edi <= 0) return;
    Ordinal_10141();
    int* p = (int*)(intptr_t)FUN_6fc31ca0(Ordinal_10331(), eax, 4, (int*)1);
    if (!p) return;
    if (Ordinal_11088(p) != 4) fatal(0x24b);
    if (*p == 0) Ordinal_10049(p);
    Ordinal_10887(p, 0xe);
    Ordinal_10386(p, ebx ? *(int*)(ebx + 0xc) : -1);
}

// 1.13c D2Game+0x78FA0  (REG: eax=client; stack param_1)
extern "C" void D2GAME_78FA0(int* eax, int param_1)
{
    int list = eax[0x18];
    int slot = -1;
    int pt[2] = {list, 0}; // GUESS: local_14/iStack_10 (x,y) are set by callee, not shown by decompile
    do {
        int cur = slot;
        void* o = (void*)(intptr_t)(cur == -1 ? Ordinal_11017(list) : Ordinal_11139(list, cur));
        if (o) {
            if (cur == -1) FUN_6fc97d90();
            else {
                if (at<int>(o, 0x10) != 1) fatal(0x2a7);
                FUN_6fc98d00(o, param_1);
            }
            int* q = FUN_6fc307e0(o);
            Ordinal_10141();
            int g = Ordinal_10331();
            int a[2] = {pt[0] + 2, pt[1] + 3};
            if (!FUN_6fc21280()) { a[0] = pt[0]; a[1] = pt[1]; }
            Ordinal_10654(g, a, pt, 1, 0x3e01, 0x801, 1);
            FUN_6fc2e990(a[1]);
            list = pt[0];
            cur = a[0];
            (void)q;
        }
        slot = cur + 1;
    } while (slot < 0xd);
    Ordinal_10973(0, 0, 0); // GUESS args
    if (*eax == 0) Ordinal_10049(eax);
    Ordinal_10887(eax, 0);  // GUESS args
    FUN_6fc57f80((void*)(intptr_t)eax[3], eax);
}

// 1.13c D2Game+0x79A60  (fastcall)
extern "C" void D2GAME_79A60(int game, int* unit)
{
    FUN_6fcbc620(game);
    FUN_6fcb8250(game);
    FUN_6fcaea80(game, unit, 0xb, *(int*)(game + 0xa8) + 0x1e, 0, 0, 0);
}

// 1.13c D2Game+0x79AA0  (fastcall)
extern "C" void D2GAME_79AA0(int game, int* unit)
{
    int v;
    if (!unit || !(v = unit[0x29])) return;
    uint32_t t = Ordinal_11067(v);
    if (t <= *(uint32_t*)(game + 0xa8)) {
        Ordinal_10422(*(int*)(game + 0x1c), v);
        unit[0x29] = 0;
        Ordinal_10158(unit);
        unit[0x31] |= 0x100;
        return;
    }
    FUN_6fcaea80(game, unit, 6, t, 0, 0, 0);
}
