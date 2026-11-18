// 1.13c D2Client+0x772E0 (78 B): rebuild the party rows. Clear (0x74F90), then walk the roster from head->next (+0x80)
// calling the row builder (0x77170, EBX = y starting 0x5A step 0x26, stack = node) until the node is 0 and the
// counter is <= 8 (loop test exactly as compiled). Links are 32-bit (retail layout); head is the global at +0x11BC14.
#include <cstdint>
#include <cstring>

struct D2ClientRows772e0Env {
    void (*clear)();                          // 0x74F90
    void (*row)(uint32_t y, uint32_t node);   // 0x77170: EBX = y, stack = node
};

static inline uint32_t r772e0_rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

extern "C" void d2client_772e0_port(const uint32_t* headGlobal, const D2ClientRows772e0Env* env) {
    env->clear();
    const uint32_t head = *headGlobal;
    uint32_t y = 0x5A, esi = head ? r772e0_rd(head + 0x80) : 0;
    int32_t n = 0;
    while (esi != 0 || n > 8) {
        env->row(y, esi);
        y += 0x26;
        if (esi) { esi = r772e0_rd(esi + 0x80); }
        else { esi = *headGlobal; }
        n++;
    }
}
