// D2Multi n71: 13540 and 13750 (the two login-screen builders, 1.13c). Globals are retail's, named by address.
#include <cstdint>

extern "C" {
char*    q_d2multi_n71_Ordinal_571(void);
void Ordinal_10097(void);  // x86 cdecl ignores the stale stack args; zero-arg to match the other callers (wasm signature)
int32_t Ordinal_10003(void);
void n71_Ordinal_10014(void*);
int32_t Ordinal_10098(const void*);
void Ordinal_10167(void);
void Ordinal_10185(void);
intptr_t q_d2multi_n71_Ordinal_10202(void);
void q_d2multi_n71_Ordinal_10031(intptr_t);
void q_d2multi_n71_Ordinal_10053(intptr_t);
void Ordinal_10146(void);
int32_t Ordinal_10021(void);
void d2_multi_sub_aa60(void);
void d2_multi_sub_8450(void);
void d2_multi_sub_8790(void);
void d2_multi_sub_be00(void);
void d2_multi_sub_11600(void);
uint32_t d2_multi_sub_1274(uint8_t*, uint8_t*);
extern uint8_t* DAT_6fa0917c;
extern void *DAT_6fa09188, *DAT_6fa0918c, *DAT_6fa09190;
extern int32_t DAT_6fa09118, DAT_6fa09dac, DAT_6fa0a094, DAT_6fa0a0d0;
extern intptr_t DAT_6fa09d64, DAT_6fa09d68, DAT_6fa09d6c, DAT_6fa09d70, DAT_6fa09d74, DAT_6fa09d78,
    DAT_6fa09d7c, DAT_6fa09d80, DAT_6fa09d84, DAT_6fa09d88, DAT_6fa09d8c, DAT_6fa09da0, DAT_6fa09da4;
extern char DAT_6fa09dd4[];
extern uint32_t DAT_6fa09de4, DAT_6fa09de8;
extern uint8_t DAT_6f9eb560[], DAT_6f9eb590[], DAT_6f9eb5f0[], DAT_6f9eb620[], DAT_6f9eb650[],
    DAT_6f9eb680[], DAT_6f9eb6b0[], DAT_6f9eb6e0[], DAT_6f9eb710[], DAT_6f9eb830[], DAT_6f9eb860[],
    DAT_6f9e74c4[], PTR_s_profile_sex_6f9e9c28[];

// shared prologue: four Ordinal_10097 scratch buffers published in 6fa09188/8c/90, profile_sex callback
static void login_prologue(uint8_t* b12, uint8_t* b44, uint8_t* b160, uint8_t* b_extra) {
    Ordinal_10097(); Ordinal_10097(); Ordinal_10097(); Ordinal_10097();
    DAT_6fa09188 = b12; DAT_6fa0918c = b44; DAT_6fa09190 = b160;
    (*(void (**)(int, void*))(DAT_6fa0917c + 0x78))(1, PTR_s_profile_sex_6f9e9c28);
    DAT_6fa09118 = 0;
    Ordinal_10003();
    d2_multi_sub_aa60();
    n71_Ordinal_10014((void*)d2_multi_sub_8450);
    d2_multi_sub_8790();
}

// copy the NUL-terminated name from the frame buffer into 6fa09dd4, then two trailing dwords
static void login_epilogue(const char* src, uint32_t a, uint32_t b) {
    char* d = DAT_6fa09dd4; char c;
    do { c = *src++; *d++ = c; } while (c);
    DAT_6fa09de4 = a; DAT_6fa09de8 = b;
}

// 1.13c D2MULTI+0x13540
void d2_multi_sub_13540(void) {
    uint8_t b12[12], b32[32], b168[168]; uint8_t tok[4];
    q_d2multi_n71_Ordinal_571();
    login_prologue(b12, b32, b168, b12);  // ponytail: retail frame layout is the decompiler's; buffers stay opaque
    if (DAT_6fa09118 == 0) {
        d2_multi_sub_be00();
        DAT_6fa09dac = 1;
        DAT_6fa09d64 = Ordinal_10098(DAT_6f9eb650);
        DAT_6fa09d68 = Ordinal_10098(DAT_6f9eb680);
        Ordinal_10167();
        DAT_6fa09d6c = Ordinal_10098(DAT_6f9eb6b0);
        Ordinal_10167();
        DAT_6fa09d70 = Ordinal_10098(DAT_6f9eb6e0);
        Ordinal_10167();
        DAT_6fa09d74 = Ordinal_10098(DAT_6f9eb710);
        tok[0] = DAT_6f9e74c4[0]; tok[1] = DAT_6f9e74c4[1]; tok[2] = DAT_6f9e74c4[2]; tok[3] = 0;
        uint32_t r = d2_multi_sub_1274(b32, tok);
        while (r != 0) { Ordinal_10167(); r = d2_multi_sub_1274(nullptr, tok); }
        DAT_6fa09da0 = Ordinal_10098(DAT_6f9eb830);
        q_d2multi_n71_Ordinal_10031(DAT_6fa09da0);
        if (DAT_6fa0a094 == 0) DAT_6fa09da4 = Ordinal_10098(DAT_6f9eb860);
        d2_multi_sub_11600();
        if (DAT_6fa0a0d0 != 0) q_d2multi_n71_Ordinal_10053(DAT_6fa0a0d0);
        login_epilogue((const char*)b32, 0, 0);  // ponytail: tail dwords are frame slots uStack_2c/28
    }
}

// 1.13c D2MULTI+0x13750
void d2_multi_sub_13750(void) {
    uint8_t b12[12], b44[44], b160[160], bx[12];
    login_prologue(b12, b44, b160, bx);
    if (DAT_6fa09118 == 0) {
        d2_multi_sub_be00();
        DAT_6fa09dac = 2;
        DAT_6fa09d78 = Ordinal_10098(DAT_6f9eb560);
        DAT_6fa09d7c = Ordinal_10098(DAT_6f9eb590);
        Ordinal_10167();
        DAT_6fa09d80 = Ordinal_10098(nullptr);
        Ordinal_10185(); q_d2multi_n71_Ordinal_10202();
        DAT_6fa09d84 = Ordinal_10098(DAT_6f9eb5f0);
        Ordinal_10185(); q_d2multi_n71_Ordinal_10202();
        DAT_6fa09d88 = Ordinal_10098(DAT_6f9eb620);
        Ordinal_10185(); q_d2multi_n71_Ordinal_10202();
        DAT_6fa09d8c = Ordinal_10098(nullptr);
        q_d2multi_n71_Ordinal_10031(DAT_6fa09d8c);
        DAT_6fa09da0 = Ordinal_10098(DAT_6f9eb830);
        if (DAT_6fa0a094 == 0) DAT_6fa09da4 = Ordinal_10098(DAT_6f9eb860);
        d2_multi_sub_11600();
        Ordinal_10146(); Ordinal_10146(); Ordinal_10146();
        q_d2multi_n71_Ordinal_10053(DAT_6fa09d88);
        Ordinal_10021();
        login_epilogue((const char*)b160, 0, 0);  // ponytail: tail data are frame slots iStack_2c/uStack_28/24
    }
}
}
