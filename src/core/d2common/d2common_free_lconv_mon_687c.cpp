#include <cstdint>

// 1.13c D2Common+0x0687C ___free_lconv_mon(lconv): MSVC CRT. Releases the seven monetary strings
// (lconv slots 3..9, offsets 0x0C..0x24) unless each is the current-locale string
// (*PTR_6fde994c [i]) or the static C-locale default (PTR_6fde9928..9940). Null table: no-op.
// Env seam: current/default pointers and _free (+0x4C0D) are owner-supplied.
struct D2CommonFreeLconvMonEnv {
    void* const* current;      // lconv-shaped table from *PTR_6fde994c (indexed 3..9)
    void* const* defaults;     // 7 entries: 6fde9928, 992c, 9930, 9934, 9938, 993c, 9940
    void (*freeFn)(void*);
};

void d2common_free_lconv_mon_687c(void** table, const D2CommonFreeLconvMonEnv& e)
{
    if (!table) return;
    for (int i = 0; i < 7; ++i)
        if (table[3 + i] != e.current[3 + i] && table[3 + i] != e.defaults[i])
            e.freeFn(table[3 + i]);
}
