// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c body. Host seams as in
// d2common_crt_write_lseek.cpp.
#include <cstdint>

struct D2LseekLkEnv {
    intptr_t (*get_osfhandle)(uint32_t fh);                       // 0x7A5A; -1 if unopened
    uint32_t (*set_file_pointer)(intptr_t h, int32_t off, uint32_t origin); // kernel32 SetFilePointer (no high dword)
    uint32_t (*get_last_error)();
    void (*dosmaperr)(uint32_t err);                              // 0x44CE
    void (*set_errno)(int32_t v);                                 // 0x44BC slot
    uint8_t* (*ioinfo_entry)(int32_t fh);                         // 0x24-byte record; osfile flags at +4
};

// 1.13c D2Common+0x07255: __lseek_lk. Bad handle -> errno EBADF, -1. On success clear osfile bit 0x02
// (FEOFLAG); on SetFilePointer failure (-1 with nonzero last error) map the DOS error and return -1.
extern "C" uint32_t d2common_lseek_lk_7255(uint32_t fh, int32_t off, uint32_t origin, const D2LseekLkEnv* e)
{
    intptr_t h = e->get_osfhandle(fh);
    if (h == -1) { e->set_errno(9); return 0xFFFFFFFFu; }
    uint32_t r = e->set_file_pointer(h, off, origin);
    uint32_t err = (r == 0xFFFFFFFFu) ? e->get_last_error() : 0;
    if (err == 0) { e->ioinfo_entry(static_cast<int32_t>(fh))[4] &= 0xFD; return r; }
    e->dosmaperr(err);
    return 0xFFFFFFFFu;
}
