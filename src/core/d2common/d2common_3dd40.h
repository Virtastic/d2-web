// D2Common 0x3DD40: stdcall(level), ret 4. Its two callees arrive through seams.
#pragma once
#include <cstdint>
// 0x7EA40: EAX = level, EDI = row, stack (&x, &y), ret 8; writes the found cell or leaves both at -1.
typedef void (*D2Common3DD40Find)(uint32_t level, int32_t row, int32_t* x, int32_t* y);
// 0x3D4A0: ESI = level, ECX = first-set vertex, EDX = second-set vertex, plain ret.
typedef void (*D2Common3DD40Coords)(uint32_t level, uint32_t src, uint32_t dst);
extern "C" void D2Common_3DD40_With(uint32_t level, D2Common3DD40Find find, D2Common3DD40Coords coords);
