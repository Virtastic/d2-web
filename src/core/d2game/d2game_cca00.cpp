// src/core/d2game/d2game_cca00.cpp -- 1.13c D2Game.dll 0xCCA00, the whisper callback sub_CDBB0 hands to
// GAME_ForEachIngameClient (0x29820). Transcribed from the disassembly (no function row: only referenced by address).
// ECX = client, EDX = ctx {packet, header block (+0x103 = target name), sender unit id}; plain ret.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2GameCCA00Env {
    int32_t  (*strCmpI)(const char* a, const char* b, uint32_t n);   // 0xA286 (Storm 509 SStrCmpI)
    uint32_t (*sub4AD10)(uint32_t client, uint32_t z);              // 0x4AD10: ESI = client, stack 0, ret 4: the client's player unit
    uint32_t (*subF02A0)(uint32_t flag, uint32_t esi, uint32_t edi); // 0xF02A0: stack flag, ESI/EDI units, ret 4: refuse test
    void     (*send26)(uint32_t msg, uint32_t client);              // 0x8B470 (EAX = msg, stack client)
    uint32_t* found;                                                // 0x6FD301E0
    uint32_t* notFound;                                             // 0x6FD301E4
};

static inline uint32_t ca_rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2GAME+0xCCA00
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_CCA00(uint32_t client, const uint32_t* ctx, const D2GameCCA00Env* env)
{
    if (!client) _exit(-1);                                         // assert 0x996
    const uint32_t pkt = ctx[0], g = ca_rd32(client + 0x1a8);
    if (env->strCmpI((const char*)(uintptr_t)(client + 0xd), (const char*)(uintptr_t)(ctx[1] + 0x103), 0x7fffffff)) return;
    const uint32_t id = ctx[2];
    if (id != 0xffffffffu) {
        if (!g) _exit(-1);                                          // assert 0x2d3
        uint32_t hit = ca_rd32(g + 0x1120 + (id & 0x7f) * 4);
        while (hit && ca_rd32(hit + 0xc) != id) hit = ca_rd32(hit + 0xe4);
        const uint32_t res = env->sub4AD10(client, 0);
        if (hit && res) {
            if (env->subF02A0(4, hit, res) || env->subF02A0(2, res, hit)) { *env->found = 1; return; }
        }
    }
    *env->notFound = 1;
    env->send26(pkt, client);
}
