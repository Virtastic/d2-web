// src/core/realm/bnclient_6677.cpp -- the BNClient CRT __setmbcp (VC7.1, cdecl(cp), SEH frame under lock 0xD).
// Resolves the -2/-3/-4 pseudo code pages (OEM, ANSI, the stored default; each sets the has-default flag, which is
// cleared first); an unchanged page returns 0. Otherwise the snapshot block is the saved one when its first dword is
// zero, else a fresh 0x220-byte allocation; on a successful 0x64E3 it receives {0, cp, mb_flag, lang, the five u16
// at 0x1FA90, the 257-byte ctype table at +0x1C, the 256 bytes of 0x1FAA0 at +0x11D} and becomes the saved block.
// A failure frees the block unless it is the saved one.
#include <cstdint>
#include <cstring>
#include "core/realm/bnclient_mbcp.h"

// 1.13c BNClient+0x06677 (327 B, caller 0x67C7). Returns 0, or -1 on failure.
extern "C" int32_t bn_setmbcp_wrap(int32_t cp, const BnMbcpGlobals* g, const BnMbcpSaved* sv, const BnMbcpWrapSeams* s) {
    int32_t rc = -1;
    s->lock(0xd);
    *g->has_default = 0;
    if (cp == -2) { *g->has_default = 1; cp = (int32_t)s->oemcp(); }
    else if (cp == -3) { *g->has_default = 1; cp = (int32_t)s->acp(); }
    else if (cp == -4) { *g->has_default = 1; cp = (int32_t)*sv->default_cp; }
    if ((uint32_t)cp == *g->cp) {
        rc = 0;
    } else {
        uint8_t* blk = (uint8_t*)*sv->saved;
        if (!blk || *(const uint32_t*)blk != 0) blk = (uint8_t*)s->alloc(0x220);
        if (blk && (rc = s->setmbcp((uint32_t)cp)) == 0) {
            const uint32_t head[4] = {0, *g->cp, *g->mb_flag, *g->lang};
            std::memcpy(blk, head, 16);
            std::memcpy(blk + 0x10, g->extra, 10);
            std::memcpy(blk + 0x1c, g->ctype, 0x101);
            std::memcpy(blk + 0x11d, sv->tbl256, 0x100);
            *sv->saved = blk;
        }
        if (rc == -1 && blk != (uint8_t*)*sv->saved) s->release(blk);
    }
    s->unlock();
    return rc;
}
