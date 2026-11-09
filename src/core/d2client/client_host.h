// Callee seam for D2Client functions whose callees are not recovered yet: ours calls host(&call) in retail's call order.
// A call carries the registers and stack words retail passes (unused fields 0); host returns eax and may fill out[].
#pragma once
#include <cstdint>
struct D2cCall { uint32_t rva, eax, ecx, edx, ebx, esi, s[4]; uint32_t out[4]; uint32_t s4; uint32_t edi; };  // s4: stack word 4 (5-word callees, e.g. Ordinal_10079); edi: an EDI argument (0x1400); trailing so positional initializers keep their meaning
typedef uint32_t (*D2cHost)(D2cCall*);
