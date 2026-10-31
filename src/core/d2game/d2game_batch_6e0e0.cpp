// 1.13c D2Game AI think bodies 0x6DEC0, 0x6E0E0 and 0x6E270. Written from the 1.13c disassembly.
// 0x6DEC0 and 0x6E0E0 take ecx = think context (ctx[0] game, [1] unit, [2] target, [4] monster id).
// 0x6E270 takes ecx = game, edx = unit (register convention), no stack args, no result.
// 0x6E0E0 calls FUN_6fd142d0 (0xF42D0, stdcall 4 args: game, unit, target, mode) twice, with mode 0 and mode 1.
#include <cstdint>

namespace ordns_6e0e0 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) uint32_t Ordinal_10331(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10141"))) void Ordinal_10141(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10589"))) void Ordinal_10589(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10904"))) void Ordinal_10904(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10032"))) void Ordinal_10032(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10408"))) void Ordinal_10408(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10522"))) int Ordinal_10522(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10688"))) uint32_t Ordinal_10688(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_11090"))) void Ordinal_11090(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fd142d0"))) void FUN_6fd142d0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcfffd0"))) void FUN_6fcfffd0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void FUN_6fcaea80(...);
}
extern "C" uint32_t d2web_GetTickCount();

namespace {
using namespace ordns_6e0e0;
inline uint32_t rd32(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
inline uint32_t* w32(uintptr_t b, uint32_t o) { return reinterpret_cast<uint32_t*>(b + o); }
inline uintptr_t P(int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); }
// mode-1 send at (rec+0xDC >> 8) + 1 + game+0xA8 (ecx = game, edx = unit; stack 1, frame, 0, 0, 0).
inline void sendMode1(uintptr_t game, uintptr_t unit, uint32_t rec) {
    FUN_6fcaea80(game, unit, 1, (rd32(rec, 0xDC) >> 8) + 1 + rd32(game, 0xA8), 0, 0, 0);
}
}

// 1.13c D2Game+0x6DEC0 (think table data reference 0x107CAC). Always returns 1.
extern "C" int D2GAME_AI_TimedIdleTick_6DEC0(int32_t* ctx)
{
    uintptr_t unit = P(ctx[1]);
    uint32_t blk = Ordinal_10331(unit);
    uint32_t xy[2] = {0, 0};
    Ordinal_10141(unit, xy);
    Ordinal_10589(unit);
    Ordinal_10904(unit);
    uint32_t now = d2web_GetTickCount();
    uint32_t mode = unit ? rd32(unit, 0x10) : 0;
    if (now < rd32(unit, 0xD4) + 0x1F4) return 1;  // the original dereferences the unit here even when null
    uint32_t rec = Ordinal_10688(ctx[4]);
    if (mode == 0) {
        Ordinal_10408(unit);
        Ordinal_11090(unit, 1);
        *w32(unit, 0xD4) = now;
        sendMode1(P(ctx[0]), unit, rec);
    } else if (mode == 2) {
        Ordinal_10032(unit, blk, xy[0], xy[1]);
        Ordinal_11090(unit, 0);
        *w32(P(ctx[1]), 0xD4) = now;
    }
    return 1;
}

// 1.13c D2Game+0x6E0E0. Returns 0.
extern "C" int D2GAME_AI_ApplyStateRollAndStand_6E0E0(int32_t* ctx)
{
    uintptr_t unit = P(ctx[1]);
    if (unit && rd32(unit, 0x10)) return 0;
    FUN_6fd142d0(ctx[0], ctx[1], ctx[2], 0);
    FUN_6fd142d0(ctx[0], ctx[1], ctx[2], 1);
    Ordinal_11090(ctx[1], 1);
    sendMode1(P(ctx[0]), P(ctx[1]), Ordinal_10688(ctx[4]));
    return 0;
}

// 1.13c D2Game+0x6E270 (ecx = game, edx = unit)
extern "C" void D2GAME_AI_NeighborScanState_6E270(uintptr_t game, uintptr_t unit)
{
    uint32_t mode = unit ? rd32(unit, 0x10) : 0;
    uintptr_t blk = Ordinal_10331(unit);
    uint8_t st = *reinterpret_cast<uint8_t*>(rd32(unit, 0x14) + 4);
    auto send = [&](uint32_t frame) { FUN_6fcaea80(game, unit, 3, frame, 0, 0, 0); };
    if (st >= 3 || st == 2) {
        if (st == 2 && mode != 0) {
            Ordinal_11090(unit, 0);
            *reinterpret_cast<uint8_t*>(rd32(unit, 0x14) + 4) = 0;
        }
        send(rd32(game, 0xA8) + 0xF);
        return;
    }
    bool flag = (st == 1);
    uint32_t buf[2];
    Ordinal_10141(unit, buf);
    uint32_t hits = 0;  // ebx: F42D0 calls made
    bool found = false;
    for (uintptr_t n = rd32(blk, 0x74); n; n = rd32(n, 0xE8)) {
        if (rd32(n, 0) != 0 || rd32(n, 0x10) == 0x11) continue;
        if (Ordinal_10522(unit, n) > 0) continue;
        if (flag) { FUN_6fd142d0(game, unit, n, 0); hits = 1; }
        else { FUN_6fcfffd0(0xD, 0); found = true; }
    }
    if (found) {
        *reinterpret_cast<uint8_t*>(rd32(unit, 0x14) + 4) = 1;
        send(rd32(game, 0xA8) + 0x19);
        return;
    }
    if (!flag) { send(rd32(game, 0xA8) + 0xF); return; }
    if (hits == 1) { *reinterpret_cast<uint8_t*>(rd32(unit, 0x14) + 4) = 2; Ordinal_11090(unit, 1); }
    else *reinterpret_cast<uint8_t*>(rd32(unit, 0x14) + 4) = 0;
    uint32_t rec = Ordinal_10688(rd32(unit, 4));
    send((rd32(rec, 0xDC) >> 8) + rd32(game, 0xA8) + 1);
}
