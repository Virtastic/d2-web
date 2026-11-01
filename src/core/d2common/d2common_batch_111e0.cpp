// D2Common implementation batch: 1.13c RVA 0x15980 and five preset-generation handlers.
// Transcribed from cached Ghidra decompiles. Table storage and unmapped callees are explicit seams;
// retail table contents are supplied by the caller and never copied into this source tree.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint8_t* B(void* p) { return static_cast<uint8_t*>(p); }
inline const uint8_t* B(const void* p) { return static_cast<const uint8_t*>(p); }
inline uint8_t R8(const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, B(p) + o, 1); return v; }
inline uint32_t R32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, B(p) + o, 4); return v; }
inline int32_t RI32(const void* p, uint32_t o) { int32_t v; std::memcpy(&v, B(p) + o, 4); return v; }
inline void W32(void* p, uint32_t o, uint32_t v) { std::memcpy(B(p) + o, &v, 4); }
inline void WI32(void* p, uint32_t o, int32_t v) { std::memcpy(B(p) + o, &v, 4); }

inline uint32_t advance(void* state) {
    const uint64_t next = uint64_t(R32(state, 0x1c4)) * 0x6ac690c5ULL + R32(state, 0x1c8);
    const uint32_t low = static_cast<uint32_t>(next);
    W32(state, 0x1c4, low);
    W32(state, 0x1c8, static_cast<uint32_t>(next >> 32));
    return low;
}
}

struct D2CommonBatch15980Env {
    const uint8_t* levelRows; // 0x9c-byte LevelTxt records indexed by level id
    uint32_t levelCount;      // sgptDataTables+0xc5c
    // Retail-layout blobs keep 32-bit pointer tokens at their 1.13c offsets. Resolve each token to
    // the corresponding wasm64 address; never read a native pointer from a retail-layout field.
    void* (*resolveRetailAddress)(uint32_t address);
    void* (*createLevelInfo)(void* pDrlg, int32_t levelId); // 0x2D9B0; returns a writable retail-layout level blob
};

namespace {
inline void* RPtr32(const void* p, uint32_t o, void* (*resolveRetailAddress)(uint32_t)) {
    const uint32_t address = R32(p, o);
    return address && resolveRetailAddress ? resolveRetailAddress(address) : nullptr;
}
}

// Position the five preset levels 0x4f..0x53 below the map. Inputs and level records are writable
// 1.13c x86-layout byte images; pointer fields hold uint32 address tokens resolved by the wasm adapter.
// Existing records are reused by level id; missing records are made by 0x2D9B0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_15980(void* pLevel, const D2CommonBatch15980Env* env) { // 1.13c D2Common+0x15980
    if (!pLevel || !env || !env->levelRows || !env->resolveRetailAddress) return;
    void* pDrlg = RPtr32(pLevel, 0x1b4, env->resolveRetailAddress);
    if (!pDrlg) return;
    const uint32_t direction = R8(pDrlg, 0x450);
    const int32_t mapWidth = RI32(pLevel, 0x24);
    const int32_t originX = RI32(pLevel, 0x1c);
    const int32_t originY = RI32(pLevel, 0x20);
    int32_t y = 0;

    for (int32_t levelId = 0x4f; levelId < 0x54; ++levelId) {
        const uint8_t* row = static_cast<uint32_t>(levelId) < env->levelCount
            ? env->levelRows + static_cast<uint32_t>(levelId) * 0x9c : nullptr;
        const int32_t width = row ? static_cast<int32_t>(R32(row, 0x0c + direction * 4)) : 0;
        const int32_t height = row ? static_cast<int32_t>(R32(row, 0x18 + direction * 4)) : 0;
        y -= height;

        void* pInfo = RPtr32(pDrlg, 0x47c, env->resolveRetailAddress);
        while (pInfo && RI32(pInfo, 0x1d0) != levelId)
            pInfo = RPtr32(pInfo, 0x1ac, env->resolveRetailAddress);
        if (!pInfo && env->createLevelInfo) pInfo = env->createLevelInfo(pDrlg, levelId);
        if (!pInfo) continue;

        WI32(pInfo, 7 * 4, originX + (mapWidth / 2 - width / 2));
        WI32(pInfo, 8 * 4, originY + y);
        WI32(pInfo, 9 * 4, width);
        WI32(pInfo, 10 * 4, height);
    }
}

