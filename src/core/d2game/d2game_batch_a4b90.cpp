// Alpha batch D2Game+0xA4B90..0xA6530 (monster mode handlers). Written from the cached 1.13c decompile;
// no test runner run. Retail field offsets verbatim, pointer fields read at host width. REG? = input the decompiler
// could not attribute (register/stack), passed explicitly. Packet frames are zeroed 8-dword stack blocks;
// callee seams are variadic extern "C" (same convention as d2game_batch_6ae60.cpp).
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define ORD(n, ret, ...) namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("Ordinal_" #n))) ret Ordinal_##n(__VA_ARGS__); } using namespace ordns_a4b90
// shared with d2game_batch_6ae60.cpp: identical declarations
ORD(10346, void, ...);
ORD(10867, int, ...);
ORD(10750, int, ...);
ORD(11103, void, ...);
ORD(10141, void, ...);
ORD(10302, void, ...);
ORD(10630, void*, ...);
ORD(10243, void, ...);
ORD(10494, int, ...);
ORD(10331, void*, ...);
ORD(10265, int, ...);
ORD(10024, void, ...);
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc95e10"))) void FUN_6fc95e10(...); } using namespace ordns_a4b90;   // send packet frame
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21320"))) void* FUN_6fc21320(...); } using namespace ordns_a4b90;  // unit -> monstats-like record
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fd12380"))) void FUN_6fd12380(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fd122c0"))) void FUN_6fd122c0(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fd12220"))) void FUN_6fd12220(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fd12540"))) void FUN_6fd12540(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fd120c0"))) void FUN_6fd120c0(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21130"))) void FUN_6fc21130(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21100"))) void FUN_6fc21100(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc766d0"))) int FUN_6fc766d0(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc21180"))) uint32_t FUN_6fc21180(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc41490"))) void FUN_6fc41490(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc412f0"))) void FUN_6fc412f0(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fc416a0"))) void FUN_6fc416a0(...); } using namespace ordns_a4b90;
namespace ordns_a4b90 { __attribute__((weak, import_module("env"), import_name("FUN_6fcbd230"))) void FUN_6fcbd230(...); } using namespace ordns_a4b90;

typedef uint8_t u8;
template <class T> static inline T& at(const void* p, int off) { return *(T*)((u8*)p + off); }
static inline void* pat(const void* p, int off) { return at<void*>(p, off); }
static inline bool isPathUnit(int t) { return t == 2 || (t > 3 && t < 6); }
[[noreturn]] static void assertFail(int line) { Ordinal_10024("D2Game.dll", Ordinal_10265(line)); exit(-1); }
// 0xA4B90 etc: LCG step on a unit's 64-bit seed (lo*0x6AC690C5 + hi), returns the new low dword
static inline uint32_t seedStep(void* seed) {
    uint64_t s = (uint64_t)at<uint32_t>(seed, 0) * 0x6ac690c5ull + at<uint32_t>(seed, 4);
    at<uint64_t>(seed, 0) = s;
    return (uint32_t)s;
}
// bit i of the interact-mask table gdwBitMasks[i] is 1<<i
static inline bool recFlag(void* unit, int off, int bit) {
    u8* r = (u8*)FUN_6fc21320(unit);
    return r && (r[off] & (1u << bit));
}

// 1.13c D2Game+0xA4B90  (ecx=game, edx=unit, stack ctx**)
void D2GAME_A4B90(void* game, void* unit, int** ctx)
{
    uint32_t pkt[8] = {};
    if (at<int>(*ctx, 0x18) == 1) {
        if (unit) at<uint32_t>(unit, 0xc4) |= 0x20000;
        Ordinal_11103(unit, 0);
        pkt[1] = (uint32_t)(uintptr_t)unit;
        pkt[5] = 0x6400;
        FUN_6fc95e10(game, pkt);
        return;
    }
    int t = at<int>(unit, 0);
    void* d = pat(unit, 0x2c);
    int a, b;
    a = isPathUnit(t) ? at<int>(d, 0x10) : (d ? Ordinal_10750(d) : 0);
    b = isPathUnit(t) ? at<int>(d, 0xc) : (d ? Ordinal_10867(d) : 0);
    Ordinal_11103(unit, 0);
    u8 pct = recFlag(unit, 0x104, 4) ? 0x65 : 100;
    pkt[0] = 4;
    pkt[1] = (uint32_t)(uintptr_t)unit;
    pkt[3] = b - 10;
    pkt[4] = a;
    pkt[5] = (uint32_t)pct << 8;
    FUN_6fc95e10(game, pkt);
    at<int>(*ctx, 0x18) = 1;  // REG? iStack_4 is the ctx pointer
}

