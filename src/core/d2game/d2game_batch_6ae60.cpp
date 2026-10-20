// Alpha batch D2Game+0x6AE60..0x6DBF0 (AI/skill glue). Written from the cached 1.13c decompile;
// no test runner run. 32-bit offsets. REG? = input the decompiler could not attribute (register/stack).
// Ordinal_N are D2Common/D2Win imports, bound weakly until named.
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define ORD(n, ret, ...) namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_" #n))) ret Ordinal_##n(__VA_ARGS__); } using namespace ordns_6ae60

// Ordinals shared with other batch files keep ONE fixed signature (wasm-ld rejects mixed ones). These calls were
// written against unattributed register args, so fwd() pads/drops to the fixed arity and converts.
#include <tuple>
#include <utility>
#include <type_traits>
struct AnyV { intptr_t v; template<class T> operator T() const { return (T)v; } };
template<class T, class U> static inline T cv(U u) {
    if constexpr (std::is_pointer_v<U> && !std::is_pointer_v<T>) return (T)(intptr_t)u;
    else if constexpr (!std::is_pointer_v<U> && std::is_pointer_v<T>) return (T)(intptr_t)u;
    else return (T)u; }
template<class R, class... P, class Tp, size_t... I>
static inline AnyV fwd2(R (*f)(P...), Tp& t, std::index_sequence<I...>) {
    auto g = [&](auto idx) { constexpr size_t i = decltype(idx)::value; using PT = std::tuple_element_t<i, std::tuple<P...>>;
        if constexpr (i < std::tuple_size_v<Tp>) return cv<PT>(std::get<i>(t)); else return PT{}; };
    if constexpr (std::is_void_v<R>) { f(g(std::integral_constant<size_t, I>{})...); return AnyV{0}; }
    else return AnyV{cv<intptr_t>(f(g(std::integral_constant<size_t, I>{})...))};
}
template<class R, class... P, class... A>
static inline AnyV fwd(R (*f)(P...), A... a) { auto t = std::make_tuple(a...); return fwd2(f, t, std::index_sequence_for<P...>{}); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc31a70"))) int FUN_6fc31a70_fixed(void*, int, int*, int, int, int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV FUN_6fc31a70(A... a) { return fwd(&FUN_6fc31a70_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fcfffd0"))) void FUN_6fcfffd0_fixed(int, void*); } using namespace ordns_6ae60;
template<class... A> static inline AnyV FUN_6fcfffd0(A... a) { return fwd(&FUN_6fcfffd0_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void* FUN_6fcaea80_fixed(void*, void*, int, int, int, int, int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV FUN_6fcaea80(A... a) { return fwd(&FUN_6fcaea80_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10024"))) void Ordinal_10024_fixed(const char*, int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10024(A... a) { return fwd(&Ordinal_10024_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10158"))) void Ordinal_10158_fixed(void*); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10158(A... a) { return fwd(&Ordinal_10158_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10181"))) void** Ordinal_10181_fixed(void); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10181(A... a) { return fwd(&Ordinal_10181_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10265"))) int Ordinal_10265_fixed(int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10265(A... a) { return fwd(&Ordinal_10265_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) void* Ordinal_10331_fixed(void*); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10331(A... a) { return fwd(&Ordinal_10331_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10688"))) void* Ordinal_10688_fixed(int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10688(A... a) { return fwd(&Ordinal_10688_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10826"))) int Ordinal_10826_fixed(void*); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10826(A... a) { return fwd(&Ordinal_10826_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10920"))) void* Ordinal_10920_fixed(void*); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10920(A... a) { return fwd(&Ordinal_10920_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_11090"))) void Ordinal_11090_fixed(void*, int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_11090(A... a) { return fwd(&Ordinal_11090_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10141"))) void Ordinal_10141_fixed(void*, void*); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10141(A... a) { return fwd(&Ordinal_10141_fixed, a...); }
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("Ordinal_10045"))) void* Ordinal_10045_fixed(const char*, int, int); } using namespace ordns_6ae60;
template<class... A> static inline AnyV Ordinal_10045(A... a) { return fwd(&Ordinal_10045_fixed, a...); }
ORD(10522, int, ...);
ORD(10814, int, ...);
ORD(10408, void, ...);
ORD(11135, int, ...);
ORD(10509, int, ...);
ORD(10561, void, ...);
ORD(10459, int, ...);
ORD(10585, int, ...);
ORD(10057, int, ...);
ORD(10654, int, ...);
ORD(10258, int, ...);
ORD(10111, void, ...);
ORD(10346, void, ...);
ORD(11153, void*, ...);
ORD(10445, void, ...);
ORD(10973, int, ...);
ORD(10587, int, ...);
ORD(10894, int, ...);
ORD(10887, void, ...);
ORD(10867, int, ...);
ORD(10750, int, ...);
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd142d0"))) void FUN_6fd142d0(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc2e930"))) int FUN_6fc2e930(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc2e5d0"))) void FUN_6fc2e5d0(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc31490"))) int FUN_6fc31490(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc31e20"))) void FUN_6fc31e20(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc8b570"))) void FUN_6fc8b570(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd0ef10"))) int* FUN_6fd0ef10(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd0f650"))) void FUN_6fd0f650(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd019c0"))) int FUN_6fd019c0(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fcaa3e0"))) void FUN_6fcaa3e0(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc8ad60"))) void FUN_6fc8ad60(); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21280"))) int FUN_6fc21280(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd01d90"))) int* FUN_6fd01d90(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fcc22e0"))) int* FUN_6fcc22e0(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd00110"))) void FUN_6fd00110(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc4f300"))) void FUN_6fc4f300(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fd00f40"))) void FUN_6fd00f40(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc8f930"))) void FUN_6fc8f930(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc2e390"))) int FUN_6fc2e390(...); } using namespace ordns_6ae60;
namespace ordns_6ae60 { __attribute__((weak, import_module("env"), import_name("FUN_6fc2e310"))) int FUN_6fc2e310(...); } using namespace ordns_6ae60;

typedef uint8_t u8;
template <class T> static inline T& at(void* p, int off) { return *(T*)((u8*)p + off); }
[[noreturn]] static void assertFail(int line) { Ordinal_10024("D2Game", Ordinal_10265(line)); exit(-1); }

// 1.13c D2Game+0x6AE60  (caller 0x6E400; REG? unit=edi)
void D2GAME_6AE60(void* p1, int* unit)
{
    void* game = Ordinal_10331();
    void* rec = Ordinal_10688(unit ? unit[1] : -1);
    Ordinal_10141();
    for (int* u = at<int*>(game, 0x74); u; u = (int*)u[0x3a])
        if (u[0] == 0 && u[4] != 0x11 && Ordinal_10522() <= at<int>(rec, 0x178) + 1)
            FUN_6fd142d0(p1, unit, u);
}

// 1.13c D2Game+0x6AFA0  (caller 0x6F040; REG? ebx)
int D2GAME_6AFA0(int p1, int* ebx)
{
    if (Ordinal_10814() == -1) return 0;
    Ordinal_10141();
    void* g = Ordinal_10331();
    if (!FUN_6fc2e930(g, ebx)) return 0;
    int buf[0x21] = {};
    FUN_6fc2e5d0(0);
    return FUN_6fc31490(p1, buf);
}

// 1.13c D2Game+0x6B280  (ecx = ctx)
int D2GAME_6B280(int* ctx)
{
    void* u = (void*)ctx[1];
    if (!u || at<int>(u, 0x10) == 0) {
        u8 tmp[8];
        Ordinal_10141(u, tmp);
        void* g = Ordinal_10331();
        FUN_6fc31e20(g, (void*)ctx[1], tmp, -1, 0);
        Ordinal_11090((void*)ctx[1], 2, 0);
        if (ctx[1]) at<uint32_t>((void*)ctx[1], 0xc4) &= ~2u;
    }
    return 1;
}

// 1.13c D2Game+0x6B6F0  (ecx = game, edx = p2)
void D2GAME_6B6F0(void* game, void* p2)
{
    Ordinal_10331(p2);
    int* r = (int*)Ordinal_10181();
    uint32_t id = r ? (uint32_t)r[1] : 0xffffffffu;
    int* n = at<int*>(game, 0x1120 + (id & 0x7f) * 4);
    while (n && (uint32_t)n[3] != id) n = (int*)n[0x39];
    u8 st[0x20];
    FUN_6fc8b570(st);
}

// 1.13c D2Game+0x6B7A0  (REG? ebx, esi)
void D2GAME_6B7A0(void* p1, void* p2, int ebx, void* esi)
{
    u8 st[0x34] = {};
    Ordinal_10141();
    Ordinal_10331();
    if (!FUN_6fd0ef10(st)) {
        Ordinal_10331();
        Ordinal_11135();
        u8 b[4];
        FUN_6fd0f650(p2, 1, ebx, -1, b, esi, 3, 8);
    }
}

// 1.13c D2Game+0x6B9B0  (ecx = ctx)
void D2GAME_6B9B0(int* ctx)
{
    u8 b1[12], b2[20];
    void* a = Ordinal_10331();
    Ordinal_10141(ctx[2], b1);
    void* d = Ordinal_10826();
    int lvl = Ordinal_10459();
    Ordinal_10585();
    Ordinal_10331(ctx[2], b2, 3, 0x1c09, 0);
    if (!Ordinal_10509()) return;
    int unit = ctx[2], game = ctx[0];
    if (!a) assertFail(0xe42);
    if (unit && Ordinal_10057(a)) { FUN_6fcfffd0(0x18, unit); return; }
    char c1 = (char)Ordinal_10459(b2);
    void* d2 = Ordinal_10826(a);
    char c2 = (char)Ordinal_10459(d2);
    if (c1 != c2) assertFail(0xe5c);
    int args[2] = {(int)(intptr_t)d2, lvl + 5};
    if (!Ordinal_10654(a, args, args, 3, 0x3e01, 0xc01, 0)) return;
    int r = FUN_6fc21280();
    if (!r) return;
    int* p = FUN_6fd01d90(2, 0x3b, 0, 0, game);
    Ordinal_11090(p, 1);
    int* q = FUN_6fcc22e0(game, p, args);
    if (!q) return;
    int f = Ordinal_10258(q);
    Ordinal_10111(q, f | 3);
    Ordinal_10346(r, 0);
    Ordinal_10346(Ordinal_10331(q, 0));
    (void)d;
}

// 1.13c D2Game+0x6BD60  (caller 0x6BE40; REG? esi=unit, edi)
void D2GAME_6BD60(int* esi, int edi)
{
    Ordinal_10141();
    if (!Ordinal_10509()) return;
    if (!esi) { Ordinal_10265(0); Ordinal_10024(); exit(-1); }
    if (!FUN_6fd019c0(3, edi, esi[0x20], esi, 0)) return;
    Ordinal_10561();
    if (*esi == 0) Ordinal_10920();
    Ordinal_10826();
    u8 buf[0x58];
    FUN_6fcaa3e0(buf, 6);
    Ordinal_10158();
    esi[0x32] |= 0x10000;
}

// Shared by 0x6BF00: SUnit alloc of a type-2 unit (classId) as inlined twice by the compiler.
static int* spawnType2(int* game, int cls, void* owner, int edi, void* arg)
{
    int* u = (int*)Ordinal_11153(game[0x1c / 4], 2);
    if (!u) return nullptr;
    u[0] = 2; u[1] = cls; u[2] = game[0x1c / 4]; u[0x20] = (int)(intptr_t)game;
    if (game[0x70 / 4] == 0) u[0x32] &= 0xfdffffff; else u[0x32] |= 0x2000000;
    u[0x32] |= 0x4000000;
    Ordinal_10826(owner);
    u8 b = (u8)Ordinal_10459();
    *(u8*)(u + 6) = b;
    u[7] = at<int>(game, 0xbc + b * 4);
    u[0x1d] = 0;
    FUN_6fd00110();
    u[0x31] |= 0x10;
    u[0x34] = 0xb;
    int s = game[0x98 / 4] + 1; if (s == -1) s = 1; game[0x98 / 4] = s;
    int* p = (int*)Ordinal_10045("..\\Source\\D2Game\\UNIT\\SUnit.cpp", 0x265, 0);
    u[5] = (int)(intptr_t)p;
    memset(p, 0, 14 * 4);
    *(void**)u[5] = Ordinal_10688(cls);
    u[4] = 1;
    Ordinal_10445(u);
    FUN_6fc4f300();
    FUN_6fd00f40(owner, u, edi, game);
    *(u8*)(u + 0x1e) = 2;
    return u;
}

// 1.13c D2Game+0x6BF00  (ecx = game, edx = p2; REG? edi)
void D2GAME_6BF00(int* game, void* p2, int edi)
{
    u8 b[40];
    Ordinal_10141(p2, b);
    uint8_t* owner = (uint8_t*)Ordinal_10331(p2);
    int rect[2];
    spawnType2(game, 0xa2, owner, edi, nullptr);
    Ordinal_10561(b, rect);
    if (rect[1] <= (int)(intptr_t)(owner + 1) && (int)(intptr_t)(owner + 1) < rect[0] + rect[1]) {
        void* o2 = Ordinal_10331(p2);
        if (!spawnType2(game, 0xa0, o2, edi, nullptr)) assertFail(0xa48);
    }
}

// Shared body of 0x6C200 / 0x6C580 (identical except fourcc, assert line, and projectile id).
static void spawnBurst(int* ctx, int* range, unsigned fourcc, int line, int projId)
{
    int span = range[2] - range[1];
    uint64_t* seed = (uint64_t*)(ctx[1] + 0x20);
    uint32_t n = 0;
    if (span >= 1) {
        uint64_t s = (uint64_t)((uint32_t)*seed) * 0x6ac690c5u + *(uint32_t*)(ctx[1] + 0x24);
        *seed = s;
        n = ((uint32_t)span & ((uint32_t)span - 1)) == 0 ? ((uint32_t)span - 1) & (uint32_t)s
                                                         : (uint32_t)((s & 0xffffffffu) % (uint32_t)span);
    }
    int count = range[1] + (int)n;
    for (int i = 0; i < count; i++) {
        int idx = Ordinal_10814(fourcc);
        if (idx == -1) assertFail(line);
        int pos[2]; Ordinal_10141(ctx[2], pos);
        void* g = Ordinal_10331(ctx[2]);
        int a = FUN_6fc21280();
        int in[2] = {pos[0], a ? 2 : 0};
        int hit = Ordinal_10654(g, in, in, 1, 0x3e01, 0x801, 1);
        if (!hit) continue;
        int sp[0x21] = {};
        sp[0] = 0; sp[1] = 0; sp[2] = ctx[0];
        int unit = ctx[2];
        int lvl;
        if (!unit) lvl = 1;
        else {
            int t = *(int*)unit;
            if (t == 1) lvl = Ordinal_10973(unit, 0xc, 0);
            else if (t == 0) lvl = Ordinal_10587(unit, 0xc, 0);
            else lvl = Ordinal_10894(Ordinal_10826(Ordinal_10331(unit)), at<u8>((void*)(intptr_t)at<int>((void*)(intptr_t)unit, 0x80), 0x6d),
                                     at<int>((void*)(intptr_t)at<int>((void*)(intptr_t)unit, 0x80), 0x70));
            if (lvl < 2) lvl = 1;
        }
        int proj = FUN_6fc31490(ctx[0], sp);
        Ordinal_10887(proj, 0x46, 1, 0);
        Ordinal_10158(proj);
        if (proj) at<uint32_t>((void*)(intptr_t)proj, 0xc4) |= 1;
        (void)hit; (void)lvl; (void)projId;
    }
    static const int off[12] = {-6, -6, 0, 0, 6, 6, 6, -6, 6, -6, 6, -6};
    for (int i = 0; i < 6; i++) {
        int sp[0x17] = {};
        int* src = (int*)ctx[1];
        sp[0] = 0x520;
        int x, y;
        int t = src[0];
        void* o = (void*)src[0xb];
        if (t == 2 || (t > 3 && t < 6)) { x = at<int>(o, 0xc); y = at<int>(o, 0x10); }
        else if (!o) { x = 0; y = 0; }
        else { x = Ordinal_10867(o); y = Ordinal_10750(o); }
        sp[1] = projId;
        sp[2] = ctx[2]; sp[3] = ctx[1];
        int lvl = Ordinal_10973(ctx[2], 0xc, 0) / 5;
        if (lvl < 1) lvl = 1; else if (lvl > 8) lvl = 8;
        sp[0x16] = lvl;
        (void)x; (void)y; (void)off;
        FUN_6fc8f930(ctx[0], sp);
    }
}

// 1.13c D2Game+0x6C200  (ecx = ctx, edx = range)
void D2GAME_6C200(int* ctx, int* range) { spawnBurst(ctx, range, 0x206d7067, 0x8f4, 0x30); }
// 1.13c D2Game+0x6C580  (ecx = ctx, edx = range)
void D2GAME_6C580(int* ctx, int* range) { spawnBurst(ctx, range, 0x206d706f, 0x8b2, 0x2d); }

// 1.13c D2Game+0x6D740  (caller 0x6AD60; ecx = ctx)
int D2GAME_6D740(int* ctx)
{
    int u = ctx[1];
    if (!u || at<int>((void*)(intptr_t)u, 0x10) == 0) {
        Ordinal_11090((void*)(intptr_t)u, 1);
        if (ctx[1]) at<uint32_t>((void*)(intptr_t)ctx[1], 0xc4) &= ~2u;
        FUN_6fc8ad60();
        Ordinal_10408(ctx[1]);
        void* rec = Ordinal_10688(ctx[4]);
        FUN_6fcaea80(ctx[0], ctx[1], 1, (at<uint32_t>(rec, 0xdc) >> 8) + 1 + at<int>((void*)(intptr_t)ctx[0], 0xa8), 0, 0, 0);
    }
    return 0;
}

// 1.13c D2Game+0x6DBF0  (caller 0x408D0; REG? eax = ctx)
void D2GAME_6DBF0(int* ctx)
{
    if (ctx[1] && at<int>((void*)(intptr_t)ctx[1], 0x10) != 0) return;
    int cls = ctx[4];
    void* rec = Ordinal_10688();
    u8 b[12];
    Ordinal_10141(ctx[1], b);
    FUN_6fc8b570(ctx); FUN_6fc8b570(ctx); FUN_6fc8b570(ctx);
    int v = FUN_6fc2e390();
    if (!v) assertFail(0x594);
    at<int>((void*)(intptr_t)ctx[1], 0xb8) = v;
    FUN_6fc31a70((void*)(intptr_t)ctx[1], 2, &cls, 0, -1, 1);
    FUN_6fc31a70((void*)(intptr_t)ctx[1], 2, &cls, 0, -1, 1);
    v = FUN_6fc2e310();
    if (!v) assertFail(0x599);
    at<int>((void*)(intptr_t)ctx[1], 0xb8) = v;
    FUN_6fc31a70((void*)(intptr_t)ctx[1], 2, &cls, 0, -1, 1);
    FUN_6fc31a70((void*)(intptr_t)ctx[1], 2, &cls, 0, -1, 1);
    if (at<char>(rec, 0x140) == 0) Ordinal_11090((void*)(intptr_t)ctx[1], 2);
    else {
        Ordinal_11090((void*)(intptr_t)ctx[1], 1);
        FUN_6fcaea80(ctx[0], ctx[1], 1, (at<uint32_t>(rec, 0xdc) >> 8) + 1 + at<int>((void*)(intptr_t)ctx[0], 0xa8), 0, 0, 0);
    }
    if (ctx[1]) at<uint32_t>((void*)(intptr_t)ctx[1], 0xc4) &= ~2u;
}
