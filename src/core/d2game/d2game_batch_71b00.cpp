// src/core/d2game/d2game_batch_71b00.cpp -- 1.13c D2Game.dll 0x71B00, transcribed from the cached Ghidra
// decompile. Batch lane: implementation + marker only. Host model as in d2game_batch_d36d0.cpp: opaque
// uint32 handles, retail byte offsets, register arguments explicit, every callee a seam taking up to six
// uint32 words in retail argument order. Fatal path: Fog #10265 -> #10024 -> _exit(-1).
#include <initializer_list>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void* H(uint32_t h) { return (void*)(uintptr_t)h; }

typedef uint32_t (*Sx)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

struct D2Game71B00Env {
    Sx o10265, o10024, o10920, o10867, o10750, o11090, o10012, o10757, o10508;
    Sx sendPacket;   // 0x8A3E0 (buf, size)
    Sx sub107E0;     // 6fc307e0(unit)
    Sx subD76A0, subD5080, subDFC10, subE20A0, sub719A0, sub6FFC0, sub70B70;
};

template <class... A> static inline uint32_t call(Sx f, A... a) {
    uint32_t v[6] = {0, 0, 0, 0, 0, 0}; uint32_t t[] = {(uint32_t)a...}; for (size_t i = 0; i < sizeof...(A); ++i) v[i] = t[i];
    return f(v[0], v[1], v[2], v[3], v[4], v[5]);
}

// 1.13c D2Game+0x71B00
// this = game (ECX), unit = in_EAX (the acting unit, reg), rec = param_1 (pickup record: +0x64 id,
// +0x68 flag byte, +0x6C/+0x70 flag word). Packet buffers on the stack are built by code the decompiler
// lost (stack0xffffffdc); ptkBuf is the caller-visible 6-byte scratch passed through.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_71b00(uint32_t game, uint32_t unit, uint32_t rec,
                                                         uint32_t pktBuf, uint32_t ebp, const D2Game71B00Env* env)
{
    auto fail = [&]() {                      // short 0xC77 error packet, size 2
        if (rd32(H(rec), 0) == 0) call(env->o10920, rec);
        uint16_t pkt[2] = {0x0c77, 0};
        call(env->sendPacket, (uint32_t)(uintptr_t)pkt, 2);
    };
    if ((uint8_t)rd32(H(rec), 0x6c) != 0 && rd32(H(rec), 0x68) != 0) { fail(); return; }
    if (game == 0) { call(env->o10265); call(env->o10024); _exit(-1); }
    uint32_t it = rd32(H(game), 0x1120 + (rd32(H(rec), 0x64) & 0x7f) * 4);
    for (;;) {
        if (it == 0) { fail(); return; }
        if (rd32(H(it), 0xc) == rd32(H(rec), 0x64)) break;
        it = rd32(H(it), 0xe4);              // piVar1[0x39]
    }
    uint32_t piVar2 = call(env->sub107E0, unit);
    int32_t t = (int32_t)rd32(H(unit), 0);
    if (t != 2 && (t < 4 || t > 5) && rd32(H(unit), 0x2c) != 0) call(env->o10867);
    t = (int32_t)rd32(H(unit), 0);
    uint32_t stk;                            // piStack_c
    if (t == 2 || (t > 3 && t < 6)) stk = rd32(H(rd32(H(unit), 0x2c)), 0x10);
    else if (rd32(H(unit), 0x2c) == 0) stk = 0;
    else stk = call(env->o10750);
    call(env->o11090);
    call(env->o10012, piVar2, 1);
    uint32_t id = piVar2 == 0 ? 0xffffffffu : rd32(H(piVar2), 0xc);
    if (call(env->subD76A0, game, it, id) != 0) {
        if (rd32(H(unit), 0) == 4 && call(env->o10757) != 0 && rd32(H(unit), 0x60) != 0 &&
            call(env->o10508, rd32(H(unit), 0x60)) != 0)
            call(env->subD5080, game);
        call(env->sub6FFC0);
        uint32_t a = call(env->o10920, stk), b = call(env->o10920, it);
        if (rd32(H(a), 0x50) == 4 || rd32(H(b), 0x50) == 4) {
            uint8_t pkt[0x24] = {0};         // four size-6 packets, contents lost to the decompiler
            for (uint32_t s : {stk, stk, it, it}) {
                if (rd32(H(s), 0) == 0) call(env->o10920, s);
                call(env->sendPacket, (uint32_t)(uintptr_t)pkt, 6);
            }
            call(env->sub70B70, ebp, ebp, b);
        }
        return;
    }
    call(env->subDFC10, piVar2);
    call(env->subE20A0, game, piVar2);
    call(env->o10920, stk);
    call(env->o10920, it);
    call(env->sub719A0, it);
    (void)pktBuf;
}
