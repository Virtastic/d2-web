#pragma once
#include <cstdint>

// Native boundary for register arguments: retail 0x31F60 takes count in EAX,
// the input cursor address in EDI and the output cursor address in EBX.
// count must be positive; output must have room for at most count * 2 bytes.
// Both cursors advance. No pointer is stored in a retail 32-bit integer.
extern "C" void Storm_Sub31F60(uint32_t count, const uint8_t** input, uint8_t** output);

// Retail 0x32690 takes an 18-byte serialized TGA header in EAX.
extern "C" uint32_t Storm_Sub32690(const uint8_t* header);
