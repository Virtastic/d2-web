// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c bodies. Same env-seam shape as
// bnclient_batch_6265.cpp: the ioinfo table, errno slots and lock calls are host seams.
#include <cstdint>
#include "Crt/D2CrtFdEnv.h"

static bool badf(const D2CrtFdEnv* e) { e->set_errno(9); e->set_doserrno(0); return true; }

// 1.13c D2Common+0x072C9: __lseek, same guard; locked-but-closed returns 0xFFFFFFFF (== -1).
extern "C" int32_t d2common_lseek_72c9(int32_t fh, int32_t off, int32_t origin, const D2CrtFdEnv* e)
{
    if (static_cast<uint32_t>(fh) < e->nhandle && (e->ioinfo_entry(fh)[4] & 1)) {
        e->lock_fhandle(fh);
        int32_t r = -1;
        if (e->ioinfo_entry(fh)[4] & 1) r = e->lseek_nolock(fh, off, origin);
        else badf(e);
        e->unlock_fhandle(fh);
        return r;
    }
    badf(e);
    return -1;
}
