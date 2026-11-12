// 1.13c D2Launch+0x12280 -- stdcall, one ignored argument: flips bit 0x40 of the 16-bit word at +0x1EB of the record the module global at image offset 0x25ABC points at; returns 1.
#include <cstdint>
uint32_t d2launch_12280(uint8_t* rec, uint32_t) { rec[0x1EB] ^= 0x40; return 1; }   // rec = the record the module global points at; the flag word's low byte holds the bit
