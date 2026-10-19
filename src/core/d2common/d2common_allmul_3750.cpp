// 1.13c D2Common+0x03750 (__allmul): low 64 bits of a signed 64x64 multiply.
// Prior art used: D2MOO 1.10f has no CRT __allmul equivalent; the 1.13c decompile at this RVA and
// the x86 low-64 result convention are authoritative.
// The input halves are explicit retail 32-bit words; unsigned multiplication keeps the result
// modulo 2^64, matching the EDX:EAX result regardless of the sign of either operand.
#include <cstdint>

int64_t d2common_allmul(uint32_t lo1, int32_t hi1, uint32_t lo2, int32_t hi2)
{
	const uint64_t x = (static_cast<uint64_t>(static_cast<uint32_t>(hi1)) << 32) | lo1;
	const uint64_t y = (static_cast<uint64_t>(static_cast<uint32_t>(hi2)) << 32) | lo2;
	return static_cast<int64_t>(x * y);
}
