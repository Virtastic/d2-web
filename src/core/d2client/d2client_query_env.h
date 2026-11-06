// Seams of the 6E860 / 6E930 / 6EA50 query routines for the FN probes (src/core/fn/d2client_6e860.cpp, _6e930, _6ea50).
#pragma once
#include <cstdint>
struct D2cQueryEnv {
 int32_t (*f1a3d0)(void*,int32_t,int32_t); int32_t (*o10569)(void*,uint32_t*,uint32_t*); int32_t (*o10663)(void*);
 int32_t (*f1e2c0)(uint32_t*,uint8_t*); int32_t (*o10005)(uint8_t*,int32_t,int32_t); int32_t (*f1d280)(int32_t,int32_t,int32_t);
 int32_t (*o10060)(int32_t); int32_t (*o10000)(int32_t); int32_t (*o10069)(int32_t); int32_t (*o11142)(void*);
};
