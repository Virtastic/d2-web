// Host test: g++ -o /tmp/t src/core/d2cmp/d2cmp_3090.test.cpp && /tmp/t
#include "d2cmp_batch_crt_b54.cpp"
#include <cassert>
int main() {
    assert(d2cmp_3090("", "") == 0 && d2cmp_3090("abc", "abc") == 0);
    assert(d2cmp_3090("abc", "abd") == -1 && d2cmp_3090("abd", "abc") == 1);
    assert(d2cmp_3090("ab", "abc") == -1 && d2cmp_3090("abc", "ab") == 1);
    assert(d2cmp_3090("\xff", "a") == 1);  // unsigned bytes
    return 0;
}
