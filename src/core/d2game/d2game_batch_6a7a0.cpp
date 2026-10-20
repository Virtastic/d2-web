// Alpha batch D2Game+0x6A7A0, 0x6A960 (interact-end glue). Written from the cached 1.13c decompile;
// no test runner run. ctx = {game, unit, target}; 32-bit offsets.
#include <cstdint>

#define ORD(n, ret, ...) namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("Ordinal_" #n))) ret Ordinal_##n(__VA_ARGS__); } using namespace ordns_6a7a0
ORD(10920, void*, void*);
ORD(11017, int, int);
ORD(11090, void, void*, int);     // UNITS_SetMode
namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("FUN_6fc898e0"))) int FUN_6fc898e0(); } using namespace ordns_6a7a0;
namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("FUN_6fcfffd0"))) void FUN_6fcfffd0(int, void*); } using namespace ordns_6a7a0;
namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("FUN_6fcffae0"))) void FUN_6fcffae0(int, int); } using namespace ordns_6a7a0;
namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("FUN_6fcaab20"))) void FUN_6fcaab20(); } using namespace ordns_6a7a0;
namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("FUN_6fd003a0"))) int FUN_6fd003a0(int, uint32_t); } using namespace ordns_6a7a0;
namespace ordns_6a7a0 { __attribute__((weak, import_module("env"), import_name("FUN_6fcc00b0"))) void FUN_6fcc00b0(); } using namespace ordns_6a7a0;

typedef uint8_t u8;
template <class T> static inline T& at(void* p, int off) { return *(T*)((u8*)p + off); }

// 1.13c D2Game+0x6A7A0  (REG: ecx = ctx)
int D2GAME_6A7A0(int* ctx)
{
    int cur = 0;
    void* p = (void*)ctx[2];
    if (at<u8>(p, 0x6c)) cur = FUN_6fd003a0(ctx[0], at<uint32_t>(p, 100));
    if (p && at<int>(p, 0) == 0) Ordinal_10920(p);
    int tgt = ctx[1];
    if (!tgt || at<int>((void*)tgt, 0x10) == 0) {
        p = (void*)ctx[2];
        if (!p || at<int>(p, 0) != 0 ||
            (at<u8>(p, 0x6c) == 0 && Ordinal_11017(at<int>(p, 0x60)) == 0 &&
             at<int>(Ordinal_10920(p), 0x4c) != 1)) {
            if (!FUN_6fc898e0()) {
                FUN_6fcfffd0(0x13, (void*)ctx[2]);
                return 1;
            }
            FUN_6fcffae0(2, ctx[1] ? at<int>((void*)ctx[1], 0xc) : -1);
            Ordinal_11090((void*)ctx[1], 3);
            FUN_6fcaab20();
        }
    } else if (at<int>((void*)tgt, 0x10) == 3 && cur == tgt) {
        int s = ctx[2];
        at<int>((void*)s, 100) = -1;
        at<int>((void*)s, 0x68) = 6;
        at<u8>((void*)s, 0x6c) = 0;
        Ordinal_11090((void*)ctx[1], 1);
    }
    return 1;
}

// 1.13c D2Game+0x6A960
void D2GAME_6A960() { FUN_6fcc00b0(); }
