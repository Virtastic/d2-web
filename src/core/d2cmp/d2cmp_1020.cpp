// 1.13c D2CMP+0x1020 (55 bytes): SEH frame around 0x1060 (unlink a {link, ref} node twice).
#include <cstdint>
extern "C" void d2cmp_1060(uint8_t* mem, uint32_t node);
// 1.13c D2CMP+0x1020
extern "C" void d2cmp_1020(uint8_t* m, uint32_t node) { d2cmp_1060(m, node); }
