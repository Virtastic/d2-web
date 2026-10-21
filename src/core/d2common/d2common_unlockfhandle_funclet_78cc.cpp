#include <cstdint>

// 1.13c D2Common+0x078CC SEH cleanup funclet of the CRT fhandle-locked region (caller 0x7887).
// Runs in the parent frame: if the "locked" local (ebp-0x1c) is nonzero it calls
// __unlock_fhandle (0x7B3B) on the handle local (ebp-0x20). Frame locals are passed in.
struct D2UnlockFhandleFrame {
    uint32_t fh;       // ebp-0x20
    uint32_t locked;   // ebp-0x1c
};

void d2common_unlockfhandle_funclet_78cc(const D2UnlockFhandleFrame& f, void (*unlockFhandle)(uint32_t))
{
    if (f.locked != 0) unlockFhandle(f.fh);
}
