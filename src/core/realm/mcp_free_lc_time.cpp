#include <cstddef>
#include <cstdint>

struct D2McpLcTimeStrings {
    uint32_t abbreviated_days[7];
    uint32_t days[7];
    uint32_t remaining[29];
};
static_assert(offsetof(D2McpLcTimeStrings, days) == 0x1C);
static_assert(offsetof(D2McpLcTimeStrings, remaining) == 0x38);
static_assert(sizeof(D2McpLcTimeStrings) == 0xAC);

// 1.13c D2MCPClient+0x047D3 (400 B, caller 0x02E3B ___freetlocinfo).
// MSVC 2003 CRT destructor: free the 43 owned LC_TIME string pointers in retail order.
extern "C" void d2_mcp_free_lc_time(const D2McpLcTimeStrings* time,
                                     void (*free_token)(uint32_t)) {
    if (time == nullptr) return;
    for (unsigned i = 1; i <= 6; ++i) free_token(time->abbreviated_days[i]);
    free_token(time->abbreviated_days[0]);
    for (unsigned i = 1; i <= 6; ++i) free_token(time->days[i]);
    free_token(time->days[0]);
    for (unsigned i = 0; i < 29; ++i) free_token(time->remaining[i]);
}
