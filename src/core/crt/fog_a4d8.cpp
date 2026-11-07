// Prior art used: MSVC VC7.1 CRT _commit / __unlock_fhandle (decompile cache); ioinfo entries are 0x24 bytes, 32 per block, flags at +4 (bit 0 = FOPEN).
#include <cstdint>

struct FogCrtHost {
    uint32_t nhandle;                       // DAT_6ffa7134, the handle count
    uint8_t* (*ioinfo)(uint32_t block);     // DAT_6ffa7140[block], 0 if absent
    void (*lock)(int32_t fh);               // __lock_fhandle
    void (*unlock)(int32_t fh);             // Fog+0xA578
    uint32_t (*flushOs)(int32_t fh);        // FlushFileBuffers(__get_osfhandle(fh)); 0 = ok, else GetLastError
    void (*setErrno)(int32_t v);            // *FUN_56A5A = v
    void (*setDosErrno)(uint32_t v);        // *FUN_56A63 = v
};

// 1.13c Fog+0x0A4D8: _commit(fh). Out-of-range or closed handle: errno = EBADF (9), return -1. Otherwise lock, and
// if still open flush the OS handle; success returns 0; failure sets _doserrno, then errno = 9, returns -1; unlock on both.
int32_t fog_a4d8_commit(const FogCrtHost& h, int32_t fh)
{
    if (h.nhandle <= (uint32_t)fh) { h.setErrno(9); return -1; }
    const uint32_t i = ((uint32_t)fh & 0x1f) * 0x24;
    if ((h.ioinfo((uint32_t)fh >> 5)[4 + i] & 1) == 0) { h.setErrno(9); return -1; }
    h.lock(fh);
    int32_t r;
    if ((h.ioinfo((uint32_t)fh >> 5)[4 + i] & 1) != 0) {
        const uint32_t e = h.flushOs(fh);
        if (e == 0) { h.unlock(fh); return 0; }
        h.setDosErrno(e);
    }
    h.setErrno(9);
    r = -1;
    h.unlock(fh);
    return r;
}

// 1.13c Fog+0x0A578: the unlock tail of _commit, `__unlock_fhandle(EBX)` (EBX = the handle, kept by the caller); 8 bytes.
void fog_a578_unlock(const FogCrtHost& h, int32_t fh) { h.unlock(fh); }
