// 1.13c D2Multi+0x2AF0 (__aulldvrm): unsigned 64-bit divide; quotient in EDX:EAX (remainder in ECX:EBX, unused by the probe).
// Prior art used: D2Common+0x7470 port (same CRT routine). Zero divisor traps, as retail's DIV does.
#include <cstdint>
#include <emscripten.h>
uint64_t d2multi_aulldvrm(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi) {
	return (((uint64_t)hi << 32) | lo) / (((uint64_t)dhi << 32) | dlo);
}
