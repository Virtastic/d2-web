// 1.13c Fog+0x07F07: CRT lock leaf, calls _unlock(4) (Fog+0x3C3F) and returns nothing.
#include <cstdint>

void fog_unlock(int32_t index);   // Fog+0x3C3F, fog_batch_tiny_1018c.cpp

void fog_unlock4_7f07() { fog_unlock(4); }  // 1.13c Fog+0x07F07
