// src/core/d2client/d2client_batch_1510.cpp -- UNMAPPED 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is
// retail's. Unrecovered callees are host seams (the src/core/d2client/cursor_init_166c0.cpp pattern):
// the function takes a D2ClientBatch1510Env AFTER its retail arguments, which keep their order.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// sgptDataTables fields both 0x1510 and 0x1C10 read: monstats at +0xA78 (stride 0x1A8) with count
// +0xA80, and the monstats2 record it indexes at +0xA90 (stride 0x134) with count +0xA98. Each row
// keeps its own copy so FUNCTION replay can pin them independently. gdwBitMasks[0..7] is the fixed
// array 1,2,4,8,16,32,64,128, written here as 1u << (bit & 7).
namespace {
uint32_t g_mon1510, g_mon2_1510;   // table bases
int32_t  g_monCount1510, g_mon2Count1510;
uint32_t g_mon1c10, g_mon2_1c10;
int32_t  g_monCount1c10, g_mon2Count1c10;
}

// D2Client+0x0BE6C (Ordinal_403) is the debug operator delete(p, file, line, kind). 0x67CC0, 0x67940
// and 0x66C40 are the three base destructors the deleting destructors below call; 0x21D0, 0x2320,
// 0x2360, 0x2540, 0x2580 and 0x1E80 are the unrecovered tear-down callees of the 0x1F70..0x2130 group.
struct D2ClientBatch1510Env {
    void (*opDelete)(void* p, const char* file, int32_t line, int32_t kind);   // 0x0BE6C
    void (*destroyBase)(void* obj);                                            // 0x67CC0 / 0x66C40
    void (*destroyDerived)(void* obj);                                         // 0x67940
    void (*unicodeDefaultCtor)(uint16_t* p);                                   // D2Lang Unicode::Unicode()
    void (*sub21D0)(void* obj);                                                // 0x21D0
    void (*sub2320)(void* obj);                                                // 0x2320 (object arrives in EDI)
    void (*sub2360)(void* obj, int32_t arg);                                   // 0x2360 (object arrives in EBX)
    void (*sub2540)(void* obj);                                                // 0x2540 (object arrives in EAX)
    void (*sub2580)(void* p);                                                  // 0x2580
    void (*sub1E80)(void* obj);                                                // 0x1E80
};

// 0x2050 is another row of this batch (declared here because 0x1FB0 calls it).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2050(void* obj, const D2ClientBatch1510Env* env);

// 1.13c D2Client+0x01510 (d2client_batch_1510.cpp:d2_client_1510): EAX = monster class, EDX = bit. Reads MonStatsTxt[class].wMonStatsEx (+0x18,
// signed), range-checks it against the monstats2 count, then returns bit `bit` of the byte at record+4
// (MonStats2Txt nFlags/dwFlags). ECX (the decompile's param_1) is never read. Any failed check -> 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1510(int32_t nClass, uint32_t nBit) {
    if (nClass < 0 || nClass >= g_monCount1510) return 0;
    if (!g_mon1510) return 0;   // retail dereferences the table pointer; pinned replay always sets it
    const int16_t nStats2 = *reinterpret_cast<const int16_t*>(
        static_cast<uintptr_t>(g_mon1510 + static_cast<uint32_t>(nClass) * 0x1A8u + 0x18u));
    if (nStats2 < 0 || nStats2 >= g_mon2Count1510) return 0;
    const uint32_t rec = g_mon2_1510 + static_cast<uint32_t>(nStats2) * 0x134u;
    if (!rec) return 0;
    const uint8_t flags = *reinterpret_cast<const uint8_t*>(
        static_cast<uintptr_t>(rec + 4u + static_cast<uint32_t>((static_cast<int32_t>(nBit)) >> 3)));
    return (1u << (nBit & 7u)) & flags;
}

// FUNCTION replay pins the two sgptDataTables fields 0x1510 reads.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1510_pin(uint32_t monStats, int32_t monStatsCount,
                                             uint32_t monStats2, int32_t monStats2Count) {
    g_mon1510 = monStats; g_monCount1510 = monStatsCount;
    g_mon2_1510 = monStats2; g_mon2Count1510 = monStats2Count;
}

// 1.13c D2Client+0x01C10 d2client_batch_1510.cpp:d2_client_1c10 -- identical lookup to 0x1510 against the byte at record+0xF0
// (MonStats2Txt nModeFlags/dwModeFlags). EAX = monster class, EDX = bit, ECX unused.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1c10(int32_t nClass, uint32_t nBit) {
    if (nClass < 0 || nClass >= g_monCount1c10) return 0;
    if (!g_mon1c10) return 0;
    const int16_t nStats2 = *reinterpret_cast<const int16_t*>(
        static_cast<uintptr_t>(g_mon1c10 + static_cast<uint32_t>(nClass) * 0x1A8u + 0x18u));
    if (nStats2 < 0 || nStats2 >= g_mon2Count1c10) return 0;
    const uint32_t rec = g_mon2_1c10 + static_cast<uint32_t>(nStats2) * 0x134u;
    if (!rec) return 0;
    const uint8_t flags = *reinterpret_cast<const uint8_t*>(
        static_cast<uintptr_t>(rec + 0xF0u + static_cast<uint32_t>((static_cast<int32_t>(nBit)) >> 3)));
    return (1u << (nBit & 7u)) & flags;
}

