// D2Multi batch 13: 7EE0, 83E0, 8450, 8790, 8BD0. Globals are retail's, named by address.
#include <cstdint>

extern "C" {
int32_t Ordinal_10009(void);            // game-mode query
void*   Ordinal_10042(int32_t, int32_t); // Fog alloc(line, 0)
void    Ordinal_10043(int32_t, int32_t); // Fog free
void    Ordinal_10050(void*);            // release (per slot)
void    Ordinal_10031(int32_t);          // reset(handle)
void    Ordinal_10199(void);
extern int32_t DAT_6f9eaed8, DAT_6f9eb058, DAT_6f9eaa58, DAT_6f9eaa88, DAT_6f9eacc8, DAT_6f9eacf8,
    DAT_6f9ead28, DAT_6f9ea578, DAT_6f9ea5a8, DAT_6f9eb8f8, DAT_6f9ea7e8, DAT_6fa0a0f4, DAT_6f9ea818,
    DAT_6f9ea848, DAT_6f9ea968, DAT_6f9ea998, DAT_6f9ea9c8, DAT_6f9ea9f8, DAT_6f9eaa28, DAT_6f9ea548,
    DAT_6f9ea5d8, DAT_6fa09f24, DAT_6fa09f2c, DAT_6fa09160, DAT_6fa09118;
extern uint8_t* DAT_6fa0a0f0;
extern uint8_t DAT_6fa08e80[];
extern uint8_t DAT_6fa09188[];
extern uintptr_t DAT_6fa0917c;

// 1.13c D2Multi+0x7EE0
int32_t d2_multi_sub_7ee0(int32_t p) {
    if (Ordinal_10009() == 9) {
        DAT_6f9eaed8 = DAT_6f9eb058 = 0x62;
        DAT_6f9eaa58 = DAT_6f9eaa88 = 0x151;
        DAT_6f9eacc8 = DAT_6f9eacf8 = DAT_6f9ead28 = 0x177;
        DAT_6f9ea578 = DAT_6f9ea5a8 = 0x74;
        DAT_6f9eb8f8 = DAT_6f9ea7e8 = 0x65;
        DAT_6f9ea818 = 0x82; DAT_6f9ea848 = 0xb6; DAT_6f9ea968 = 0x60; DAT_6f9ea998 = 0x81;
        DAT_6f9ea9c8 = 0xb7; DAT_6f9ea9f8 = 0xee; DAT_6f9eaa28 = 0x12d; DAT_6f9ea548 = 0x60;
        DAT_6f9ea5d8 = 0xcd;
    }
    DAT_6fa0a0f4 = p;
    return 1;
}

// 1.13c D2Multi+0x83E0
void d2_multi_sub_83e0(void) {
    uint8_t* n = DAT_6fa0a0f0;
    while (n) { n = *(uint8_t**)(n + 0x204); Ordinal_10043(0x1b75, 0); }
    DAT_6fa0a0f0 = 0;
}

// 1.13c D2Multi+0x8450
int32_t d2_multi_sub_8450(void) {
    int32_t a[3] = {8, 0x28, 200};
    int32_t r = ((int32_t(*)(void*, void*))*(uintptr_t*)(DAT_6fa0917c + 0x7c))(DAT_6fa09188, a);
    if (r != 0 || DAT_6fa09118 != 0) Ordinal_10199();
    return 1;
}

// 1.13c D2Multi+0x8790
void d2_multi_sub_8790(void) {
    for (int32_t i = 0; i < DAT_6fa09f24; i++) Ordinal_10050(DAT_6fa08e80 + 4 * i);
    DAT_6fa09f24 = 0;
    Ordinal_10031(DAT_6fa09160);
    DAT_6fa09f2c = 0;
}

// 1.13c D2Multi+0x8BD0 (retail: edi/esi/retaddr-slot are implicit register inputs; named here)
int32_t* d2_multi_sub_8bd0(int32_t edi, int32_t esi, int32_t third) {
    int32_t* p = (int32_t*)Ordinal_10042(0xc17, 0);
    p[0] = p[1] = p[2] = p[3] = 0;  // retail writes before the null test
    if (!p) return p;
    if (edi == 0) { p[2] = third; p[0] = 0; p[1] = esi; return p; }
    p[0] = (int32_t)(intptr_t)Ordinal_10042(0xc22, 0);
    p[1] = edi; p[2] = 0xc17;
    return p;
}
}  // extern "C"
