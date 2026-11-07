// 1.13c Storm+0x1B170: debug-fill a freed block's payload with 0xDD, then release it.
// EDX = block header (u16 size at +0, overhead byte at +2, flags byte at +3: 1 = two pad bytes, 4 = external), stack: heap, payload (ret 8).
// The fill runs only while the module's debug-fill word is set and the block is not external; its length is the size less the pad, the
// overhead byte and the 8-byte header. The release callee takes the heap in ESI and the header in EDI.
#include <cstdint>

struct Storm1B170Env {
    const int32_t* debugFill;                       // the module's debug-fill word
    void (*release)(uint16_t* hdr, uint32_t heap);  // the block release (ESI = heap, EDI = header)
};

void Storm_Sub1B170(const Storm1B170Env& e, uint16_t* hdr, uint32_t heap, uint32_t* p) {
    const uint8_t flags = ((const uint8_t*)hdr)[3];
    if (*e.debugFill != 0 && (flags & 4) == 0) {
        const uint32_t n = (uint32_t)hdr[0] - (uint32_t)(flags & 1) * 2u - ((const uint8_t*)hdr)[2] - 8u;
        for (uint32_t i = n >> 2; i; --i) *p++ = 0xDDDDDDDDu;
        uint8_t* b = (uint8_t*)p;
        for (uint32_t i = n & 3; i; --i) *b++ = 0xDD;
    }
    e.release(hdr, heap);
}
