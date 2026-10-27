// Host test: mkdir -p /tmp/e && touch /tmp/e/emscripten.h && g++ -DEMSCRIPTEN_KEEPALIVE= -I/tmp/e -o /tmp/t src/core/d2win/d2win_sub_7f40.test.cpp && /tmp/t
#include "d2win_batch_8640.cpp"
#include <cassert>
#include <sys/mman.h>
int main() {
    // retail layout needs 32-bit addresses
    uint8_t* m = (uint8_t*)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    assert(m != MAP_FAILED);
    uint8_t* ent[3] = {m + 0x100, m + 0x200, m + 0x300};
    for (int i = 0; i < 3; i++) *(uint32_t*)(ent[i] + 0x1C) = i < 2 ? (uint32_t)(uintptr_t)ent[i + 1] : 0;
    *(int32_t*)m = 4;
    *(uint32_t*)(m + 0x48) = (uint32_t)(uintptr_t)ent[0];
    *(int32_t*)(m + 0x54) = 3;
    auto col = [&](int i) { return *(uint32_t*)(ent[i] + 0x14); };
    assert(d2win_sub_7F40((D2WinListCtl*)m, 1, 0xAB) == 1);
    assert(col(0) == 0 && col(1) == 0xAB && col(2) == 0);
    assert(d2win_sub_7F40((D2WinListCtl*)m, -1, 7) == 1);
    assert(col(0) == 7 && col(1) == 7 && col(2) == 7);
    assert(d2win_sub_7F40((D2WinListCtl*)m, 2, 9) == 1 && col(2) == 9);
    return 0;
}
