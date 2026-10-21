#include <cstdint>

// 1.13c D2Common+0x0681D ___free_lconv_num(lconv): MSVC CRT. Releases each of the three numeric
// strings (decimal_point, thousands_sep, grouping) unless it is the current-locale string
// (*PTR_6fde994c [i]) or the static C-locale default (PTR_6fde991c/9920/9924). Null table: no-op.
// Env seam: current/default pointers and _free are owner-supplied.
struct D2CommonFreeLconvNumEnv {
    void* const* current;      // 3 entries from *PTR_6fde994c
    void* const* defaults;     // 3 entries: PTR_DAT_6fde991c, 9920, 9924
    void (*freeFn)(void*);     // _free (+0x4C0D)
};

void d2common_free_lconv_num_681d(void** table, const D2CommonFreeLconvNumEnv& e)
{
    if (!table) return;
    for (int i = 0; i < 3; ++i)
        if (table[i] != e.current[i] && table[i] != e.defaults[i])
            e.freeFn(table[i]);
}
