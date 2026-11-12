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
extern "C" void d2_multi_sub_be00(void);
void    d2_multi_sub_c610(int32_t);
extern "C" void d2_multi_sub_acb0(int32_t ebx, const void* text);
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
// Register-passed thunk arguments are leading parameters: #10147 (0x7C14) and #10131 (0x7C20) take ECX = slot word, EDX = state; #10003 (0x7EA8) ECX = string id;
// ACB0 takes EBX = 0x6f9d8a40 and one stack word; C610 two stack words (label 0x6f9d8fc0, text). Its callees are weak seams that fall back to the module ports.
extern "C" {
__attribute__((weak)) int32_t d2m_ce70_t7e2a(void);
__attribute__((weak)) uint32_t d2m_ce70_t7ea8(int32_t);
__attribute__((weak)) void d2m_ce70_t7c20(int32_t, int32_t);
__attribute__((weak)) void d2m_ce70_be00(void);
__attribute__((weak)) void d2m_ce70_acb0(int32_t, uint32_t);
__attribute__((weak)) void d2m_ce70_c610(uint32_t, const void*);   // retail's stack order: the text, then the callback
__attribute__((weak)) void d2m_e760_t7c14(int32_t, int32_t);
extern int32_t DAT_6fa09138[], DAT_6fa0a094;
}
extern "C" void d2_multi_sub_ce70(int32_t in_eax) {
    if (in_eax == 0) {
        if (DAT_6fa09f20 == 0 && DAT_6fa0a0d8 != 0) { if (d2m_ce70_be00) d2m_ce70_be00(); else d2_multi_sub_be00(); }
        uint16_t f = *(uint16_t*)(DAT_6fa09f14 + 0x1eb);
        if ((f & 8) == 0 || (f & 4) == 0) { d2m_e760_t7c14(DAT_6fa09138[6], 1); d2m_e760_t7c14(DAT_6fa09138[8], 1); }
        d2m_e760_t7c14(DAT_6fa09138[9], DAT_6fa0a094 == 0);
        if (DAT_6fa09f88 != 0) d2m_ce70_t7c20(DAT_6fa09f88, 1);
    } else if (d2m_ce70_t7e2a() == 0) {
        uint32_t t = d2m_ce70_t7ea8(0x14e7);
        if (d2m_ce70_acb0) d2m_ce70_acb0(0x6f9d8a40, t); else d2_multi_sub_acb0(0x6f9d8a40, (const void*)(intptr_t)t);
    } else {
        DAT_6fa09f20 = 0;
        uint32_t t = d2m_ce70_t7ea8(0x2b0c);
        if (d2m_ce70_c610) d2m_ce70_c610(t, LAB_6f9d8fc0); else d2_multi_sub_c610((int32_t)t);
        if (DAT_6fa09f88 != 0) d2m_ce70_t7c20(DAT_6fa09f88, 0);
    }
}

// 1.13c D2Multi+0xE730
extern "C" int32_t d2_multi_sub_e730(uint8_t* p1) {
    DAT_6fa09f14 = p1;
    Ordinal_10100();
    Ordinal_10176(1);
    d2_multi_sub_e150(p1);   // retail passes the context in EAX
    Ordinal_10144();
    return DAT_6fa0a0dc;
}

// 1.13c D2Multi+0xE760
// D2Win #10166 (thunk 0x7C50, ECX = DAT_6fa0a0d0) and #10147 (thunk 0x7C14, ECX = a button slot word, EDX = its state) keep their register arguments
// as leading parameters; the five slots are DAT_6fa09138[0], [1], [3], [4], [5] (6fa09138, 3c, 44, 48, 4c).
extern "C" {
__attribute__((weak)) uint16_t* d2m_e760_t7c50(int32_t);
__attribute__((weak)) void d2m_e760_t7c14(int32_t, int32_t);
extern int32_t DAT_6fa09138[], DAT_6fa0a0d0;
}
extern "C" void d2_multi_sub_e760(void) {
    const uint16_t* s = d2m_e760_t7c50(DAT_6fa0a0d0);
    int32_t on = (s && s[0]) ? 1 : 0;   // retail: Unicode::strlen(s) != 0 (IAT 0x6f9e601c); the decompile drops the result
    d2m_e760_t7c14(DAT_6fa09138[5], on);
    d2m_e760_t7c14(DAT_6fa09138[0], on);
    d2m_e760_t7c14(DAT_6fa09138[1], DAT_6fa09f08 && on);
    uint32_t m = 0;
    if (DAT_6fa09f08 && *(int32_t*)(DAT_6fa09f08 + 0x9c)) {
        m = Ordinal_10023(*(int32_t*)(DAT_6fa09f08 + 0x9c));
        m &= 1u << 9;   // gdwBitMasks[9]
    }
    d2m_e760_t7c14(DAT_6fa09138[3], DAT_6fa09f08 && m == 0);
    d2m_e760_t7c14(DAT_6fa09138[4], DAT_6fa09f08 && m != 0);
}
