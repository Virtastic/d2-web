// D2Multi batch 19: E860, EA50, EB70, EC20, EC80. Globals are retail's, named by address.
// Register-passed args (retail's in_EAX/unaff_ESI/EBX) are passed as explicit parameters.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cctype>

extern "C" {
uint32_t Ordinal_10023(int32_t);
void     Ordinal_10194(int32_t, uint32_t);
uint32_t Ordinal_10265(int32_t);
void     Ordinal_10024(void*, int32_t);
int32_t  Ordinal_10003(void);
void     Ordinal_10060(uint32_t, int32_t*, int32_t*);
int32_t  Ordinal_10107(int32_t);
intptr_t Ordinal_10111(intptr_t, intptr_t);
void     Ordinal_10109(void);
int32_t  Ordinal_10002(uint32_t);
void     Ordinal_10043(int, int);
char*    Ordinal_571(const char*, int32_t);
void     q_d2multi_batch_19_Ordinal_501(void);
extern uint8_t* DAT_6fa09f08;
extern int32_t DAT_6fa09f34, DAT_6f9e9bc0, DAT_6fa09178, DAT_6fa09184;
extern char DAT_6fa09df0[], DAT_6f9e75d7[];
extern uint32_t* gdwInvBitMasks_exref;
void d2_multi_sub_e760(void);
size_t ustrlen(const uint16_t*);
uint16_t* ustrstr(const uint16_t*, const uint16_t*);
uint16_t* ustrcpy(uint16_t*, const uint16_t*);
uint16_t* ustrcat(uint16_t*, const uint16_t*);
void utoUnicode(uint16_t*, const char*, int32_t);
uint32_t GetTempPathA(uint32_t, char*);
uint32_t GetTempFileNameA(const char*, const char*, uint32_t, char*);
int DeleteFileA(const char*);
int wsprintfA(char*, const char*, ...);
}

// 1.13c D2Multi+0xE860
void d2_multi_sub_e860(void) {
    if (DAT_6fa09f08) {
        int32_t p = *(int32_t*)(DAT_6fa09f08 + 0x9c);
        if (!p) { Ordinal_10024(DAT_6f9e75d7, Ordinal_10265(0x84c)); exit(-1); }
        uint32_t m = Ordinal_10023(p);
        Ordinal_10194(*(int32_t*)(DAT_6fa09f08 + 0x9c), gdwInvBitMasks_exref[10] & m);
        DAT_6fa09f08 = 0;
        DAT_6f9e9bc0 = -1;
    }
    if (DAT_6fa09f34 == 0) d2_multi_sub_e760();
}

// 1.13c D2Multi+0xEA50
int32_t d2_multi_sub_ea50(uint32_t* esi) {
    char tmp[260], dir[264];
    Ordinal_10060(0, nullptr, nullptr);
    if (GetTempPathA(0x104, dir) == 0) { dir[0] = '.'; dir[1] = 0; }
    if (GetTempFileNameA(dir, "BNetAd", 0, tmp) != 0) {
        DeleteFileA(DAT_6fa09df0);
        uint32_t z = 0;
        if (Ordinal_10107(0) != -1) { Ordinal_10111((intptr_t)esi[5], (intptr_t)&z); Ordinal_10109(); }
        const char* dot = strrchr((const char*)esi[2], '.');
        (void)tmp, Ordinal_10002((uint32_t)DAT_6fa09178);
        strcpy(DAT_6fa09df0, tmp);
    }
    DAT_6fa09184 = *esi;
    Ordinal_10043(0, 0);
    Ordinal_10043(0, 0);
    return 1;
}

// 1.13c D2Multi+0xEB70
void d2_multi_sub_eb70(uint16_t* s) {
    if (ustrlen(s) >= 500) { Ordinal_10024(DAT_6f9e75d7, Ordinal_10265(0x2ef)); exit(-1); }
    uint16_t* pat = (uint16_t*)Ordinal_10003();
    size_t n = ustrlen(pat);
    uint16_t* d = ustrstr(s, pat);
    while (d && d[n] != 0) {
        memmove(d, d + n + 1, (ustrlen(d) - n) * 2);
        d = ustrstr(s, pat);
    }
}

// 1.13c D2Multi+0xEC20
void d2_multi_sub_ec20(char* src, char* dst) {
    if (*src != '*') {
        char* e = Ordinal_571(nullptr, 0);
        if (e) { size_t n = e - src; memcpy(dst, src, n); dst[n] = 0; return; }
    }
    q_d2multi_batch_19_Ordinal_501();
}

// 1.13c D2Multi+0xEC80
void d2_multi_sub_ec80(int32_t v, uint16_t* dst) {
    uint16_t buf[512], num[6];
    char a[4];
    ustrcpy(buf, (uint16_t*)Ordinal_10003());
    wsprintfA(a, "%d", v);
    utoUnicode(num, a, 2);
    ustrcat(buf, num);
    if (*dst) {
        if (ustrlen(dst) > 499) { Ordinal_10024(DAT_6f9e75d7, Ordinal_10265(0xb7e)); exit(-1); }
        ustrcat(buf, dst);
    }
    ustrcpy(dst, buf);
}
