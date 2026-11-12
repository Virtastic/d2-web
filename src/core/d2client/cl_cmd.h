// Shared rules of the D2Client key-command handlers (the handlers the key-binding table points at). Each handler is
// void(void) in retail, but the value it leaves in EAX is modelled too, so a vector can compare it. Module globals are
// read through the Env at their retail addresses (base 0x6FAB0000 + RVA); callees go through Env::H with the registers
// and stack words the retail call site passes.
#pragma once
#include "cl_env.h"
namespace clc {
constexpr uint32_t kBase = 0x6FAB0000u;
inline uint32_t G(const cl::Env& e, uint32_t rva) { return e.R(kBase + rva); }
inline void S(const cl::Env& e, uint32_t rva, uint32_t v) { e.W(kBase + rva, v); }
constexpr uint32_t kBusy = 0x11C3A4;      // nonzero while input is held off
constexpr uint32_t kPlayer = 0x11BBFC;    // the local player unit
constexpr uint32_t kDeadMode = 0x11;      // player mode: dead

// The common gate: input held off -> that flag; no player -> 0; a dead player -> the player pointer. True when the
// handler goes on (eax then holds the player pointer, as retail's EAX does).
inline bool gate(const cl::Env& e, uint32_t& eax) {
    eax = G(e, kBusy);
    if (eax) return false;
    eax = G(e, kPlayer);
    if (!eax) return false;
    return e.R(eax + 0x10) != kDeadMode;
}
// gate, then one callee with a single stack word
inline uint32_t gated(const cl::Env& e, uint32_t callee, uint32_t arg) {
    uint32_t eax;
    if (!gate(e, eax)) return eax;
    return e.H(callee, arg);
}
// a three-byte message {0x3F, word n} through the send routine (EBX = length, stack word 0 = the bytes)
inline uint32_t send3f(const cl::Env& e, uint32_t n) {
    const uint8_t b[4] = {0x3f, (uint8_t)n, (uint8_t)(n >> 8), 0};
    return e.H(0x143e0, (uint32_t)(uintptr_t)b, 0, 0, 0, 0, 0, 0, 0, 3, 0);
}
// skill hotkey n: with the assign panel up the key binds (stack word n), otherwise it selects (ESI = n).
// needTree: the upper keys also want the skill data present.
inline uint32_t hotkey(const cl::Env& e, uint32_t n, bool needTree) {
    uint32_t eax;
    if (!gate(e, eax)) return eax;
    if (needTree && !G(e, 0x119854)) return 0;
    if (G(e, 0xFAD8C)) return e.H(0x3d180, n);
    return e.H(0x3d5b0, 0, 0, 0, 0, 0, 0, 0, 0, 0, n);
}
// belt slot key: only when that slot's flag byte is 1; the shift state (GetKeyState(0x10), through its import slot)
// rides in EAX masked to the down bit, ECX = the player's inventory, EDX = the player, stack word 0 = the slot.
inline uint32_t belt(const cl::Env& e, uint32_t slot, uint32_t flagRva) {
    const uint32_t a = G(e, kBusy);
    if (a) return a;
    const uint32_t pl = G(e, kPlayer);
    if (!pl || e.R(pl + 0x10) == kDeadMode || e.R8(kBase + flagRva) != 1) return 0;
    const uint32_t ks = e.H(0xCF104, 0x10);
    return e.H(0x29990, slot, 0, 0, 0, 0, ks & 0x8000u, e.R(pl + 0x60), pl);
}
// flip a config flag and store it under its key name (4 stack words: section, key, 0, the new value)
inline uint32_t cfgFlip(const cl::Env& e, uint32_t flagRva, uint32_t key, uint32_t& v) {
    v = G(e, flagRva) == 0;
    return e.H(0xbef6, 0x6FB83588u, key, 0, v);
}
}  // namespace clc
