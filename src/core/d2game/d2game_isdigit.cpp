// MSVC _isdigit under the C locale: ctype table mask 4 is set only for '0'..'9'.
// Retail's multithreaded-locale branch (mb_cur_max > 1) goes through ___isctype_mt; not reachable here.
#include <cstdint>

// 1.13c D2Game+0xA056 (_isdigit)
int D2Game_IsDigit(int c)
{
    return (c >= '0' && c <= '9') ? 4 : 0;
}
int d2_fn_d2game_a056(int c) { return D2Game_IsDigit(c); }
