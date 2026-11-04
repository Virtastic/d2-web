#pragma once
#include <cstdint>

struct D2g7cf10Env {
    int32_t (*dead)(const uint8_t* cand);                                  // 0xDFA90: EAX = cand
    int32_t (*check)(const uint8_t* cand, const uint8_t* unit, uint32_t game); // 0xE0A60: EAX = cand, ECX = unit, stack = game
    int32_t (*dist)(const uint8_t* unit, const uint8_t* cand);             // 0x21900: EAX = unit, EBX = cand
    int32_t (*stat)(const uint8_t* cand, uint32_t a, uint32_t b);          // stdcall import 0xA42A (cand, 6, 0)
};

extern "C" uint32_t d2_game_7cf10(uint32_t game, const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7cf10Env* e);
