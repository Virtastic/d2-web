// src/core/d2game/d2game_batch_b61.cpp -- 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Alpha lane: implementation + marker.
// Records are opaque host objects addressed by retail byte offsets; callees are host seams in retail
// argument order. Every malformed-state branch is Fog #10265/#10024 fatal then _exit(-1).
// Not done (register-arg or goto-web): 0x86900 0x8A770 0x8B3D0 0x98250 0x9BCB0 0x9C620 0x9F340
// 0xA39E0 0xA3B50.
#include <initializer_list>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t b61_u32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static inline void b61_w32(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }

struct D2GameB61Env {
    // Fog seams
    void*    (*ord10920)(void* arg);                                  // Fog #10920
    uint32_t (*ord10174)(uint32_t a, uint32_t b);                     // Fog #10174
    void     (*ord10158)(void* unit);                                 // Fog #10158
    void*    (*alloc)(const char* file, uint32_t line, uint32_t zero);// Fog #10045
    void     (*fatal)(uint32_t line);                                 // #10265 + #10024 (caller _exits)
    int      (*isBadCodePtr)(void* fn);                               // kernel32 IsBadCodePtr
    // 0x8A3E0 packet send (buffer, length)
    void     (*sub8a3e0)(const void* buf, uint32_t len);
    // 0x9D3B0 callees
    uint32_t (*sub9cd90)(void* a);                                    // returns low byte bool
    uint32_t (*sub9d160)(void* a);                                    // returns low byte bool
    int      (*sub9cfe0)(void* b, void* a);
    void*    sgptDataTables;                                          // sgptDataTables_exref target
    // 0xBAE40 quest callback table (retail addresses, 32-bit)
    uint32_t cb9560, cb94d0, cba550, cbabf0, cba100, cba5e0, cb9320, cb8ae0, cb8a70, data6118;
};

// 1.13c D2Game+0x9D3B0
// __fastcall(param_1 = ECX, param_2 = EDX unit). Builds a callback context {handler table, 0.., stats
// row, ...} and, if the unit's two data rows exist and none of the three veto checks fire, calls
// the handler at table+4.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_9d3b0(void* param_1, int32_t* param_2, const D2GameB61Env* env)
{
    int32_t ctx[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int32_t local_4;
    const uint8_t* dt; std::memcpy(&dt, &env->sgptDataTables, sizeof dt);
    if (param_2 == nullptr || param_2[0] != 1 || param_2[5] == 0)
        ctx[0] = 0;
    else
        ctx[0] = *(int32_t*)((uint8_t*)(intptr_t)param_2[5] + 0x28);
    int32_t idx = param_2 ? param_2[1] : -1;
    if (param_2 == nullptr || idx < 0 || (int32_t)b61_u32(dt + 0xA80) <= idx)
        ctx[7] = 0;
    else
        ctx[7] = (int32_t)(idx * 0x1A8 + (int32_t)b61_u32(dt + 0xA78));
    int32_t sub;
    if (param_2 == nullptr || idx < 0 || (int32_t)b61_u32(dt + 0xA80) <= idx ||
        (sub = (int16_t)(*(uint16_t*)((uint8_t*)(intptr_t)b61_u32(dt + 0xA78) + 0x18 + idx * 0x1A8)),
         sub < 0 || (int32_t)b61_u32(dt + 0xA98) <= sub))
        local_4 = 0;
    else
        local_4 = sub * 0x134 + (int32_t)b61_u32(dt + 0xA90);
    if (ctx[7] != 0 && local_4 != 0 &&
        (env->sub9cd90(param_1) & 0xFF) == 0 &&
        (env->sub9d160(param_1) & 0xFF) == 0 &&
        env->sub9cfe0(param_2, param_1) == 0) {
        void* fn = (void*)(intptr_t)b61_u32((const uint8_t*)(intptr_t)ctx[0] + 4);
        if (env->isBadCodePtr(fn)) { env->fatal(0x39E); _exit(-1); }
        ((void (*)(int32_t*))fn)(ctx);
    }
}

// 1.13c D2Game+0xB9B00
// __fastcall(param_1 = ECX game/client, param_2 = EDX unit). If Fog #10174 on the table entry for
// the client's slot byte (+0x6D) says yes, sends packet 5D 17 02 00 00 00, then rebinds the unit:
// +0x6E = 0x4B, +0x70 = self, Fog #10158, flags(+0xC4) |= 0x400. Always returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_b9b00(uint8_t* param_1, int32_t* param_2, const D2GameB61Env* env)
{
    uint8_t* tbl = (uint8_t*)env->ord10920(nullptr);
    uint32_t ent = b61_u32(tbl + 0x10 + (uint32_t)param_1[0x6D] * 4);
    if (env->ord10174(ent, 0x1A) != 0) {
        if (param_2 != nullptr && *param_2 == 0) env->ord10920(param_2);
        uint8_t pkt[6] = {0x5D, 0x17, 0x02, 0x00, 0x00, 0x00};
        env->sub8a3e0(pkt, 6);
        if (param_2 == nullptr) { env->fatal(0x558); _exit(-1); }
        *(int16_t*)((uint8_t*)param_2 + 0x6E) = 0x4B;
        param_2[0x1C] = (int32_t)(intptr_t)param_2;
        env->ord10158(param_2);
        param_2[0x31] |= 0x400;
    }
    return 0;
}

// 1.13c D2Game+0xBAE40
// __fastcall(param_1 = ECX quest record). Initialises the Act 4 quest 1 record: clears 0xF dwords at
// +0xA0, sets flags/state bytes, installs the callback table, allocates the 0x1C-byte state (a4q1.cpp
// line 0x2A0) and zeroes it.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_bae40(uint8_t* param_1, const D2GameB61Env* env)
{
    std::memset(param_1 + 0xA0, 0, 0xF * 4);
    param_1[10] = 1;
    param_1[0xC] = 1;
    b61_w32(param_1 + 0xA0, env->cb9560);
    b61_w32(param_1 + 0xDC, env->data6118);
    b61_w32(param_1 + 0xC8, env->cb94d0);
    b61_w32(param_1 + 0xAC, env->cba550);
    b61_w32(param_1 + 0xC0, env->cbabf0);
    b61_w32(param_1 + 0xA8, env->cba100);
    b61_w32(param_1 + 0xCC, env->cba5e0);
    b61_w32(param_1 + 0xD4, env->cb9320);
    b61_w32(param_1 + 0xE0, 0x19);
    param_1[0xD] = 4;
    b61_w32(param_1 + 0xE8, 0);
    b61_w32(param_1 + 0xEC, env->cb8ae0);
    uint8_t* st = (uint8_t*)env->alloc("..\\Source\\D2Game/Quests/a4q1.cpp", 0x2A0, 0);
    b61_w32(param_1 + 0x18, (uint32_t)(uintptr_t)st);
    b61_w32(param_1 + 0xF0, env->cb8a70);
    b61_w32(param_1 + 0x10, 0x18);
    std::memset(st, 0, 7 * 4);
}
