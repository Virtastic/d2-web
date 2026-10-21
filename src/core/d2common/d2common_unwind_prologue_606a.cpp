#include <cstdint>

// 1.13c D2Common+0x0606A  __local_unwind2 prologue (caller 0x5FD6). Saves three registers into
// CRT globals: [0x6fde9848] = dword at [ebp+8], [0x6fde9844] = eax, [0x6fde984c] = ebp.
// Registers and the ebp-relative word are passed in; the globals are returned as a struct.
struct D2UnwindSaved {
    uint32_t g9848;  // DAT_6fde9848
    uint32_t g9844;  // DAT_6fde9844
    uint32_t g984c;  // DAT_6fde984c
};

D2UnwindSaved d2common_unwind_prologue_606a(uint32_t eax, uint32_t ebp, uint32_t ebpPlus8)
{
    return D2UnwindSaved{ebpPlus8, eax, ebp};
}
