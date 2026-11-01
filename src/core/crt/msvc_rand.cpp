// MSVC CRT rand/srand (see msvc_rand.h). holdrand = holdrand * 0x343FD + 0x269EC3; rand = (holdrand >> 16) & 0x7FFF.
#include "msvc_rand.h"

namespace d2::crt {
namespace {
uint32_t g_holdrand = 1;
}  // namespace

// 1.13c BNClient+0x15F2  _srand: p->_holdrand = seed
void msvc_srand(uint32_t seed) { g_holdrand = seed; }

// 1.13c BNClient+0x15FF  _rand
int msvc_rand() {
    g_holdrand = g_holdrand * 0x343fdu + 0x269ec3u;
    return int((g_holdrand >> 16) & 0x7fffu);
}
}  // namespace d2::crt
