// 1.13c D2CMP+0x01F30 (sub_1F30, EDI = list object): drain the offset-linked list.
// obj[0] = base address, obj[2] = head offset from base (>0 = non-empty). Node = base+off,
// node[0] = link (first), node[1] = prev (<0: ~address of the slot, else offset-relative).
#include <cstdint>

extern "C" void d2cmp_1f30(uint32_t* obj) {
    int32_t off;
    while ((off = (int32_t)obj[2]) > 0) {
        uint32_t* n = (uint32_t*)(uintptr_t)(obj[0] + off);
        uint32_t first = n[0];
        if (first) {
            uint32_t prev = n[1];
            uint32_t* slot = ((int32_t)prev < 0) ? (uint32_t*)(uintptr_t)~prev
                : (uint32_t*)(uintptr_t)((uint32_t)(uintptr_t)n + (prev - ((uint32_t*)(uintptr_t)first)[1]));
            *slot = first;
            ((uint32_t*)(uintptr_t)n[0])[1] = n[1];
            n[0] = 0;
            n[1] = 0;
        }
    }
}
