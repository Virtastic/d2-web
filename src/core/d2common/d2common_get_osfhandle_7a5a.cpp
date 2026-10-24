// D2Common CRT descriptor -> OS handle leaf.
#include "d2common_crt.h"

// 1.13c D2Common+0x07A5A
// fh below _nfile (unsigned) with FOPEN (record+4 bit 0) returns record dword 0; else errno=EBADF(9),
// doserrno=0, -1.
extern "C" int32_t d2common_get_osfhandle_7a5a(int32_t fh) {
    if (static_cast<uint32_t>(fh) < g_d2common_crt.nhandle) {
        const uint8_t* r = d2common_ioinfo_entry(fh);
        if (r[4] & 1) return static_cast<int32_t>(r[0] | r[1] << 8 | r[2] << 16 | static_cast<uint32_t>(r[3]) << 24);
    }
    *d2common_errno_44bc() = 9;
    g_d2common_crt.tdoserrno = 0;
    return -1;
}

struct D2GetOsfhandleEnv {
    uint32_t nhandle;
    uint8_t* (*ioinfo_entry)(int32_t fh);
    void (*set_errno)(int32_t v);
    void (*set_doserrno)(uint32_t v);
};
// Old seam form: loads the caller's CRT cells into module state, runs the real function, mirrors errors back.
extern "C" int32_t d2common_get_osfhandle_7a5a_seam(int32_t fh, const D2GetOsfhandleEnv* e) {
    g_d2common_crt.nhandle = e->nhandle;
    if (static_cast<uint32_t>(fh) < e->nhandle) {
        const uint8_t* r = e->ioinfo_entry(fh);
        uint8_t* m = d2common_ioinfo_entry(fh);
        for (int i = 0; i < 8; i++) m[i] = r[i];
    }
    int32_t v = d2common_get_osfhandle_7a5a(fh);
    if (v == -1) { e->set_errno(9); e->set_doserrno(0); }
    return v;
}
