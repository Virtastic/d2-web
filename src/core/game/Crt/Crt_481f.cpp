// 1.13c D2Common+0x481F __write(fh, buf, n). the 1.10f source has no twin (host CRT).
#include "Crt/D2CrtFdEnv.h"

static bool badf(const D2CrtFdEnv* e) { e->set_errno(9); e->set_doserrno(0); return true; }

// 1.13c D2Common+0x481F: __write. Unopened handle (osfile bit 0 clear) = EBADF, -1; rechecked under lock.
extern "C" int32_t d2common_write_481f(int32_t fh, const void* buf, uint32_t n, const D2CrtFdEnv* e)
{
    if (static_cast<uint32_t>(fh) < e->nhandle && (e->ioinfo_entry(fh)[4] & 1)) {
        e->lock_fhandle(fh);
        int32_t r = -1;
        if (e->ioinfo_entry(fh)[4] & 1) r = e->write_nolock(fh, buf, n);
        else badf(e);
        e->unlock_fhandle(fh);
        return r;
    }
    badf(e);
    return -1;
}
