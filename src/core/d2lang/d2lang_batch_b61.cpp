// D2Lang batch b61: the one non-SEH, non-Win32 row of the list.
#include <cstdint>
#include <cstdlib>
#include <initializer_list>

// 1.13c D2Lang+0x3e37 (___free_lc_time): frees the 43 string pointers of a __lc_time_data.
// Retail frees slot 0 sixth and slot 7 after slot 0x0d; order is not observable.
void D2Lang_FreeLcTime(void** p)
{
    if (!p) return;
    for (int i = 0; i <= 0x2a; ++i) free(p[i]);
}
