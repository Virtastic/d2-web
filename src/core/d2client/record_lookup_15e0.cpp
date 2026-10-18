#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x15E0: EAX = skill id, EDX = n. sgptDataTables+0xB98 rows (stride 0x23C) / +0xBA0 count;
// valid index and non-null row and n > 0 -> (n-1) * dword row+0x154 + dword row+0x150 (32-bit wrap), else 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_15e0(int32_t index, int32_t count, uint32_t rowValid,
                                             int32_t field150, int32_t field154, int32_t n) {
    if (index < 0 || index >= count || !rowValid || n <= 0) return 0;
    return static_cast<uint32_t>((n - 1) * field154 + field150);
}