struct D2CommonBatchPresetEnv {
    const uint8_t* candidates3;  // 3 records, 0x10-byte stride (0x111E0)
    const uint8_t* candidates3b; // 3 records, 0x10-byte stride (0x111E0 mode 0x7b)
    const uint8_t* candidates4;  // contiguous four-record groups at offsets 0x00..0x1c0
    // 0x106B0's ECX input and clobbered ECX are separate from its EAX result. The helper updates
    // pIndex and may mutate state through its other arguments.
    uint32_t (*choose)(uintptr_t ecx, const uint8_t* record, void* state,
                       uint32_t* pIndex, uintptr_t* pClobberedEcx);
    void (*fatal)(uint32_t messageId); // must not return; models 10265/10024 followed by _exit
};

namespace {
inline uint32_t choose(const D2CommonBatchPresetEnv* env, uintptr_t ecx, const uint8_t* row,
                       void* state, uint32_t* index, uintptr_t* clobberedEcx) {
    *clobberedEcx = ecx;
    return env->choose(ecx, row, state, index, clobberedEcx);
}
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_111e0(void* pState, const D2CommonBatchPresetEnv* env) { // 1.13c D2Common+0x111E0
    if (!pState || !env || !env->choose) return 0;
    const uint32_t value = advance(pState);
    const uint32_t lane = value % 3;
    uint32_t index = lane;
    uint32_t result = value / 3;
    uintptr_t ecx = 3;
    const int32_t mode = RI32(pState, 0x1d0);
    if (mode != 0x7c && env->candidates3) {
        uintptr_t nextEcx;
        result = choose(env, ecx, env->candidates3 + lane * 0x10, pState, &index, &nextEcx);
        ecx = nextEcx;
    }
    if (mode == 0x7b) {
        if (index == lane) {
            if (env->fatal) env->fatal(0x5fc);
            std::abort();
        }
        if (env->candidates3b) {
            uintptr_t nextEcx;
            result = choose(env, ecx, env->candidates3b + index * 0x10, pState, &index, &nextEcx);
        }
    }
    return result;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11280(void* pState, const D2CommonBatchPresetEnv* env) { // 1.13c D2Common+0x11280
    if (!pState || !env || !env->choose || !env->candidates4) return;
    uint32_t index = advance(pState) & 3;
    uintptr_t ecx = R32(pState, 0x1c8);
    uintptr_t nextEcx;
    choose(env, ecx, env->candidates4 + index * 0x10, pState, &index, &nextEcx);
    const uintptr_t firstClobberedEcx = nextEcx;
    const int32_t mode = RI32(pState, 0x1d0);
    if (mode == 0x65) {
        choose(env, firstClobberedEcx, env->candidates4 + 0x40 + index * 0x10, pState, &index, &nextEcx);
        choose(env, reinterpret_cast<uintptr_t>(&index), env->candidates4 + 0x80 + index * 0x10,
               pState, &index, &nextEcx);
    }
    if (mode == 100)
        choose(env, firstClobberedEcx, env->candidates4 + 0x80 + index * 0x10, pState, &index, &nextEcx);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11320(void* pState, const D2CommonBatchPresetEnv* env) { // 1.13c D2Common+0x11320
    if (!pState || !env || !env->choose || !env->candidates4) return;
    uint32_t index = advance(pState) & 3;
    uintptr_t clobberedEcx;
    choose(env, R32(pState, 0x1c8), env->candidates4 + 0xc0 + index * 0x10,
           pState, &index, &clobberedEcx);
    uintptr_t nextEcx;
    choose(env, clobberedEcx, env->candidates4 + 0x100 + index * 0x10,
           pState, &index, &nextEcx);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11380(void* pState, const D2CommonBatchPresetEnv* env) { // 1.13c D2Common+0x11380
    if (!pState || !env || !env->choose || !env->candidates4) return;
    uint32_t index = advance(pState) & 3;
    uintptr_t clobberedEcx;
    choose(env, R32(pState, 0x1c8), env->candidates4 + 0x140 + index * 0x10,
           pState, &index, &clobberedEcx);
    uintptr_t nextEcx;
    choose(env, clobberedEcx, env->candidates4 + 0x180 + index * 0x10,
           pState, &index, &nextEcx);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_113e0(void* pState, const D2CommonBatchPresetEnv* env) { // 1.13c D2Common+0x113E0
    if (!pState || !env || !env->choose || !env->candidates4) return;
    uint32_t index = advance(pState) & 3;
    uintptr_t clobberedEcx;
    choose(env, reinterpret_cast<uintptr_t>(pState), env->candidates4 + 0x1c0 + index * 0x10,
           pState, &index, &clobberedEcx);
}
