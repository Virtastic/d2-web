// D2Game+0x49F30 -- reset four dwords of a buffer to -1, then hand it to the release import.
// ecx = owner, edx = size, eax = buffer. size < 16 or a null owner is fatal (line 0x3be).
// release(owner, buffer, &local) takes a 16-byte stack local the function never initialises.
#include <cstdint>
#include <cstring>

// 1.13c D2Game+0x49F30
extern "C" void d2game_49f30_impl(void* owner, uint32_t size, void* buf,
    void (*release)(void* owner, void* buf, void* local), void (*fatal)(uint32_t line))
{
    if (size < 16 || owner == nullptr) { fatal(0x3be); return; }
    const uint32_t m = 0xffffffffu;
    for (int i = 0; i < 4; ++i) std::memcpy((uint8_t*)buf + i * 4, &m, 4);
    uint8_t local[16];
    release(owner, buf, local);
}
