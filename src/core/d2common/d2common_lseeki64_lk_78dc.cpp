// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c body. Seams as d2common_lseek_lk_7255.cpp.
#include <cstdint>

struct D2Lseeki64LkEnv {
    intptr_t (*get_osfhandle)(uint32_t fh);                                      // 0x7A5A; -1 if unopened
    uint32_t (*set_file_pointer)(intptr_t h, int32_t lo, int32_t* hi, uint32_t origin); // SetFilePointer with high dword
    uint32_t (*get_last_error)();
    void (*dosmaperr)(uint32_t err);                                             // 0x44CE
    void (*set_errno)(int32_t v);                                                // 0x44BC slot
    uint8_t* (*ioinfo_entry)(int32_t fh);                                        // osfile flags at +4
};

// 1.13c D2Common+0x078DC
// __lseeki64_lk: returns the 64-bit position (hi:lo). Bad handle -> errno EBADF, -1. On success clear osfile
// bit 0x02; on SetFilePointer failure with nonzero last error map it and return -1.
extern "C" uint64_t d2common_lseeki64_lk_78dc(uint32_t fh, int32_t lo, int32_t hi, uint32_t origin, const D2Lseeki64LkEnv* e)
{
    int32_t h2 = hi;
    intptr_t h = e->get_osfhandle(fh);
    if (h == -1) { e->set_errno(9); return ~0ull; }
    uint32_t r = e->set_file_pointer(h, lo, &h2, origin);
    if (r == 0xFFFFFFFFu) {
        uint32_t err = e->get_last_error();
        if (err != 0) { e->dosmaperr(err); return ~0ull; }
    }
    e->ioinfo_entry(static_cast<int32_t>(fh))[4] &= 0xFD;
    return (static_cast<uint64_t>(static_cast<uint32_t>(h2)) << 32) | r;
}
