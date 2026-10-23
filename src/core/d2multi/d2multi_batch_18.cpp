// D2Multi batch 18: C690, CBD0, CE70, E730, E760. Globals are retail's, named by address.
// Register-passed ordinal args (retail's unaff_/in_ registers) are not recoverable from the decompile.
#include <cstdint>

extern "C" {
void    Ordinal_10097(void);
void    Ordinal_10099(int32_t);
void    Ordinal_10199(void);
int32_t Ordinal_10054(void);
int32_t Ordinal_10003(void);
void    Ordinal_10038(int32_t);
void    Ordinal_10131(void);
void    Ordinal_10147(void);
void    Ordinal_10100(void);
void    Ordinal_10176(int32_t);
void    Ordinal_10144(void);
void*   Ordinal_10166(void);
uint32_t Ordinal_10023(int32_t);
void    d2_multi_sub_be00(void);
void    d2_multi_sub_c610(int32_t);
void    d2_multi_sub_acb0(void);
void    d2_multi_sub_e150(void*);
extern uintptr_t DAT_6fa0917c;
extern uint8_t* DAT_6fa09f14;
extern int32_t DAT_6fa09f20, DAT_6fa0a0dc, DAT_6fa0a0d8, DAT_6fa09f88, DAT_6fa09f18;
extern uint8_t* DAT_6fa09f08;
extern uint16_t DAT_6f9e9c34;
extern uint8_t LAB_6f9d8fc0[];
extern void* _DAT_6fa0a0e0;
extern uint32_t* gdwBitMasks_exref;
}

// 1.13c D2Multi+0xC690
void d2_multi_sub_c690(void) {
    (*(void (**)(int32_t))(DAT_6fa0917c + 0x1c))(0);
    d2_multi_sub_be00();
    Ordinal_10097(); Ordinal_10097(); Ordinal_10097(); Ordinal_10097();
    DAT_6fa09f20 = 0;
}

// 1.13c D2Multi+0xCBD0
int32_t d2_multi_sub_cbd0(uint16_t p1, const char* p2, const char* name, const char* pass) {
    DAT_6fa09f20 = 2;
    d2_multi_sub_be00();
    uint8_t* c = DAT_6fa09f14;
    _DAT_6fa0a0e0 = LAB_6f9d8fc0;
    *(uint16_t*)(c + 0x19) = p1;
    char* d = (char*)c + 0x1b; do *d++ = *name; while (*name++);
    d = (char*)c + 0x241;      do *d++ = *pass; while (*pass++);
    if (c[0x229] == 0) {
        *(int32_t*)(c + 0x15) = 7;
        d = (char*)c + 0x33; do *d++ = *p2; while (*p2++);
        DAT_6fa0a0dc = 1;
        Ordinal_10099(0);
        Ordinal_10199();
        return 1;
    }
    if (Ordinal_10054() == 0) {
        DAT_6fa09f20 = 0;
        d2_multi_sub_c610(Ordinal_10003());
        return 0;
    }
    *(int32_t*)(DAT_6fa09f14 + 0x15) = 3;
    do { DAT_6f9e9c34++; } while (DAT_6f9e9c34 == 0);
    Ordinal_10038(DAT_6f9e9c34);
    DAT_6fa09f18 = DAT_6f9e9c34;
    Ordinal_10131();
    return 1;
}

// 1.13c D2Multi+0xCE70
void d2_multi_sub_ce70(int32_t in_eax) {
    if (in_eax == 0) {
        if (DAT_6fa09f20 == 0 && DAT_6fa0a0d8 != 0) d2_multi_sub_be00();
        uint16_t f = *(uint16_t*)(DAT_6fa09f14 + 0x1eb);
        if ((f & 8) == 0 || (f & 4) == 0) { Ordinal_10147(); Ordinal_10147(); }
        Ordinal_10147();
        if (DAT_6fa09f88 != 0) Ordinal_10131();
    } else if (Ordinal_10054() == 0) {
        Ordinal_10003();
        d2_multi_sub_acb0();
    } else {
        DAT_6fa09f20 = 0;
        d2_multi_sub_c610(Ordinal_10003());
        if (DAT_6fa09f88 != 0) Ordinal_10131();
    }
}

// 1.13c D2Multi+0xE730
int32_t d2_multi_sub_e730(uint8_t* p1) {
    DAT_6fa09f14 = p1;
    Ordinal_10100();
    Ordinal_10176(1);
    d2_multi_sub_e150(nullptr);
    Ordinal_10144();
    return DAT_6fa0a0dc;
}

// 1.13c D2Multi+0xE760
void d2_multi_sub_e760(void) {
    void* s = Ordinal_10166();
    (void)s;  // retail: Unicode::strlen(s) when non-null, result unused
    Ordinal_10147(); Ordinal_10147(); Ordinal_10147();
    uint32_t m = 0;
    if (DAT_6fa09f08 && *(int32_t*)(DAT_6fa09f08 + 0x9c)) {
        m = Ordinal_10023(*(int32_t*)(DAT_6fa09f08 + 0x9c));
        m = gdwBitMasks_exref[9] & m;
    }
    Ordinal_10147();
    if (DAT_6fa09f08 && m) { Ordinal_10147(); return; }
    Ordinal_10147();
}
