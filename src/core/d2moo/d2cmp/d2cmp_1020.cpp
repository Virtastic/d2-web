// 1.13c D2CMP+0x1020 (55 bytes): SEH frame around 0x1060 (unlink a {link, ref} node twice). The callee keeps the node address in
// EAX from its first load to its return and the frame teardown here goes through ECX, so EAX on return is the node.
#include <cstdint>
extern "C" void d2cmp_1060(uint8_t* mem, uint32_t node);
// 1.13c D2CMP+0x1020
extern "C" uint32_t d2cmp_1020(uint8_t* m, uint32_t node) { d2cmp_1060(m, node); return node; }
