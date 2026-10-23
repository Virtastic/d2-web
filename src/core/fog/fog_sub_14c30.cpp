// 1.13c Fog+0x14C30: EAX = list head. Walks nodes linked at +0x20C, freeing each via the pool free (Fog+0x1B0C0).
// NAMED only: touches struct memory, not probe-able; link slot is a 32-bit retail address.
#include <cstdint>
#include <cstring>
void fog_free_block(void* block, const char* file, uint32_t line);

extern "C" void fog_sub_14c30(uint8_t* head) {  // 1.13c Fog+0x14C30
    while (head) {
        uint32_t next; std::memcpy(&next, head + 0x20c, 4);
        fog_free_block(head, "", 0);
        head = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(next));
    }
}
