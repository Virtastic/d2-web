// src/core/d2client/crt_isatty_9f9a.cpp -- the CRT __isatty statically linked into D2Client.dll
// (VS2003 Release), from the cached 1.13c decompile. Base 0x6FAB0000.
//
// 6fab9f9a: 3b 0d 90 39 bd 6f   cmp 0x6fbd3990,%ecx        ; _nfile
//           73 ..               jae out                    ; unsigned: negative handles too
//           8b c1               mov %ecx,%eax
//           c1 e8 05            shr $5,%eax                ; block index
//           ...                 (&DAT_6fbd39a0)[block] + 4 + (handle & 0x1F) * 0x24
//           8a 44 08 04         mov 4(%eax,%ecx,1),%al     ; the record's device flag byte
//           83 e0 40            and $0x40,%eax             ; 0x40 = character device
#include <cstdint>
#include <emscripten.h>

// The descriptor table is owned by d2client_batch_03f82.cpp and read by the other CRT rows.
extern uint32_t g_fdcount;     // +0x123990  _nfile
extern uint32_t g_pioinfo[0x20]; // +0x1239A0  fd blocks of 0x20 records, stride 0x24

// 1.13c D2Client+0x09F9A (42 B, caller 0x4AE8 __flsbuf): __isatty. A handle at or above _nfile
// (unsigned compare, so any negative handle) is not a file; otherwise the result is bit 0x40 of the
// fd record's device-flag byte at +4. Returns the bit itself (0 or 0x40), not 0/1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_9f9a(int32_t handle) {
    const uint32_t fd = (uint32_t)handle;
    if (g_fdcount <= fd) return 0;
    const uint32_t record = g_pioinfo[fd >> 5] + (fd & 0x1Fu) * 0x24u;
    return *(const uint8_t*)(uintptr_t)(record + 4) & 0x40;
}
