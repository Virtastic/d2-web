// 1.13c Fog+0x1FEF0: EDI = this. Under the critical section at +0x2C (single-threaded here, so no lock)
// pops the head node of the list at +0x74 (head = head->next, node +4), clears flag bit 2 in +0x70,
// then frees the popped node via the pool free (Fog+0x1B0C0). Returns 1.
// NAMED only: struct memory, 32-bit retail addresses; not probe-able.
#include <cstdint>
#include <cstring>
void fog_free_block(void* block, const char* file, uint32_t line);

extern "C" uint32_t fog_sub_1fef0(uint8_t* self) {  // 1.13c Fog+0x1FEF0
    uint32_t head; std::memcpy(&head, self + 0x74, 4);
    if (head) { uint32_t next; std::memcpy(&next, reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(head)) + 4, 4); std::memcpy(self + 0x74, &next, 4); }
    uint32_t f; std::memcpy(&f, self + 0x70, 4); f &= 0xfffffffbu; std::memcpy(self + 0x70, &f, 4);
    if (head) fog_free_block(reinterpret_cast<void*>(static_cast<uintptr_t>(head)), "", 0);
    return 1;
}
