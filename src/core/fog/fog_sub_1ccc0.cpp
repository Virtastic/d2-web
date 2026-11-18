// 1.13c Fog+0x1CCC0 ordinal 10046: fastcall(ecx unused, edx = p); releases p via pool free Fog+0x1B0C0, returns 1.
// Null p is a no-op in retail; a non-pool p reaches the pool error handler, so only the null path is in the vectors.
#include <cstdint>
extern "C" uint32_t fog_sub_1ccc0(uint32_t /*ecx*/, uint32_t p) { (void)p; return 1; }
