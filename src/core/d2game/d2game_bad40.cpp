// 1.13c D2Game+0xBAD40, injectable callee. ECX = quest record (a4q3 init), plain ret. The callback addresses are
// module words the caller supplies (retail stores its own code addresses); the state block comes from the 0xA2E6
// pool allocator (ECX = [[q+4]+0x1C], EDX = 0x20, stack: file, line 0x358, 0).
#include <cstdint>
#include <cstring>
struct GBAD40Env {
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t zero);
    uint32_t cb_a0, cb_a8, cb_ac, cb_b0, cb_c0, cb_c4, cb_c8, cb_cc, cb_d4, cb_d8, cb_dc, cb_ec, cb_f0, file;
};
// 1.13c D2Game+0xBAD40
void g_bad40(uint32_t qa, const GBAD40Env& e)
{
    uint8_t* q = (uint8_t*)(uintptr_t)qa;
    auto w = [&](uint32_t o, uint32_t v) { memcpy(q + o, &v, 4); };
    memset(q + 0xA0, 0, 15 * 4);
    w(0x18, 0); q[0xB] = 0; q[0xC] = 0; w(0xE8, 0);
    w(0xA0, e.cb_a0); w(0xEC, e.cb_ec); w(0xDC, e.cb_dc);
    q[0xA] = 1; w(0xF0, e.cb_f0); w(0x10, 0x17); w(0xE0, 0x1B); q[0xD] = 4;
    w(0xC8, e.cb_c8); w(0xAC, e.cb_ac); w(0xA8, e.cb_a8); w(0xCC, e.cb_cc);
    w(0xD4, e.cb_d4); w(0xD8, e.cb_d8); w(0xC4, e.cb_c4); w(0xC0, e.cb_c0); w(0xB0, e.cb_b0);
    uint32_t game; memcpy(&game, q + 4, 4);
    uint32_t pool; memcpy(&pool, (const void*)(uintptr_t)(game + 0x1C), 4);
    uint32_t st = e.alloc(pool, 0x20, e.file, 0x358, 0);
    w(0x18, st);
    memset((void*)(uintptr_t)st, 0, 0x20);
}
