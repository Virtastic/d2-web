// D2Multi batch 17: B970, BC00, BE00, C510, C610. Globals are retail's, named by address.
#include <cstdint>

extern "C" {
void    Ordinal_10050(void*);
void    Ordinal_10031(int32_t);
void    Ordinal_10131(void);
void    Ordinal_10097(void);
int32_t Ordinal_10098(const void*);
intptr_t Ordinal_10020(intptr_t);  // signature shared with D2Game batch_b63 (wasm-ld rejects two import signatures)
int32_t Ordinal_10003(void);
uint32_t Ordinal_10265(int32_t);
void    Ordinal_10024(const void*, int32_t);
void d2_multi_sub_1a20(void); void d2_multi_sub_1da0(void); void d2_multi_sub_b900(void);
void d2_multi_sub_8790(void); void d2_multi_sub_1510(void);
void d2_multi_sub_b160(void*, int32_t); void d2_multi_sub_b2a0(void);
void d2_multi_sub_acb0(void); uint8_t d2_multi_sub_9000(const char*, const char*, uint8_t*, int32_t);
void d2_multi_exit(int32_t);
extern uint8_t DAT_6fa0a0d8[], DAT_6fa0a0cc[], DAT_6fa09dbc[], DAT_6fa09db0[], DAT_6fa09db4[], DAT_6fa09db8[];
extern int32_t DAT_6fa0a03c, DAT_6fa09dc0, DAT_6fa09160, DAT_6fa09f20, DAT_6fa09f88, DAT_6fa0a0e0;
extern uintptr_t DAT_6fa0917c;
extern uint8_t DAT_6f9ea2a0[], DAT_6f9e75d7[];

// 1.13c D2Multi+0xBE00
void d2_multi_sub_be00(void) {
    d2_multi_sub_1a20();
    d2_multi_sub_1da0();
    d2_multi_sub_b900();
    d2_multi_sub_8790();
    d2_multi_sub_1510();
    DAT_6fa0a03c = 0;
    Ordinal_10050(DAT_6fa0a0d8);
    Ordinal_10050(DAT_6fa0a0cc);
    if (DAT_6fa09dc0 == 1) {
        Ordinal_10050(DAT_6fa09dbc);
        Ordinal_10050(DAT_6fa09db0);
        Ordinal_10050(DAT_6fa09db4);
        Ordinal_10050(DAT_6fa09db8);
        DAT_6fa09dc0 = 0;
        Ordinal_10031(DAT_6fa09160);
    }
    if (DAT_6fa09f20 == 3) DAT_6fa09f20 = 0;
    if (DAT_6fa09f88 != 0) Ordinal_10131();
}

// 1.13c D2Multi+0xC610
int32_t d2_multi_sub_c610(int32_t p) {
    d2_multi_sub_be00();
    if (DAT_6fa09f88 != 0) Ordinal_10131();
    Ordinal_10097(); Ordinal_10097(); Ordinal_10097(); Ordinal_10097();
    *(int32_t*)DAT_6fa0a0d8 = Ordinal_10098(DAT_6f9ea2a0);
    Ordinal_10020(0);
    DAT_6fa0a0e0 = p;
    return 1;
}

// 1.13c D2Multi+0xC510  (fastcall: ecx=time, edx=flag)
int32_t d2_multi_sub_c510(uint32_t t, int32_t flag) {
    if (flag == 0) {
        Ordinal_10024(DAT_6f9e75d7, Ordinal_10265(0xc9e));
        d2_multi_exit(-1);
    }
    if (t != 0) {
        // unix time -> FILETIME (t*10^7 + 0x019DB1DED53E8000), then local; 64-bit math is exact
        uint64_t ft = ((uint64_t)t + 11644473600ull) * 10000000ull;
        d2_multi_sub_b160(&ft, 0x40);  // local-time conversion is the host's FileTimeToLocalFileTime
        d2_multi_sub_b2a0(); d2_multi_sub_b2a0(); d2_multi_sub_b2a0();
    }
    return 1;
}
}
// B970 (realm UI init, string-table pointer fetch + 7 Ordinal_10098 sprite loads) and BC00 (shell-open of
// a URL via registry .html handler + CreateProcessA; web build has no registry): spec only, UI/Win32 bound.
