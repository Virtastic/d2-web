// Storm batch b46-storm-1: leaf wrappers over already-ported list primitives.
#include <cstdint>
void Storm_Sub4C20(uint8_t* node);
struct StormElement;
StormElement* StormElementInit(StormElement* e);

// 1.13c Storm+0x3000: if node+8 != 0, unlink the {link,tag} pairs at +4 and +0xC.
void Storm_Sub3000(uint8_t* node) {
    if (*(uint32_t*)(node + 8) == 0) return;
    Storm_Sub4C20(node + 4);
    Storm_Sub4C20(node + 0xC);
}

// 1.13c Storm+0x2D80: SEH-guarded call of the RGN ctor (0x2410) when the pointer is non-null.
void Storm_Sub2D80(StormElement* e) {
    if (e) StormElementInit(e);
}
