// Six 1.13c preset-selection handlers. Candidate records are retail data supplied by the
// adapter; the retail x86 state image retains 32-bit pointer tokens at its original offsets.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

struct D2CommonPreset11430Env {
    // First record is retail DAT_6fde6478; each candidate has a 0x10-byte stride.
    const uint8_t* candidates;
    void* (*resolveRetailAddress)(uint32_t token);
    // Adapter for 0x106B0, including its ECX clobber and index mutation.
    uintptr_t (*choose)(uintptr_t ecx, const uint8_t* record, void* state,
                        uint32_t* index);
    void (*fatal)(uint32_t messageId);
};

namespace {
uint32_t rd(const void* p, uint32_t offset) {
    uint32_t v;
    std::memcpy(&v, static_cast<const uint8_t*>(p) + offset, sizeof(v));
    return v;
}
void wr(void* p, uint32_t offset, uint32_t v) {
    std::memcpy(static_cast<uint8_t*>(p) + offset, &v, sizeof(v));
}
void* ptr(const D2CommonPreset11430Env* env, const void* p, uint32_t offset) {
    const uint32_t token = rd(p, offset);
    return token && env->resolveRetailAddress ? env->resolveRetailAddress(token) : nullptr;
}
uint32_t roll(void* state) {
    const uint64_t v = uint64_t(rd(state, 0x1c4)) * 0x6ac690c5ULL + rd(state, 0x1c8);
    wr(state, 0x1c4, uint32_t(v));
    wr(state, 0x1c8, uint32_t(v >> 32));
    return uint32_t(v) & 3;
}
uintptr_t choose(const D2CommonPreset11430Env* env, uintptr_t ecx, uint32_t table,
                 void* state, uint32_t* index) {
    return env->choose(ecx, env->candidates + table + *index * 0x10, state, index);
}
bool ready(void* state, const D2CommonPreset11430Env* env) {
    return state && env && env->candidates && env->choose;
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11430(void* state, const D2CommonPreset11430Env* env) { // 1.13c D2Common+0x11430
    if (!ready(state, env)) return;
    uint32_t index = roll(state);
    uintptr_t ecx = reinterpret_cast<uintptr_t>(&index);
    if (rd(state, 0x1d0) == 0x40) {
        ecx = env->choose(0x40, env->candidates + 0xc0, state, &index);
        index = 3;
        ecx = env->choose(ecx, env->candidates + 0xb0, state, &index);
    } else {
        ecx = choose(env, ecx, 0x40, state, &index);
    }
    choose(env, ecx, 0, state, &index);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_114c0(void* state, const D2CommonPreset11430Env* env) { // 1.13c D2Common+0x114C0
    if (!ready(state, env)) return;
    void* node = ptr(env, state, 0x10);
    while (node) {
        void* record = ptr(env, node, 0x20);
        if (static_cast<int32_t>(rd(record, 0)) >= 0x1ad) break;
        node = ptr(env, node, 0x24);
    }
    void* record = node ? ptr(env, node, 0x20) : nullptr;
    uint32_t index;
    switch (rd(record, 0)) {
        case 0x1bc: index = 3; break;
        case 0x1bd: index = 1; break;
        case 0x1be: index = 0; break;
        case 0x1bf: index = 2; break;
        default:
            if (env->fatal) env->fatal(0x519);
            std::abort();
    }
    const uint32_t level = rd(state, 0x1d0);
    uintptr_t ecx = rd(record, 0);
    if (level > 0x36 && level < 0x3b) ecx = choose(env, ecx, 0xd0, state, &index);
    if (level == 0x39) ecx = choose(env, reinterpret_cast<uintptr_t>(&index), 0x110, state, &index);
    if (level == 0x3b || level == 0x3d) ecx = choose(env, ecx, 0x150, state, &index);
    void* drlg = ptr(env, state, 0x1b4);
    if (level > 0x41 && level < 0x49 && drlg && level != rd(drlg, 0x94))
        ecx = choose(env, ecx, 0x150, state, &index);
    if (level == 0x3b) ecx = choose(env, reinterpret_cast<uintptr_t>(&index), 0x190, state, &index);
    if (level == 0x3c) ecx = choose(env, ecx, 0x1d0, state, &index);
    if (level == 0x3b || level == 0x3d)
        ecx = choose(env, reinterpret_cast<uintptr_t>(&index), 0x210, state, &index);
    if (drlg && level == rd(drlg, 0x94)) ecx = choose(env, level, 0x250, state, &index);
    if (drlg && level == rd(drlg, 0x484))
        choose(env, reinterpret_cast<uintptr_t>(&index), 0x290, state, &index);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11940(void* state, const D2CommonPreset11430Env* env) { // 1.13c D2Common+0x11940
    if (!ready(state, env)) return;
    uint32_t index = roll(state);
    uintptr_t ecx = choose(env, rd(state, 0x1c8), 0x410, state, &index);
    if (rd(state, 0x1d0) == 0x23) choose(env, ecx, 0x450, state, &index);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_119b0(void* state, const D2CommonPreset11430Env* env) { // 1.13c D2Common+0x119B0
    if (!ready(state, env)) return;
    uint32_t index = roll(state);
    uintptr_t ecx = choose(env, rd(state, 0x1c8), 0x490, state, &index);
    const uint32_t level = rd(state, 0x1d0);
    if (level == 0x1d) ecx = choose(env, ecx, 0x550, state, &index);
    if (level == 0x1e) ecx = choose(env, reinterpret_cast<uintptr_t>(&index), 0x590, state, &index);
    choose(env, ecx, level == 0x1f ? 0x4d0 : 0x510, state, &index);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11a70(void* state, const D2CommonPreset11430Env* env) { // 1.13c D2Common+0x11A70
    if (!ready(state, env)) return;
    uint32_t index = roll(state);
    uintptr_t ecx = choose(env, rd(state, 0x1c8), 0x5d0, state, &index);
    const uint32_t level = rd(state, 0x1d0);
    if (level == 0x12) ecx = choose(env, ecx, 0x610, state, &index);
    if (level == 0x13 || level == 0x85)
        ecx = choose(env, reinterpret_cast<uintptr_t>(&index), 0x690, state, &index);
    if (level > 0x14 && level < 0x19) choose(env, ecx, 0x6d0, state, &index);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_11b30(void* state, const D2CommonPreset11430Env* env) { // 1.13c D2Common+0x11B30
    if (!ready(state, env)) return;
    uint32_t index = roll(state);
    uintptr_t ecx = choose(env, rd(state, 0x1c8), 0x710, state, &index);
    ecx = choose(env, rd(state, 0x1d0) == 8 ? ecx : reinterpret_cast<uintptr_t>(&index),
                 rd(state, 0x1d0) == 8 ? 0x750 : 0x790, state, &index);
    if (rd(state, 0x1d0) == 9) ecx = choose(env, ecx, 0x7d0, state, &index);
    if (rd(state, 0x1d0) == 10) choose(env, ecx, 0x810, state, &index);
}
