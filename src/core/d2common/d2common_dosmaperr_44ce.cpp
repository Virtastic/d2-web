// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c body. Env seams as in
// d2common_lseek_lk_7255.cpp.
#include <cstdint>

struct D2DosmaperrEnv {
    void* (*getptd)(void);               // 0x03475 __getptd
    uint32_t tdoserrno_offset;           // _tiddata._tdoserrno
    uint32_t terrno_offset;              // _tiddata._terrno
    const uint32_t* errtable;            // 0x2d pairs {oserr, errno} at RVA 0xDE9588
};

// 1.13c D2Common+0x044CE: __dosmaperr. Store the DOS error, then errno = table match, else 13 (EACCES) for
// 19..36, 8 (ENOEXEC) for 188..202, else 22 (EINVAL).
extern "C" void d2common_dosmaperr_44ce(uint32_t err, const D2DosmaperrEnv* e)
{
    uint8_t* p = static_cast<uint8_t*>(e->getptd());
    *reinterpret_cast<uint32_t*>(p + e->tdoserrno_offset) = err;
    int32_t* terrno = reinterpret_cast<int32_t*>(p + e->terrno_offset);
    for (uint32_t i = 0; i < 0x2d; i++)
        if (err == e->errtable[i * 2]) { *terrno = static_cast<int32_t>(e->errtable[i * 2 + 1]); return; }
    if (err > 0x12 && err < 0x25) *terrno = 0xd;
    else if (err > 0xbb && err < 0xcb) *terrno = 8;
    else *terrno = 0x16;
}
