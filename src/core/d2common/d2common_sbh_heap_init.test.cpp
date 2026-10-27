// Host test: g++ -o /tmp/t src/core/d2common/d2common_sbh_heap_init.test.cpp && /tmp/t
#include "d2common_crt_b48c2.cpp"
#include <cassert>
static uint32_t a_h, a_f, a_n, ret;
static uint32_t fake(uint32_t h, uint32_t f, uint32_t n) { a_h = h; a_f = f; a_n = n; return ret; }
int main() {
    uint32_t s[6];
    ret = 0;
    assert(d2common_sbh_heap_init(0x3f8, fake) == 0);
    assert(a_f == 0 && a_n == 0x140);
    d2common_sbh_state(s); assert(s[2] == 0);  // failure publishes the null table
    ret = 0x1000;
    assert(d2common_sbh_heap_init(0x3f8, fake) == 1);
    d2common_sbh_state(s);
    assert(s[0] == 0 && s[1] == 0 && s[2] == 0x1000 && s[3] == 0x3f8 && s[4] == 0x1000 && s[5] == 0x10);
    return 0;
}
