#pragma once
#include <cstdint>

// Native owners. Retail arithmetic stays explicitly 32/64 bit; no serialized FN record here.
extern "C" uint64_t d2game_aulldiv(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi);
extern "C" int32_t d2game_export10046(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

// Serialized FN adapters: r[0..3] arguments; r[4] result; extra arguments start at r[5].
extern "C" void d2_fn_d2game_3e70(uint32_t* r);
extern "C" void d2_fn_d2game_b87c0(uint32_t* r);
