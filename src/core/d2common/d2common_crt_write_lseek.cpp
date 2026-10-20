// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c bodies. Same env-seam shape as
// bnclient_batch_6265.cpp: the ioinfo table, errno slots and lock calls are host seams.
#include <cstdint>

struct D2CrtFdEnv {
    uint32_t nhandle;                              // DAT_6fdf1714
    uint8_t* (*ioinfo_entry)(int32_t handle);      // record (0x24 B) of the handle's 32-slot block, or null
    void (*lock_fhandle)(int32_t handle);          // 0x7A9B
    void (*unlock_fhandle)(int32_t handle);        // 0x48A6 / 0x7350
    void (*set_errno)(int32_t value);              // 0x44BC slot
    void (*set_doserrno)(uint32_t value);          // 0x44C5 slot
    int32_t (*write_nolock)(int32_t, const void*, uint32_t);   // 0x4663
    int32_t (*lseek_nolock)(int32_t, int32_t, int32_t);        // __lseek_lk 0x7255
};

static bool badf(const D2CrtFdEnv* e) { e->set_errno(9); e->set_doserrno(0); return true; }

// 1.13c D2Common+0x0481F: __write. Unopened handle (osfile bit 0 clear) -> EBADF, -1; rechecked under lock.
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
