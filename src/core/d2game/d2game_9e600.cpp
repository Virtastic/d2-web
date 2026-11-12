// 1.13c D2Game+0x9E600 -- ECX = record: increments the 32-bit counter its +0x0C field points at and returns 1.
#include <cstdint>
uint32_t d2game_9e600(uint32_t* counter) { ++*counter; return 1; }   // counter = the pointer held at +0x0C of the record in ECX