// FUNCTION replay pins the two sgptDataTables fields 0x1C10 reads.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1c10_pin(uint32_t monStats, int32_t monStatsCount,
                                             uint32_t monStats2, int32_t monStats2Count) {
    g_mon1c10 = monStats; g_monCount1c10 = monStatsCount;
    g_mon2_1c10 = monStats2; g_mon2Count1c10 = monStats2Count;
}

// 1.13c D2Client+0x01C80: scalar deleting destructor -- 0x67CC0(this), then for flag & 1 free `this`
// through operator delete(this, "delete", -1, 0). Returns `this` in EAX either way.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_1c80(void* obj, uint32_t nFlags, const D2ClientBatch1510Env* env) {
    env->destroyBase(obj);
    if ((nFlags & 1) && obj) env->opDelete(obj, "delete", -1, 0);
    return obj;
}

// 1.13c D2Client+0x01CB0: default-construct six consecutive runs of 0x400 Unicode objects (2 bytes
// each; the loop steps 2 elements = 4 bytes). Runs start at elements 0, 0x800, 0x1000, 0x1800, 0x2000,
// 0x2800 -- i.e. byte offsets 0x0000 through 0x5000, 0x6000 bytes in all. Returns the base pointer.
extern "C"
EMSCRIPTEN_KEEPALIVE uint16_t* d2_client_1cb0(uint16_t* p, const D2ClientBatch1510Env* env) {
    for (uint32_t nBlock = 0; nBlock < 6; nBlock++)
        for (uint32_t i = 0; i < 0x400; i++)
            env->unicodeDefaultCtor(p + nBlock * 0x800u + i * 2u);
    return p;
}

// 1.13c D2Client+0x01D80: scalar deleting destructor -- 0x67940(this), then the same conditional free.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_1d80(void* obj, uint32_t nFlags, const D2ClientBatch1510Env* env) {
    env->destroyDerived(obj);
    if ((nFlags & 1) && obj) env->opDelete(obj, "delete", -1, 0);
    return obj;
}

// 1.13c D2Client+0x01DB0: scalar deleting destructor -- 0x66C40(this), then the same conditional free.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_1db0(void* obj, uint32_t nFlags, const D2ClientBatch1510Env* env) {
    env->destroyBase(obj);
    if ((nFlags & 1) && obj) env->opDelete(obj, "delete", -1, 0);
    return obj;
}

// 1.13c D2Client+0x01F70: release destructor -- 0x21D0(this), then operator delete(this, file, -2, 0)
// with no flag test. The debug file literal is the one at D2Client+0xF9F48 (Ghidra
// s___AUSGAMEDATA___6fba9f48); its bytes are not in the decompile cache, so the retail address is passed.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1f70(void* obj, const D2ClientBatch1510Env* env) {
    env->sub21D0(obj);
    env->opDelete(obj, reinterpret_cast<const char*>(static_cast<uintptr_t>(0xF9F48u)), -2, 0);
}

// 1.13c D2Client+0x01FB0: destructor -- vtable slot 0xD60B0, 0x2320(this), tear down the members at
// this+0x38 and this+0x28 through 0x2580, then the base destructor 0x2050(this).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1fb0(void* obj, const D2ClientBatch1510Env* env) {
    *reinterpret_cast<uint32_t*>(obj) = 0xD60B0u;
    env->sub2320(obj);
    env->sub2580(reinterpret_cast<uint8_t*>(obj) + 0x38);
    env->sub2580(reinterpret_cast<uint8_t*>(obj) + 0x28);
    d2_client_2050(obj, env);
}

// 1.13c D2Client+0x2050 (0x02050): base destructor -- vtable slot 0xD60E0, 0x2360(this, 1), 0x2540(this), then
// tear down the member at this+4 through 0x2580.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2050(void* obj, const D2ClientBatch1510Env* env) {
    *reinterpret_cast<uint32_t*>(obj) = 0xD60E0u;
    env->sub2360(obj, 1);
    env->sub2540(obj);
    env->sub2580(reinterpret_cast<uint8_t*>(obj) + 4);
}

// 1.13c D2Client+0x020D0: state reset -- 0x2360(this, 1); this+0x10 = this+0x24 = 0; 0x2540(this);
// this+0x14 = this+0x18 = this+0x1c = 0. No vtable store, no return value.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20d0(void* obj, const D2ClientBatch1510Env* env) {
    env->sub2360(obj, 1);
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(obj) + 0x10) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(obj) + 0x24) = 0;
    env->sub2540(obj);
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(obj) + 0x14) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(obj) + 0x18) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(obj) + 0x1c) = 0;
}

// 1.13c D2Client+0x02100: scalar deleting destructor -- 0x1E80(this), then for flag & 1 free `this`
// through operator delete(this, "delete", -1, 0). Returns `this`.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2100(void* obj, uint32_t nFlags, const D2ClientBatch1510Env* env) {
    env->sub1E80(obj);
    if ((nFlags & 1) && obj) env->opDelete(obj, "delete", -1, 0);
    return obj;
}

// 1.13c D2Client+0x02130: scalar deleting destructor -- 0x1FB0(this), then the same conditional free.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2130(void* obj, uint32_t nFlags, const D2ClientBatch1510Env* env) {
    d2_client_1fb0(obj, env);
    if ((nFlags & 1) && obj) env->opDelete(obj, "delete", -1, 0);
    return obj;
}
