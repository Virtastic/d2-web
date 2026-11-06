// 1.13c Fog+0x1FEF0: EDI = the object. Under the critical section at +0x2C (lock check line 0x53F, enter) the head node of the
// list at +0x74 is popped (head = node+4) and flag bit 2 cleared in +0x70; the section is checked again (line 0x552), left, and the
// popped node, if any, freed through the pool free (Fog+0x1B0C0, EAX = 0, ECX = &local, file string, line 0x571). Returns 1.
#include <cstdint>
#include <cstring>

struct Fog1FEF0Env {
    void (*lockCheck)(uint8_t* cs, uint32_t line);   // Fog+0x11690 (ECX = cs, EDX = line)
    void (*enter)(uint8_t* cs);                      // EnterCriticalSection
    void (*leave)(uint8_t* cs);                      // LeaveCriticalSection
    void (*poolFree)(uint32_t* slot, uint32_t line); // Fog+0x1B0C0 with the Fog+0x28C0C source string
};

uint32_t fog_1fef0_regs(uint8_t* self, const Fog1FEF0Env& env) {  // 1.13c Fog+0x1FEF0
    uint8_t* cs = self + 0x2c;
    env.lockCheck(cs, 0x53f);
    env.enter(cs);
    uint32_t head; std::memcpy(&head, self + 0x74, 4);
    if (head) { uint32_t next; std::memcpy(&next, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(head)) + 4, 4); std::memcpy(self + 0x74, &next, 4); }
    uint32_t f; std::memcpy(&f, self + 0x70, 4); f &= 0xfffffffbu; std::memcpy(self + 0x70, &f, 4);
    env.lockCheck(cs, 0x552);
    env.leave(cs);
    if (head) env.poolFree(&head, 0x571);
    return 1;
}
