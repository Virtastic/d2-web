// src/core/d2game/d2game_n127.cpp -- 1.13c D2Game.dll sub_CDBB0 (chat/whisper packet handler),
// transcribed from the DISASSEMBLY (Ghidra decompile failed twice). Base 0x6FC20000.
// Register args explicit (ECX = pGame, EDX = pUnit), stdcall stack args (packet, length), ret 8.
// Callees and the two module globals are seams on D2GameN127Env, called in retail's order. The local frame is
// retail's (0x3A8 bytes past the three saved registers): buf offsets are retail's esp offsets.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2GameN127Env {
    uint32_t (*ord10920)(const void* unit);                              // 0xA4C0 -> D2Common #10920 (player data)
    uint8_t  (*stat)(const void* unit, uint32_t stat, uint32_t layer);   // 0xA42A
    const char* (*strChr)(const char* s, int ch);                        // 0xA28C (Storm 571 SStrChr)
    uint32_t (*strCopy)(char* dst, const char* src, uint32_t n);         // 0xA280 (Storm 501 SStrCopy)
    uint32_t (*strLen)(const char* s);                                   // 0xA292 (Storm 506 SStrLen)
    // 0x29820 GAME_ForEachIngameClient; cb is the retail RVA (0xCCA00 or 0xCD530), ctx the 3-dword block.
    void     (*forEachClient)(void* game, uint32_t cbRva, void* ctx);
    void     (*send5A)(const void* msg, uint32_t client);                // 0x8B400 (EAX = msg, stack client)
    void     (*send26)(const void* msg, uint32_t client);                // 0x8B470 (EAX = msg, stack client)
    uint32_t* found;                                                     // 0x6FD301E0 (whisper refused by the target)
    uint32_t* notFound;                                                  // 0x6FD301E4 (whisper delivered)
};

[[noreturn]] static void n127fatal() { _exit(-1); }   // assert line 0x9D9 -> Fog 10265/10024 -> _exit(-1)

static inline uint32_t n127client(const uint8_t* unit, const D2GameN127Env* env) {
    uint32_t c; std::memcpy(&c, (const uint8_t*)(uintptr_t)env->ord10920(unit) + 0x9c, 4); return c;
}

// 1.13c D2GAME+0xCDBB0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_CDBB0(void* game, const uint8_t* unit,
        const uint8_t* pkt, int32_t len, const D2GameN127Env* env)
{
    uint8_t buf[0x3a8];                      // retail frame: only 0x50..0x162 is zeroed (rep stos 0x44 dwords + word + byte)
    std::memset(buf + 0x50, 0, 0x113);
    uint8_t* hdr = buf + 0x50;
    char* msg  = (char*)buf + 0x53;          // first string (0x100)
    char* name = (char*)buf + 0x153;         // second string (0x10)
    hdr[0] = pkt[0]; hdr[1] = pkt[1]; hdr[2] = pkt[2];
    const char* src = (const char*)pkt + 3;
    env->strCopy(msg, src, 0x100);
    int32_t n1 = (int32_t)env->strLen(src);
    if (n1 >= 0x100 || n1 + 4 >= len) return 2;
    src += n1 + 1;
    env->strCopy(name, src, 0x10);
    env->strLen(src);                        // result discarded

    if (!unit || *(const uint32_t*)unit != 0) n127fatal();
    uint32_t client = n127client(unit, env);
    if (!client) n127fatal();
    const char* sender = (const char*)(uintptr_t)client + 0xd;
    if (env->strLen(sender) > 0) env->strCopy((char*)buf + 0x172, sender, 0x7fffffff); else buf[0x172] = 0;

    int32_t n2 = (int32_t)env->strLen(name);
    bool whisper = n2 > 0 && n2 < 0x10;      // stored at local 0x3c
    buf[0x171] = env->stat(unit, 0xc, 0);
    buf[0x162] = 0;
    if (buf[0x152] || env->strChr(msg, -1)) return 0;

    // outgoing 0x26 packet at 0x168 (0x11a bytes): type, mode, hdr[2], 2, dword 0, 0, level, sender (0x10), text (0x100)
    std::memcpy(buf + 0x182, msg, 0x100);
    uint8_t* out = buf + 0x168;
    out[2] = hdr[2]; out[8] = 0; out[0] = 0x26;
    std::memset(out + 4, 0, 4);
    out[3] = 2;
    out[1] = whisper ? 2 : 1;

    // ctx block at local 0x44 (three retail dwords): outgoing packet, header+strings, sender unit id (-1 when not a player unit).
    uint32_t c[3] = { (uint32_t)(uintptr_t)out, (uint32_t)(uintptr_t)hdr, *(const uint32_t*)unit == 0 ? *(const uint32_t*)(unit + 0xc) : 0xffffffffu };
    *env->notFound = 0; *env->found = 0;
    env->forEachClient(game, whisper ? 0xCCA00 : 0xCD530, c);
    if (out[1] != 2) return 0;

    if (*env->found || !*env->notFound) {    // M: 0x5A system message, 0x28 bytes at 0x14
        uint8_t* m = buf + 0x14;
        std::memset(m, 0, 0x28);
        m[0] = 0x5a;
        env->strCopy((char*)m + 8, name, 0x7fffffff);
        m[1] = *env->found ? 0x0d : 0x04;
        m[0x17] = 0;
        env->send5A(m, n127client(unit, env));
        return 0;
    }
    uint8_t* nn = buf + 0x288;               // N: 0x26 mode 6 echo to the sender with the target name; byte 9 is never written
    nn[3] = 2; nn[0] = 0x26; nn[1] = 6;
    std::memset(nn + 4, 0, 4); nn[8] = 0;
    env->strCopy((char*)nn + 0xa, name, 0x7fffffff);
    nn[0x19] = 0; nn[2] = 0;
    std::memcpy(nn + 0x1a, msg, 0x100);
    env->send26(nn, n127client(unit, env));
    return 0;
}
