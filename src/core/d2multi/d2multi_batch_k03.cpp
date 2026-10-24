// D2Multi batch k03: small CRT leaves. Globals are retail's, named by address; host calls via extern.
#include <cstdint>
extern "C" {
void d2multi_unlock(void* env, void* tbl, int n);
}
struct k03_env;
extern k03_env* d2multi_k03_env;
extern void* d2multi_k03_tbl;
extern int32_t DAT_6fa0b4b4, DAT_6f9e91f0, DAT_6f9eba84, DAT_6fa0b38c;
extern int (*DAT_6f9ebe34)(uint32_t);
extern void (*DAT_6f9ebde4)(void*, uint32_t);
extern char** DAT_6fa0b3a0;
extern int* d2multi_errno_ptr();
extern uint32_t* d2multi_doserrno_ptr();
extern void __setmbcp(int);
extern uint32_t d2multi_gettick_mix();  // GetSystemTimeAsFileTime^pid^tid^tick^QPC
extern void d2multi_InitCritSec(void*);
extern void* d2multi_getproc_InitCritSecSpin();
extern void d2multi_amsg_exit(int);

// 1.13c D2Multi+0x5E6C
void sub_5E6C(void) { d2multi_unlock(d2multi_k03_env, d2multi_k03_tbl, 0xd); }

// 1.13c D2Multi+0x5E75
int ___initmbctable(void) {
    if (DAT_6fa0b4b4 == 0) { __setmbcp(-3); DAT_6fa0b4b4 = 1; }
    return 0;
}

// 1.13c D2Multi+0x5E93
void ___security_init_cookie(void) {
    if (DAT_6f9e91f0 == 0 || DAT_6f9e91f0 == (int32_t)0xbb40e64e) {
        DAT_6f9e91f0 = (int32_t)d2multi_gettick_mix();
        if (DAT_6f9e91f0 == 0) DAT_6f9e91f0 = (int32_t)0xbb40e64e;
    }
}

// 1.13c D2Multi+0x6044
int ___crtInitCritSecNoSpinCount_8(void* cs) { d2multi_InitCritSec(cs); return 1; }

// 1.13c D2Multi+0x6054
static void ___crtInitCritSecAndSpinCount(void* cs, uint32_t spin) {
    if (!DAT_6f9ebde4) {
        void* p = nullptr;
        if (DAT_6f9eba84 != 1) p = d2multi_getproc_InitCritSecSpin();
        DAT_6f9ebde4 = p ? (void (*)(void*, uint32_t))p : (void (*)(void*, uint32_t))___crtInitCritSecNoSpinCount_8;
    }
    DAT_6f9ebde4(cs, spin);
}

// 1.13c D2Multi+0x6247
void sub_6247(void) { d2multi_unlock(d2multi_k03_env, d2multi_k03_tbl, 4); }

// 1.13c D2Multi+0x62FE
void sub_62FE(void) { d2multi_unlock(d2multi_k03_env, d2multi_k03_tbl, 4); }

// 1.13c D2Multi+0x7357
int __callnewh(uint32_t size) { return DAT_6f9ebe34 && DAT_6f9ebe34(size) ? 1 : 0; }

// 1.13c D2Multi+0x74EA
intptr_t __get_osfhandle(int fh) {
    if ((uint32_t)fh < (uint32_t)DAT_6fa0b38c) {
        intptr_t* p = (intptr_t*)(DAT_6fa0b3a0[fh >> 5] + (fh & 0x1f) * 0x24);
        if (*(uint8_t*)(p + 1) & 1) return *p;
    }
    *d2multi_errno_ptr() = 9;
    *d2multi_doserrno_ptr() = 0;
    return -1;
}

// 1.13c D2Multi+0x75C2
void sub_75C2(void) { d2multi_unlock(d2multi_k03_env, d2multi_k03_tbl, 10); }

// 1.13c D2Multi+0x75CB
void __unlock_fhandle(int fh) {
    extern void d2multi_LeaveCS(void*);
    d2multi_LeaveCS(DAT_6fa0b3a0[fh >> 5] + 0xc + (fh & 0x1f) * 0x24);
}

// 1.13c D2Multi+0x7874
void __fptrap(void) { d2multi_amsg_exit(2); }
