#pragma once
#include <cstdint>
struct D2UnitStrc;
// Retail EDI = unit, EDX = skill id; native object pointers are retained.
extern "C" void d2_client_2b4c0(D2UnitStrc* unit, int32_t skillId);
