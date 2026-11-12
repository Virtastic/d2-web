// 1.13c D2Common+0x5DCC0 -- ECX = id: returns 3 for id 6, 2 for every other id.
#include <cstdint>
uint32_t d2common_5dcc0(uint32_t id) { return id == 6 ? 3u : 2u; }
