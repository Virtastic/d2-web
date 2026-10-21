#include <cstdint>

// CRT ___freetlocinfo: free a thread locale-info block whose refcount hit zero. Block slots (32-bit
// retail words, widened here): [11]=0x2c refcount ptr, [12]=0x30 numeric-ptr, [13]=0x34 monetary-ptr,
// [15]=0x3c lconv, [16]=0x40 ctype refcount ptr, [17]=0x44 ctype table, [20]=0x50 time-info.
// Env seam: the static default blocks (DAT_6fdef8f0/6fdf1704/6fdf1708/6fdf1700/6fdef8ec) and the CRT
// helpers (_free, ___free_lconv_mon 0x668D... callees 0x687C/0x4C0D/0x681D/0x668D) are passed in.
struct FreeTLocEnv {
    void* defLconv;            // DAT_6fdef8f0
    int32_t* defMon;           // DAT_6fdf1704
    int32_t* defNum;           // DAT_6fdf1708
    int32_t* defCtype;         // DAT_6fdf1700
    void* defTime;             // DAT_6fdef8ec
    void (*freeFn)(void*);     // _free
    void (*freeLconvMon)(void*);
    void (*freeLconvNum)(void*);
    void (*freeLcTime)(void*);
};

// 1.13c D2Common+0x03270
void d2common_freetlocinfo_3270(void** blk, const FreeTLocEnv* e)
{
    if (blk[15] != e->defLconv && blk[15] != nullptr && *static_cast<int32_t*>(blk[11]) == 0) {
        int32_t* p = static_cast<int32_t*>(blk[13]);
        if (p && *p == 0 && p != e->defMon) { e->freeFn(p); e->freeLconvMon(blk[15]); }
        p = static_cast<int32_t*>(blk[12]);
        if (p && *p == 0 && p != e->defNum) { e->freeFn(p); e->freeLconvNum(blk[15]); }
        e->freeFn(blk[11]);
        e->freeFn(blk[15]);
    }
    int32_t* c = static_cast<int32_t*>(blk[16]);
    if (c != e->defCtype && c && *c == 0) {
        e->freeFn(c);
        e->freeFn(static_cast<uint8_t*>(blk[17]) - 0xfe);
    }
    int32_t* t = static_cast<int32_t*>(blk[20]);
    if (t != e->defTime && t && t[0x2d] == 0) {
        e->freeLcTime(t);
        e->freeFn(blk[20]);
    }
    e->freeFn(blk);
}
