// 1.13c D2Gfx+0x5CD4 __lock_fhandle (MSVC 2003 Release CRT): lock the per-handle critical section of a
// CRT file handle, creating it on first use.
//   ioinfo = _pioinfo[fh >> 5] + (fh & 0x1f) * 0x24          (32 records per block, record stride 0x24)
//   if (ioinfo->lockinitflag == 0) {                         (+0x08)
//       __lock(10);
//       if (ioinfo->lockinitflag == 0) {
//           if (___crtInitCritSecAndSpinCount(&ioinfo->lock, 4000) == 0) return 0;   (+0x0C, finally unlocks 10)
//           ++ioinfo->lockinitflag;
//       }
//       unlock(10);                                          (the finally on every exit)
//   }
//   EnterCriticalSection(&ioinfo->lock);                     (+0x0C)
//   return 1;
// The handle is not range-checked here; retail's callers (0x3C6B __lseek, 0x3ED2 __write, ...) validate
// it against _nfile first. The CRT lock table and every critical section are kernel state and no-ops in the
// compat shim (src/core/d2gfx/d2gfx_crt_unlock.cpp, row 0x19BC), so the __lock(10) bracket cannot be
// observed; the observable state is lockinitflag going 0 -> 1 and the section the caller then enters.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
#include <Windows.h>

// MSVC 2003 Release CRT ioinfo: osfhnd +0x00, osfile +0x04, pipech +0x05, lockinitflag +0x08, then the
// 0x18-byte CRITICAL_SECTION at +0x0C. `lea (x,x,8); shl 2` in the callers is the *0x24 stride.
struct D2GfxIoInfo {
    uint32_t osfhnd;        // +0x00
    uint8_t  osfile;        // +0x04
    uint8_t  pipech;        // +0x05
    uint8_t  pad06[2];      // +0x06
    int32_t  lockinitflag;  // +0x08
    uint8_t  lock[0x18];    // +0x0C
};
static_assert(offsetof(D2GfxIoInfo, lockinitflag) == 0x08, "`cmp dword [esi+8],0`");
static_assert(offsetof(D2GfxIoInfo, lock) == 0x0C, "the section passed to EnterCriticalSection is at +0xC");
static_assert(sizeof(D2GfxIoInfo) == 0x24, "entry stride `lea (x,x,8); shl 2`");

// 0x6FA9E920: _pioinfo, 64 block pointers of 32 records each. Modelled flat: block*32 + entry == fh, so a
// block pointer is not needed to find the record.
static D2GfxIoInfo g_pioinfo[64 * 32];

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_lock_fhandle(int32_t fh) {
    D2GfxIoInfo* block = g_pioinfo + (fh >> 5) * 32;
    D2GfxIoInfo* io = block + (fh & 0x1f);
    if (io->lockinitflag == 0) {
        // __lock(10) ... finally unlock(10): both no-ops in the compat shim.
        if (io->lockinitflag == 0) {
            // ___crtInitCritSecAndSpinCount(&io->lock, 4000), which the compat InitializeCriticalSection
            // cannot fail; retail returns 0 only when the section cannot be built.
            InitializeCriticalSection((LPCRITICAL_SECTION)io->lock);
            io->lockinitflag += 1;
        }
    }
    EnterCriticalSection((LPCRITICAL_SECTION)io->lock);
    return 1;
}
