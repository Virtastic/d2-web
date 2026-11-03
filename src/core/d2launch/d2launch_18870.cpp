#include <cstdint>
// 1.13c D2Launch+0x18870 -- stdcall(p): EAX = p into 0x11BE0; if true then 0x18810(p), else 1.
extern "C" uint32_t d2l_18870_core(uint32_t p, int (*gate)(uint32_t), uint32_t (*f18810)(uint32_t)) {
    return gate(p) ? f18810(p) : 1;
}
