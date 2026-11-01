// 1.13c BNClient+0x0A180 (56 B, caller 0xB0C0): stdcall(p1, p2, p3) around four Storm big-number imports
// (thunks 0x8740 #624, 0x872E #612, 0x86F8 #622, 0x870A #606). A local slot is created, loaded with p3,
// used as the third operand of the operation (p1, p2, slot) and released. The imports are seams.
#include <cstdint>

struct BnA180Host {
    void (*big_new)(uint32_t* slot);                         // 0x8740: writes the new handle through the pointer
    void (*big_set)(uint32_t handle, uint32_t value);        // 0x872E
    void (*big_op)(uint32_t a, uint32_t b, uint32_t handle); // 0x86F8
    void (*big_del)(uint32_t handle);                        // 0x870A
};

extern "C" void bn_big_helper_a180(uint32_t p1, uint32_t p2, uint32_t p3, const BnA180Host* h) {
    uint32_t slot = 0;
    h->big_new(&slot);
    h->big_set(slot, p3);
    h->big_op(p1, p2, slot);
    h->big_del(slot);
}
