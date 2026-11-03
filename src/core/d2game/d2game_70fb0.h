// Seam for D2Game+0x70FB0 PlrTrade init (EBX = unit, EDI = game, plain ret, AL = result).
#pragma once
#include <cstdint>
struct D2Game70FB0Env {
    uint32_t (*playerData)(uint32_t unit);                                  // 0xA4C0
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);        // 0xA2E6: ECX = pool, EDX = size, stack = file ("PlrTrade.cpp"), line, 0
    uint32_t (*writeItems)(uint32_t buf, uint32_t inv, uint32_t game, uint32_t unit, uint32_t size); // 0x537D0: ECX = buf, EDX = unit+0x60, stack = game, unit, size, 0, 0, 0 (ret 0x18)
    uint32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);     // 0xA42A
    void (*fatal)(uint32_t line);                                           // Fog #10024 + _exit(-1); never returns
};
uint32_t d2game_port_70fb0(uint32_t unit, uint32_t game, const D2Game70FB0Env* env);
