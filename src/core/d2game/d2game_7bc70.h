#pragma once
#include <cstdint>

struct D2g7bc70Env {
    int32_t (*a6f4)(const uint8_t* unit, const uint8_t* cand);   // stdcall(unit, cand)
    int32_t (*a514)(const uint8_t* cand);                        // stdcall(cand)
    int32_t (*a688)(const uint8_t* unit);                        // stdcall(unit)
    int32_t (*a718)(const uint8_t* cand);                        // stdcall(cand)
    int32_t (*ad60)(const uint8_t* cand);                        // stdcall(cand)
    int32_t (*ad4e)(const uint8_t* cand);                        // stdcall(cand)
    int32_t (*c18a0)(const uint8_t* unit);                       // EAX = unit
    int32_t (*c1850)(int32_t id);                                // EAX = class id from 0x18A0
    int32_t (*c1900)(const uint8_t* unit, const uint8_t* cand);  // EAX = unit, EBX = cand
    int32_t (*c22a80)(const uint8_t* cand, uint32_t bit);        // EAX = cand, stack = bit
    int32_t tableCount;                                          // *(*sgptDataTables + 0xA80)
};

extern "C" uint32_t d2_game_7bc70(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7bc70Env* e);
extern "C" uint32_t d2_game_7bd00(const uint8_t* unit, const D2g7bc70Env* e);
extern "C" uint32_t d2_game_7bd50(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7bc70Env* e);
