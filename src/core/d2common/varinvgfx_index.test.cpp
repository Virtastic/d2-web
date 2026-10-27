// Host test: g++ -o /tmp/t src/core/d2common/varinvgfx_index.test.cpp && /tmp/t
#include "varinvgfx_index.h"
#include <cassert>
int main() {
    assert(d2common_varinvgfx_index(-5, 3) == 0);
    assert(d2common_varinvgfx_index(1, 3) == 1);
    assert(d2common_varinvgfx_index(9, 3) == 2);
    assert(d2common_varinvgfx_index(4, 0) == -1);
    return 0;
}
