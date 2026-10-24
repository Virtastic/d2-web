// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c body.
#include "d2common_crt.h"

// DAT 0x6fde9588: 0x2d {oserr, errno} pairs.
static const uint32_t kErrTable[0x2d][2] = {
    {1,22},{2,2},{3,2},{4,24},{5,13},{6,9},{7,12},{8,12},{9,12},{10,7},{11,8},{12,22},{13,22},{15,2},{16,13},
    {17,18},{18,2},{33,13},{53,2},{65,13},{67,2},{80,17},{82,13},{83,13},{87,22},{89,11},{108,13},{109,32},
    {112,28},{114,9},{6,22},{128,10},{129,10},{130,9},{131,22},{132,13},{145,41},{158,13},{161,2},{164,11},
    {167,13},{183,17},{206,2},{215,11},{1816,12}};

// 1.13c D2Common+0x044CE: __dosmaperr. Store the DOS error, then errno = table match, else 13 (EACCES) for
// 19..36, 8 (ENOEXEC) for 188..202, else 22 (EINVAL).
extern "C" void d2common_dosmaperr_44ce(uint32_t err)
{
    g_d2common_crt.tdoserrno = err;
    int32_t* terrno = d2common_errno_44bc();
    for (uint32_t i = 0; i < 0x2d; i++)
        if (err == kErrTable[i][0]) { *terrno = static_cast<int32_t>(kErrTable[i][1]); return; }
    if (err > 0x12 && err < 0x25) *terrno = 0xd;
    else if (err > 0xbb && err < 0xcb) *terrno = 8;
    else *terrno = 0x16;
}

struct D2DosmaperrEnv {
    void* (*getptd)(void);
    uint32_t tdoserrno_offset;
    uint32_t terrno_offset;
    const uint32_t* errtable;
};
// Old seam form: runs the real function, then mirrors its results into the caller's thread data.
extern "C" void d2common_dosmaperr_44ce_seam(uint32_t err, const D2DosmaperrEnv* e)
{
    d2common_dosmaperr_44ce(err);
    uint8_t* p = static_cast<uint8_t*>(e->getptd());
    *reinterpret_cast<uint32_t*>(p + e->tdoserrno_offset) = g_d2common_crt.tdoserrno;
    *reinterpret_cast<int32_t*>(p + e->terrno_offset) = g_d2common_crt.terrno;
}
