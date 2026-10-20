// 1.13c D2Gfx CRT lock leaves (alpha lane): the nine-byte wrappers around `_unlock` (0x19BC), which is
// `LeaveCriticalSection((LPCRITICAL_SECTION)(&DAT_6fa90048)[idx * 2])` [rva 0x19BC is STALLED: the lock
// table is kernel state and the compat shim's LeaveCriticalSection is a no-op]. The table is modelled as
// retail addresses it -- 0x40 pointers at +0x10048 -- so the call shape is the same even though the
// shim makes the effects invisible.
#include <Windows.h>
#include <cstdint>
#include <cstdlib>

static uint32_t g_lock_table[0x24 * 2];   // +0x10048..+0x10168, 36 pointer/flag pairs

// 1.13c D2Gfx+0x01967 (__mtdeletelocks): release heap-allocated sections and clear their
// table pointers first, then delete the sections held in static module storage. The flag at
// pair +4 distinguishes static storage (1) from heap storage; static pointers remain set.
extern "C" void d2gfx_mtdeletelocks(void) {
    for (uint32_t i = 0; i < 0x24; ++i) {
        uint32_t* slot = &g_lock_table[i * 2];
        if (slot[0] != 0 && slot[1] != 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)slot[0]);
            std::free((void*)(uintptr_t)slot[0]);
            slot[0] = 0;
        }
    }
    for (uint32_t i = 0; i < 0x24; ++i) {
        uint32_t* slot = &g_lock_table[i * 2];
        if (slot[0] != 0 && slot[1] == 1)
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)slot[0]);
    }
}

static void d2gfx_unlock(uint32_t idx) { LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_lock_table[idx * 2]); }

extern "C"
// 1.13c D2Gfx+0x2C27
void d2gfx_sub_2c27(void) { d2gfx_unlock(4); }

extern "C"
// 1.13c D2Gfx+0x274C
void d2gfx_sub_274c_h(void (*unlock)(int32_t)) { unlock(4); }   // probe seam: the callee as a callback
extern "C" void d2gfx_sub_274c(void) { d2gfx_unlock(4); }

extern "C"
// 1.13c D2Gfx+0x2803
void d2gfx_sub_2803(void) { d2gfx_unlock(4); }

extern "C"
// 1.13c D2Gfx+0xCB3B (SEH funclet): `if ([ebp-0x1c] != ecx) _unlock(ecx)`; frame value and lock index come from the unwound frame.
void d2gfx_sub_cb3b(uint32_t frame_val, uint32_t idx) { if (frame_val != idx) d2gfx_unlock(idx); }
