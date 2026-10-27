// Host test: mkdir -p /tmp/e && echo '#define EMSCRIPTEN_KEEPALIVE' > /tmp/e/emscripten.h &&
//   g++ -I/tmp/e -o /tmp/t src/core/realm/bnclient_11c0.test.cpp && /tmp/t   (needs a 32-bit-addressable mmap)
#include "bnclient_batch_1000.cpp"
#include <cassert>
#include <sys/mman.h>
static uint32_t blk(size_t n) {
    void* p = mmap(0, n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    assert(p != MAP_FAILED);
    return (uint32_t)(uintptr_t)p;
}
static uint32_t rr_ret, freed, allocd;
static uint32_t rr(uint32_t, uint32_t, const char*, uint32_t, uint32_t) { return rr_ret; }
static uint32_t al(uint32_t n, const char*, uint32_t, uint32_t) { allocd = n; return blk(n); }
static void fr(uint32_t p, const char*, uint32_t, uint32_t) { freed = p; }
int main() {
    uint32_t h[3] = {0, 2, 0};
    rr_ret = 0x1234;  // realloc succeeds: only header changes
    d2_fn_bnclient_11c0(5, h, rr, al, fr);
    assert(h[0] == 5 && h[2] == 0x1234 && !allocd && !freed);
    uint32_t old = blk(2 * 0x408);
    ((uint8_t*)(uintptr_t)old)[0x408] = 7;
    ((uint8_t*)(uintptr_t)old)[0x407 + 0x408] = 9;
    h[0] = 0; h[1] = 2; h[2] = old; rr_ret = 0;  // fallback: alloc, copy min(old,new)=1, free old
    d2_fn_bnclient_11c0(1, h, rr, al, fr);
    assert(allocd == 0x408 && freed == old && h[0] == 1 && h[2] != old);
    h[1] = 2; h[2] = old; freed = 0;
    d2_fn_bnclient_11c0(2, h, rr, al, fr);
    assert(((uint8_t*)(uintptr_t)h[2])[0x408] == 7 && ((uint8_t*)(uintptr_t)h[2])[0x407 + 0x408] == 9);
    h[2] = 0; freed = 0;  // no old buffer: no free
    d2_fn_bnclient_11c0(2, h, rr, al, fr);
    assert(!freed);
    return 0;
}
