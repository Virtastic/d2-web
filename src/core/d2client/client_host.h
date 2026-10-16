// Callee seam for D2Client functions whose callees are not recovered yet: ours calls host(&call) in retail's call order.
// A call carries the registers and stack words retail passes (unused fields 0); host returns eax and may fill out[].
#pragma once
#include <cstdint>
struct D2cCall { uint32_t rva, eax, ecx, edx, ebx, esi, s[4]; uint32_t out[4]; };
typedef uint32_t (*D2cHost)(D2cCall*);
