// 1.13c D2Common+0x07470 (__aulldvrm): MSVC unsigned 64-bit divide with remainder.
// Prior art used: D2Client+0xA050 port (same CRT routine); decompile is authoritative.
// Retail returns the quotient in EDX:EAX and the remainder in ECX:EBX. Exact 64-bit
// division gives identical results to retail's shift-and-correct loop. Zero divisor traps.
#include <cstdint>

uint64_t d2common_aulldvrm(uint64_t dividend, uint64_t divisor, uint64_t* remainder)
{
	*remainder = dividend % divisor;
	return dividend / divisor;
}
