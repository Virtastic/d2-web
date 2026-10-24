#pragma once
#include <cstdint>
struct D2UnitStrc;
extern "C" int32_t d2_client_40550(D2UnitStrc* unit, int32_t id);
extern "C" int32_t d2_client_40450(int32_t idx, D2UnitStrc* unit, uint32_t calc, int32_t level);
extern "C" void d2_client_40df0(int32_t n, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t seedLow);
