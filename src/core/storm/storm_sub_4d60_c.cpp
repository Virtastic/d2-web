// C-linkage Storm+0x4D60 for storm_batch_k04sto.cpp (declares it inside extern "C"); the body is storm_batch_b47.cpp's.
#include <cstdint>
void Storm_Sub4D60(uint8_t* node);
extern "C" void Storm_Sub4D60(void* p) { Storm_Sub4D60((uint8_t*)p); }
