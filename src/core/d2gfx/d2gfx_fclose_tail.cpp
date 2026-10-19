// src/core/d2gfx/d2gfx_fclose_tail.cpp -- 1.13c D2Gfx CRT leaf, reimplemented from the cached decompile
// (ghidra-d2gfx/decomp/0x5F11.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec,
// no vector. Host model follows d2gfx_crt_helpers.cpp: callees whose own rows are still UNMAPPED are seams
// on a caller-supplied struct in retail argument order.
#include <cstdint>

// The 1.13c FILE (32-bit CRT): ptr 0x0, cnt 0x4, base 0x8, flag 0xC, file 0x10, charbuf 0x14,
// bufsiz 0x18, tmpfname 0x1C. Same definition as d2gfx_writestr.cpp.
struct CrtStream32 { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; int32_t file; uint32_t charbuf; int32_t bufsiz; uint32_t tmpfname; };
static_assert(sizeof(CrtStream32) == 32, "1.13c MSVC 2003 FILE layout");

// The callees this row calls, both still UNMAPPED in this DLL (function list 0x05EB4 __flush and 0x07043
// _commit): (1) 0x5EB4 __flush(FILE*) -> 0 on success, -1 on a failed write; (2) 0x7043 _commit(int fd)
// -> 0 on success, nonzero on failure (FlushFileBuffers on the descriptor's handle).
struct D2GfxFcloseTailHost {
    int32_t  (*flush)(CrtStream32* file);
    uint32_t (*commit)(int32_t fd);
};

// 1.13c D2Gfx+0x05F11 (46 bytes): the tail of MSVC 2003 Release CRT _fclose.
// Contract: int __cdecl f(FILE *stream). __flush the stream first; a failed flush returns -1 with no
// commit. A stream whose _flag carries _IOCOMMIT (0x4000) then commits its descriptor (stream+0x10), and
// a commit failure becomes -1 (`-(d != 0)`); every other stream and every clean commit return 0.
// 1.13c D2Gfx+0x05F11
extern "C"
int32_t d2gfx_sub_5f11(CrtStream32* file, const D2GfxFcloseTailHost* host) {
    if (host->flush(file) != 0) return -1;
    if ((file->flag & 0x4000) != 0)
        return (int32_t)(0u - (uint32_t)(host->commit(file->file) != 0));
    return 0;
}
