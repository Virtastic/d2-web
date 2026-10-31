// 1.13c D2Game+0x73D90 PLRTRADE_TryToTrade. Written from the 1.13c disassembly.
// Register contract: eax = X (side-A trade holder, its +0x5C record has the offered gold at +0xC), ecx = Y (the
// other unit); stack: G (game), P (unit), Q (side-B trade holder); ret 0xC.
// Register arguments of the callees are passed first, in the order the disassembly loads them
// (FUN_6fc93ba0 also takes P in esi). The Ghidra view of this function loses eax/ecx/esi.
#include <cstdint>

namespace ordns_73d90 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10973"))) int Ordinal_10973(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10049"))) uint32_t Ordinal_10049(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10920"))) uintptr_t Ordinal_10920(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10762"))) int Ordinal_10762(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc93ba0"))) void FUN_6fc93ba0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcaa3e0"))) void FUN_6fcaa3e0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc91170"))) int FUN_6fc91170(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc91e10"))) uint32_t FUN_6fc91e10(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void FUN_6fcaea80(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc21670"))) void FUN_6fc21670(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc32df0"))) void FUN_6fc32df0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc900c0"))) void FUN_6fc900c0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcf4570"))) void FUN_6fcf4570(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcab340"))) void FUN_6fcab340(...);
}

namespace {
using namespace ordns_73d90;
inline uint32_t* w(uintptr_t b, uint32_t o) { return reinterpret_cast<uint32_t*>(b + o); }
// client id of a player unit (+0x9C of its Ordinal_10920 record), 0 for null / non-player.
uint32_t clientId(uintptr_t u) { return (u && *w(u, 0) == 0) ? *w(Ordinal_10920(u), 0x9C) : 0; }
void sendTrade(uintptr_t u, uint8_t code) { uint8_t b[2] = {0x77, code}; FUN_6fcaa3e0(clientId(u), b, 2); }
}

// 1.13c D2Game+0x73D90
extern "C" void D2GAME_PLRTRADE_TryToTrade_73D90(uintptr_t X, uintptr_t Y, uintptr_t G, uintptr_t P, uintptr_t Q)
{
    uint32_t gx = *w(*w(X, 0x5C), 0xC);
    if (gx) {
        if (Ordinal_10973(P, 0xE, 0) < static_cast<int32_t>(gx)) { FUN_6fc93ba0(G, X, Q, P); return; }
        uint32_t total = Ordinal_10973(Y, 0xE, 0) + gx;
        if (total > Ordinal_10049(Y)) {
            FUN_6fc93ba0(G, X, Q, P);
            sendTrade(P, 0xA); sendTrade(Y, 0x9);
            return;
        }
    }
    uint32_t gq = *w(*w(Q, 0x5C), 0xC);
    if (gq) {
        if (Ordinal_10973(Y, 0xE, 0) < static_cast<int32_t>(gq)) { FUN_6fc93ba0(G, X, Q, P); return; }
        uint32_t total = Ordinal_10973(P, 0xE, 0) + gq;
        if (total > Ordinal_10049(P)) {
            FUN_6fc93ba0(G, X, Q, P);
            sendTrade(P, 0x9); sendTrade(Y, 0xA);
            return;
        }
    }

    uint32_t out = 0;
    if (Ordinal_10762(*w(G, 0x1C), P, Y, &out) == 0) {
        *w(*w(X, 0x5C), 0xC) = 0;
        *w(*w(Q, 0x5C), 0xC) = 0;
        if (out) { sendTrade(P, 0x9); sendTrade(Y, 0xA); }
        else     { sendTrade(P, 0xA); sendTrade(Y, 0x9); }
        FUN_6fc93ba0(G, X, Q, P);
        return;
    }
    if (!FUN_6fc91170(P, Y, G) || !FUN_6fc91170(Y, P, G)) { FUN_6fc93ba0(G, X, Q, P); return; }

    if (P) *w(P, 0xC8) |= 0x800000;
    if (Y) *w(Y, 0xC8) |= 0x800000;
    *w(X, 0x50) = 9;
    *w(Q, 0x50) = 9;
    FUN_6fcaea80(G, P, 0xD, *w(G, 0xA8) + 1, 0, 0, 0);
    FUN_6fcaea80(G, Y, 0xD, *w(G, 0xA8) + 1, 0, 0, 0);
    if (!(FUN_6fc91e10(Y, G, P) & 0xFF)) { FUN_6fc93ba0(G, X, Q, P); return; }
    if (!(FUN_6fc91e10(P, G, Y) & 0xFF)) { FUN_6fc93ba0(G, X, Q, P); return; }

    uint32_t ga = *w(*w(X, 0x5C), 0xC);
    if (ga) {
        if (Ordinal_10973(P, 0xE, 0) >= static_cast<int32_t>(ga)) FUN_6fc21670(P, 0xE, -static_cast<int32_t>(ga));
        FUN_6fc32df0(Y, G, *w(*w(X, 0x5C), 0xC));
    }
    uint32_t gb = *w(*w(Q, 0x5C), 0xC);
    if (gb) {
        if (Ordinal_10973(Y, 0xE, 0) >= static_cast<int32_t>(gb)) FUN_6fc21670(Y, 0xE, -static_cast<int32_t>(gb));
        FUN_6fc32df0(P, G, *w(*w(Q, 0x5C), 0xC));
    }
    *w(P, 0x68) = 6; *w(P, 0x64) = 0xFFFFFFFFu; *reinterpret_cast<uint8_t*>(P + 0x6C) = 0;
    *w(Y, 0x68) = 6; *w(Y, 0x64) = 0xFFFFFFFFu; *reinterpret_cast<uint8_t*>(Y + 0x6C) = 0;
    FUN_6fc900c0(P);
    FUN_6fc900c0(Y);
    FUN_6fcf4570(P);
    FUN_6fcf4570(Y);
    FUN_6fcab340(0xD, *w(P, 0) == 0 ? *w(Ordinal_10920(P), 0x9C) : 0u);
    FUN_6fcab340(0xD, *w(Y, 0) == 0 ? *w(Ordinal_10920(Y), 0x9C) : 0u);
}
