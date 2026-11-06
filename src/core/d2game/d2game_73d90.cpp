// 1.13c D2Game+0x73D90 PLRTRADE_TryToTrade, transcribed from the disassembly (the Ghidra decompile loses EAX/ECX/ESI).
// Convention: EAX = X (side-A trade holder: [X+0x5C] is its trade block, +0xC the offered gold, +0x50 the state), ECX = Y (the second player), stack arg0 = G (game),
// arg1 = P (the first player), arg2 = Q (side-B trade holder), ret 0xC. Retail keeps the running gold total in P's stack slot and builds the 2-byte packet {0x77, code}
// in G's slot: neither is visible past the call (P and G live in EBP / EBX). Every callee goes through Ops::k (rva, eax, ecx, edx, ebx, esi, stack words s[0..4], s[0] = the
// last pushed); the 0x8A3E0 packet send goes through Ops::send (client, bytes, length). The out word of #10762 (0xB0A2) is a local the callee fills.
// Failure sends: gold total over the receiver's limit -> {0x77,0x0A} to the sender's client first; #10762 fail -> out ? (P:9, Y:A) : (P:A, Y:9). The final #8B340 calls take the
// client in a stack word and the mode byte 0x0D in AL over EAX's previous contents (the client word with its low byte replaced).
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2Game73D90Ops {
    void* ctx;
    uint32_t (*k)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, const uint32_t* s);
    void (*send)(void* ctx, uint32_t client, const uint8_t* buf, uint32_t len);
};

namespace {
inline uint32_t R32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
inline void W32(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
}

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_73D90(uint32_t X, uint32_t Y, uint32_t G, uint32_t P, uint32_t Q, const D2Game73D90Ops* o)
{
    auto K = [&](uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0, uint32_t esi = 0,
                 uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) -> uint32_t {
        const uint32_t s[5] = { s0, s1, s2, s3, s4 };
        return o->k(o->ctx, rva, eax, ecx, edx, ebx, esi, s);
    };
    // the client word of a unit: [#10920 record + 0x9C] for a player unit (type 0), else 0
    auto clientOf = [&](uint32_t u) -> uint32_t { return (u && R32(u, 0) == 0) ? R32(K(0xA4C0, 0, 0, 0, 0, 0, u), 0x9c) : 0; };
    auto send = [&](uint32_t u, uint8_t code) { const uint8_t b[2] = { 0x77, code }; o->send(o->ctx, clientOf(u), b, 2); };
    auto close = [&]() { K(0x73BA0, 0, 0, 0, 0, P, G, X, Q); };   // ESI = P, stack (G, X, Q)
    auto gold = [&](uint32_t u) { return K(0xA42A, 0, 0, 0, 0, 0, u, 0xE, 0); };

    // side A offers gold: P must hold it (signed), Y's total must stay within Y's limit (unsigned)
    uint32_t blk = R32(X, 0x5c);
    if (R32(blk, 0xc) != 0) {
        if ((int32_t)gold(P) < (int32_t)R32(blk, 0xc)) { close(); return; }
        const uint32_t total = gold(Y) + R32(blk, 0xc);
        if (total > K(0xA45A, 0, 0, 0, 0, 0, Y)) { close(); send(P, 0x0a); send(Y, 0x09); return; }
    }
    blk = R32(Q, 0x5c);
    if (R32(blk, 0xc) != 0) {
        if ((int32_t)gold(Y) < (int32_t)R32(blk, 0xc)) { close(); return; }
        const uint32_t total = gold(P) + R32(blk, 0xc);
        if (total > K(0xA45A, 0, 0, 0, 0, 0, P)) { close(); send(P, 0x09); send(Y, 0x0a); return; }
    }

    uint32_t out = 0;
    if (K(0xB0A2, 0, 0, 0, 0, 0, R32(G, 0x1c), P, Y, (uint32_t)(uintptr_t)&out) == 0) {
        W32(R32(X, 0x5c), 0xc, 0);
        W32(R32(Q, 0x5c), 0xc, 0);
        if (out) { send(P, 0x09); send(Y, 0x0a); }
        else     { send(P, 0x0a); send(Y, 0x09); }
        close();
        return;
    }
    if (!K(0x71170, P, Y, 0, 0, 0, G) || !K(0x71170, Y, P, 0, 0, 0, G)) { close(); return; }

    if (P) W32(P, 0xc8, R32(P, 0xc8) | 0x800000);
    if (Y) W32(Y, 0xc8, R32(Y, 0xc8) | 0x800000);
    W32(X, 0x50, 9);
    W32(Q, 0x50, 9);
    K(0x8EA80, 0, G, P, 0, 0, 0xD, R32(G, 0xa8) + 1, 0, 0, 0);   // ECX = G, EDX = P
    K(0x8EA80, 0, G, Y, 0, 0, 0xD, R32(G, 0xa8) + 1, 0, 0, 0);
    if (!(K(0x71E10, Y, 0, 0, 0, 0, G, P) & 0xff)) { close(); return; }
    if (!(K(0x71E10, P, 0, 0, 0, 0, G, Y) & 0xff)) { close(); return; }

    // commit the gold: deduct from the giver when it still holds it (signed), credit the other side (#12DF0: ESI = the receiver, stack (G, amount))
    blk = R32(X, 0x5c);
    int32_t amt = (int32_t)R32(blk, 0xc);
    if (amt) {
        if ((int32_t)gold(P) >= amt) K(0x1670, 0, 0, 0, 0xE, P, (uint32_t)-amt);
        K(0x12DF0, 0, 0, 0, 0, Y, G, R32(blk, 0xc));
    }
    blk = R32(Q, 0x5c);
    amt = (int32_t)R32(blk, 0xc);
    if (amt) {
        if ((int32_t)gold(Y) >= amt) K(0x1670, 0, 0, 0, 0xE, Y, (uint32_t)-amt);
        K(0x12DF0, 0, 0, 0, 0, P, G, R32(blk, 0xc));
    }
    W32(P, 0x68, 6); W32(P, 0x64, 0xffffffffu); *(uint8_t*)(uintptr_t)(P + 0x6c) = 0;
    W32(Y, 0x68, 6); W32(Y, 0x64, 0xffffffffu); *(uint8_t*)(uintptr_t)(Y + 0x6c) = 0;
    K(0x700C0, P);
    K(0x700C0, Y);
    K(0xD4570, P);
    K(0xD4570, Y);
    // #8B340 (0x0D in AL, the client word on the stack); EAX keeps the client's upper 24 bits
    const uint32_t cp = R32(P, 0) == 0 ? R32(K(0xA4C0, 0, 0, 0, 0, 0, P), 0x9c) : 0;
    K(0x8B340, (cp & ~0xffu) | 0xD, 0, 0, 0, 0, cp);
    const uint32_t cy = R32(Y, 0) == 0 ? R32(K(0xA4C0, 0, 0, 0, 0, 0, Y), 0x9c) : 0;
    K(0x8B340, (cy & ~0xffu) | 0xD, 0, 0, 0, 0, cy);
}
