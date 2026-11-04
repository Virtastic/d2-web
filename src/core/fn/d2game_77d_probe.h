// Probe side of d2game_77d_env.h: every D2Game77dEnv callee is a HookEnv recorder that logs the registers and stack words exactly
// as the capture's `hook RVA NST REGS` does (only the registers the descriptor names are passed; a register the descriptor does not
// name is logged 0). A callee loaded with `mov al, imm` (0xF1B10, 0xF2380) is hooked without `a`: the capture has no low-byte record and the
// upper eax bytes are residue, so the al argument is not part of the compared log. Static state because the env struct carries no context pointer. Users: d2game_<rva>.cpp probes of the
// 0x7ED90-0x89C30 AI think ports. Setup: D77::init(lg, sc, gm); build the env with D77::env(); after the port, d2TagLog.
#pragma once
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "../d2game/d2game_77d_env.h"
#include "d2game_probe_tag.h"
namespace D77 {
static HookEnv* E; static uint32_t GM = 0, SKILLDATA = 0, ORD10507 = 0;   // ORD10507: D2Game rva of the Ordinal_10507 import thunk
static void stub() { abort(); }
// 0xE03A0 is hooked `acd`: eax = type, ecx = game, edx = id
static uint32_t eE03A0(uint32_t t, uint32_t id, uint32_t g) { return E->h(0xE03A0, t, g, id); }
static void o11103(uint32_t u, uint32_t z) { E->h(0xA496, 0, 0, 0, 0, 0, u, z); }
static uint32_t e1320(uint32_t c) { return E->h(0x1320, c); }
static uint32_t gF8BE0() { return GM; }
static void e75E10(uint32_t g, const uint8_t* cmd, uint32_t one) { E->h(0x75E10, 0, 0, 0, 0, 0, g, (uint32_t)(uintptr_t)cmd, one); E->blk(0x75E10, (const uint32_t*)cmd, 8, 0x7); }
static void e20EC0(uint32_t u, uint32_t g) { E->h(0x20EC0, u, 0, 0, 0, 0, g); }
static void eF1B10(uint32_t al, uint32_t g, uint32_t u) { E->h(0xF1B10, 0, 0, 0, 0, 0, g, u); }
static void eF2220(uint32_t m, uint32_t u, uint32_t g, uint32_t p) { E->h(0xF2220, m, 0, 0, 0, 0, g, p); E->edi(0xF2220, u); }
static void eF22C0(uint32_t g, uint32_t v) { E->h(0xF22C0, g, 0, 0, 0, 0, v); }
static void eF2380(uint32_t al, uint32_t u, uint32_t g, uint32_t sk, uint32_t p, uint32_t z0, uint32_t z1) { E->h(0xF2380, 0, 0, 0, g, 0, sk, p, z0, z1); }
static uint32_t eF2540(uint32_t g, uint32_t u, uint32_t p, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { return E->h(0xF2540, 0, g, u, 0, 0, p, a, b, c, d, e); }
static uint32_t eF2D80(uint32_t p, uint32_t g, uint32_t u, uint32_t by, uint32_t one) { return E->h(0xF2D80, p, 0, 0, 0, 0, g, u, by, one); }
static uint32_t t718(uint32_t u) { return E->h(0xA718, 0, 0, 0, 0, 0, u); }
static uint32_t t478(uint32_t u) { return E->h(0xA478, 0, 0, 0, 0, 0, u); }
static uint32_t t556(uint32_t a) { return E->h(0xA556, 0, 0, 0, 0, 0, a); }
static uint32_t t56E(uint32_t a) { return E->h(0xA56E, 0, 0, 0, 0, 0, a); }
static uint32_t t4C0(uint32_t p) { return E->h(0xA4C0, 0, 0, 0, 0, 0, p); }
static uint32_t t568(uint32_t v, uint32_t k, uint32_t z) { return E->h(0xA568, 0, 0, 0, 0, 0, v, k, z); }
static uint32_t t3B8(uint32_t s) { return E->h(0xA3B8, 0, 0, 0, 0, 0, s); }
static uint32_t t454(uint32_t s) { return E->h(0xA454, 0, 0, 0, 0, 0, s); }
static uint32_t tAFE8(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { return E->h(0xAFE8, 0, 0, 0, 0, 0, a, b, c, d); }
static uint32_t eF2C10(uint32_t p, uint32_t u, uint32_t g, uint32_t e8, uint32_t one) { return E->h(0xF2C10, p, 0, 0, u, 0, g, e8, one); }
static void eF26B0(uint32_t z, uint32_t u, uint32_t g, uint32_t p, uint32_t f) { E->h(0xF26B0, z, u, 0, 0, 0, g, p, f); }
static int32_t e1900(uint32_t u) { return (int32_t)E->h(0x1900, u); }
// 0xB17B0: eax = mode, ecx = buf (the out struct: dword 0 written by the callee through the hookw `c` key), edx = game, stack unit and the callback's
// absolute address (the capture logs module code as 0xB2000000|rva). hookblk logs buf dword 1 (the radius input; dword 0 is residue).
static uint32_t BLKMASK = 0x2, BLKN = 2;
static void eB17B0m(uint32_t m, uint32_t buf, uint32_t g, uint32_t u, uint32_t cb) { E->h(0xB17B0, m, buf, g, 0, 0, u, 0xB2000000u | cb); E->blk(0xB17B0, (const uint32_t*)(uintptr_t)buf, BLKN, BLKMASK); }
static void eB17B0(uint32_t buf, uint32_t g, uint32_t u, uint32_t cb) { eB17B0m(1, buf, g, u, cb); }
static const D2Game77dEnv& env() {
    static D2Game77dEnv v; v = D2Game77dEnv{};
    v.ordinal11103 = o11103; v.sub1320 = e1320; v.globalF8BE0 = gF8BE0; v.sub75E10 = e75E10; v.sub20EC0 = e20EC0; v.subE03A0 = eE03A0;
    v.subF1B10 = eF1B10; v.subF2220 = eF2220; v.subF22C0 = eF22C0; v.subF2380 = eF2380; v.subF2540 = eF2540; v.subF2D80 = eF2D80;
    v.th2A718 = t718; v.th2A478 = t478; v.th2A556 = t556; v.th2A56E = t56E; v.th2A4C0 = t4C0; v.th2A568 = t568; v.th2A3B8 = t3B8;
    v.th2A454 = t454; v.th2AFE8 = tAFE8; v.subF2C10 = eF2C10; v.subF26B0 = eF26B0; v.sub1900 = e1900; v.subB17B0 = eB17B0; v.subB17B0m = eB17B0m;
    return v;
}
}
