#pragma once
#include <cstdint>
struct D2UnitStrc;
struct D2SndUnit { uint32_t type, id, cls; const D2UnitStrc* unit; };
void d2client_sound_unit_sync(const D2SndUnit* u, uint32_t n);   // sound_unit_refs.cpp (1.13c D2Client+0x22B20)
void d2client_sound_unit_reset();
