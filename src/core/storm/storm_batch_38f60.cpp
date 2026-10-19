// Prior art used: D2MOO Storm exports have no CRT bodies. The cached
// 1.13c ___addl body establishes this unsigned carry contract.
// This pure helper uses explicit retail-width operands and its cdecl argument order.
#include <cstdint>

// 1.13c Storm+0x40885
extern "C" uint32_t Storm_Addl(uint32_t a, uint32_t b, uint32_t* result) {
    uint32_t sum = a + b;
    *result = sum;
    return (sum < a || sum < b) ? 1u : 0u;
}