// 1.13c D2Game+0xA4E40  (caller 0xA8430; REG? esi=unit, edi=ctx**)
void D2GAME_A4E40(int* unit, int** ctx)
{
    Ordinal_10302();
    void* g = Ordinal_10630();
    if (!g) assertFail(0x1702);
    Ordinal_10243(g, 0x11e);
    if (!unit) assertFail(0xa2a);
    int t = unit[0];
    void* d = pat(unit, 0x2c);
    void* owner = pat(unit, 0x80);
    if (t != 2 && (t < 4 || t > 5) && d) Ordinal_10750(d);
    if (t != 2 && (t < 4 || t > 5) && d) Ordinal_10867(d);
    FUN_6fd12380(unit);
    at<int>(*ctx, 0x14) += 1;
    at<int>(*ctx, 0x1c) = at<int>(owner, 0xa8) + 100;
    at<uint32_t>(*ctx, 0x18) |= 2;
    at<uint32_t>(*ctx, 0x18) &= 0xfffffffe;
}

// 1.13c D2Game+0xA4F40  (ecx=game, edx=unit)
void D2GAME_A4F40(void* game, void* unit)
{
    void* list = pat(game, 0x10f4);
    if (list) {
        for (void* n = pat(list, 0); n; n = pat(n, 0xf4)) {
            if (at<int>(n, 0) == 0x23) {
                if (at<char>(pat(n, 0x18), 0x11) != 0 && FUN_6fc766d0() == 0 && Ordinal_10494(unit, 0x92) == 0) {
                    Ordinal_10346(Ordinal_10331(unit, 0));
                    FUN_6fc21130(unit);
                    FUN_6fc21100(unit);
                    FUN_6fd12380(unit);
                    return;
                }
                break;
            }
        }
    }
    FUN_6fd122c0(0x19);
}

// 1.13c D2Game+0xA5580  (ecx=game, edx=unit, stack ctx; REG? dist=iStack_4 from Ordinal_10141,
// ebx/esi/ebp = coordinates the second Ordinal_10141 leaves in registers)
void D2GAME_A5580(void* game, void* unit, void* ctx, int dist, uint32_t ebx, uint32_t esi, uint32_t ebp)
{
    int target = at<int>(ctx, 8);
    void* st = pat(ctx, 0);
    uint32_t lim = at<uint32_t>(ctx, 0x18);
    uint32_t tmp[12] = {}, pkt[8] = {};
    if (unit) at<uint32_t>(unit, 0xc4) |= 0x20000;
    Ordinal_10141(unit, tmp);
    pkt[1] = (uint32_t)(uintptr_t)unit;
    if (target == 0 || 4 < dist) {
        if (at<int>(st, 0x14) != 0) {
            Ordinal_11103(unit, 0);
            pkt[0] = 8;
            pkt[2] = target;
            pkt[5] = (uint32_t)((unit && recFlag(unit, 0x105, 0)) ? 0x65 : 100) << 8;
            FUN_6fc95e10(game, pkt);
            return;
        }
        FUN_6fd122c0(0xf);
        return;
    }
    if (at<int>(st, 0x14) == 0) {
        at<int>(st, 0x14) = 1;
        at<int>(st, 0x18) = at<int>(game, 0xa8);
        Ordinal_11103(unit, 0);
        pkt[0] = 9;
        pkt[2] = target;
        pkt[5] = (uint32_t)((unit && recFlag(unit, 0x105, 1)) ? 0x65 : 100) << 8;
        FUN_6fc95e10(game, pkt);
        return;
    }
    if (ebp == 0 || at<int>(game, 0xa8) <= at<int>(st, 0x18)) {
        FUN_6fd12220(game, target);
        return;
    }
    Ordinal_10141(target, tmp);
    if (ebx <= esi) {
        if (ebp <= lim) {
            FUN_6fd12220(game, target);
            at<int>(st, 0x18) = at<int>(game, 0xa8) + 0x23;
            return;
        }
        if (ebx < esi) goto L_far;
    }
    if (lim <= ebp) {
        FUN_6fd12220(game, target);
        at<int>(st, 0x18) = at<int>(game, 0xa8) + 0x23;
        return;
    }
L_far:
    FUN_6fd12220(game, target);
    at<int>(st, 0x18) = at<int>(game, 0xa8) + 5;
}

