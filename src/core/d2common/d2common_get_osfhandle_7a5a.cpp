// D2Common CRT descriptor -> OS handle leaf. Host seams as in d2common_crt_write_lseek.cpp.
#include <cstdint>

struct D2GetOsfhandleEnv {
    uint32_t nhandle;                               // DAT_6fdf1714
    uint8_t* (*ioinfo_entry)(int32_t fh);           // 0x24-byte record; osfile flags at +4
    void (*set_errno)(int32_t v);                   // 0x44BC cell
    void (*set_doserrno)(uint32_t v);               // 0x44C5 cell
};

// 1.13c D2Common+0x07A5A
// fh below _nfile (unsigned) with FOPEN (record+4 bit 0) returns record dword 0; else errno=EBADF(9),
// doserrno=0, -1.
extern "C" int32_t d2common_get_osfhandle_7a5a(int32_t fh, const D2GetOsfhandleEnv* e) {
    if (static_cast<uint32_t>(fh) < e->nhandle) {
        const uint8_t* r = e->ioinfo_entry(fh);
        if (r[4] & 1) return static_cast<int32_t>(r[0] | r[1] << 8 | r[2] << 16 | static_cast<uint32_t>(r[3]) << 24);
    }
    e->set_errno(9);
    e->set_doserrno(0);
    return -1;
}
