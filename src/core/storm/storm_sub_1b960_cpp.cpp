// C++-linkage twin of sub_1B960 (Storm+0x1B960, ESI=arg), for the files that declare it outside extern "C" (storm_sub_271d0.cpp, storm_batch_b65.cpp).
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
void sub_1B960(int* esi, const char* name, int n) { Storm_Sub1B960(esi, name, n); }