// 1.13c D2Game+0xA58D0  (ecx=game, edx=unit; REG? packet frame is stack garbage in the decompile, zeroed here)
void D2GAME_A58D0(void* game, void* unit)
{
    if (seedStep((u8*)unit + 0x20) % 100 < 0x14) {
        uint32_t pkt[8] = {};
        Ordinal_11103(unit, 0);
        if (unit) FUN_6fc21320(unit);
        FUN_6fc95e10(game, pkt);
        return;
    }
    FUN_6fd122c0(0x1e);
}

// 1.13c D2Game+0xA5C80  (ecx=game, edx=unit, stack ctx)
void D2GAME_A5C80(void* game, int* unit, void* ctx)
{
    int iVar1 = at<int>(ctx, 0x14);
    void* a = nullptr; void* b = nullptr;
    bool isU1 = unit && unit[0] == 1;
    void* inner = isU1 ? pat(unit, 0x14) : nullptr;
    if (inner) { a = pat(inner, 0x28); if (a) b = pat(a, 0x20); }
    if (!b || at<int>(b, 8) != 8) {
        void* ref = nullptr;
        FUN_6fc41490(&ref, 0);
        int t = inner ? at<int>(inner, 0x54) : 0;
        if ((!inner || (t != 3 && t != 0x13)) && ref == nullptr && 0x13 < iVar1) goto L_done;
        uint32_t buf[4] = {};
        buf[3] = 8;
        FUN_6fc412f0(game, buf);
    }
    FUN_6fc416a0();
    FUN_6fcbd230(game, unit);
L_done:
    FUN_6fd122c0(10);
}

// 1.13c D2Game+0xA5D70  (ecx=game, edx=unit, stack ctx)
void D2GAME_A5D70(void* game, void* unit, void* ctx)
{
    int target = at<int>(ctx, 8);
    void* rec = pat(ctx, 0x1c);
    int d = at<u8>(game, 0x6d) * 2;
    if (target == 0 || at<int>(ctx, 0x18) == 0) {
        if ((int)(seedStep((u8*)unit + 0x20) % 100) < at<int16_t>(rec, 0x5c + d))
            FUN_6fd12540(game, unit, target, 2, 0, 0, 1);
    } else {
        if ((int)(seedStep((u8*)unit + 0x20) % 100) < at<int16_t>(rec, 0x56 + d)) {
            if (at<int16_t>(rec, 0x170) >= 0) {
                uint32_t r = FUN_6fc21180((u8*)unit + 0x20);
                if ((int)r < at<int16_t>(rec, 0x62 + d)) { FUN_6fd12380(unit); goto L_end; }
            }
            FUN_6fd12220(game, target);
        } else {
            FUN_6fd122c0((int)at<int16_t>(rec, 0x62 + d));
        }
    }
L_end:
    FUN_6fd120c0((int)at<int16_t>(rec, 0x68 + d));
}

// 1.13c D2Game+0xA6530  (ecx=game, edx=unit, stack ctx; REG? packet frame zeroed)
void D2GAME_A6530(void* game, void* unit, void* ctx)
{
    int target = at<int>(ctx, 8);
    void* rec = pat(ctx, 0x1c);
    int d = at<u8>(game, 0x6d) * 2;
    if (at<int>(ctx, 0x18) == 0) {
        if (at<int16_t>(rec, 0x170) >= 0 && at<int>(ctx, 0x14) < at<int16_t>(rec, 0x62 + d) &&
            (int)(seedStep((u8*)unit + 0x20) % 100) < at<int16_t>(rec, 0x68 + d)) {
            FUN_6fd12380(unit);
            return;
        }
        if ((int)(seedStep((u8*)unit + 0x20) % 100) < at<int16_t>(rec, 0x5c + d)) {
            FUN_6fd12540(game, unit, target, 2, 0, 0, 1);
            return;
        }
    } else if ((int)(seedStep((u8*)unit + 0x20) % 100) < at<int16_t>(rec, 0x56 + d)) {
        uint32_t pkt[8] = {};
        Ordinal_11103(unit, 0);
        if (unit) FUN_6fc21320(unit);
        FUN_6fc95e10(game, pkt);
        return;
    }
    FUN_6fd122c0(0xf);
}
