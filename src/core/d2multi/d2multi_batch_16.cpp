// D2Multi batch 16: B1F0, B2A0, B300, B670, B900. Globals are retail's, named by address.
#include <cstdint>
#include <cstring>
#include <cstdio>

extern "C" {
void    Ordinal_10106(void);
void    Ordinal_10043(int32_t, int32_t);  // Fog free
void    Ordinal_10037(void);
int32_t Ordinal_10098(const void*);       // load/open by name
void    Ordinal_10147(void);
int32_t Ordinal_10054(void);
int32_t Ordinal_10003(void);
void    Ordinal_10000();
void    Ordinal_10035(void*);
void    Ordinal_10014(void*) __asm__("d2multi_Ordinal_10014b");
int32_t Ordinal_10036(void);
void    Ordinal_10007(void*);
void    Ordinal_10050(void*);
void    Ordinal_10031(int32_t);
void    Ordinal_10053(int32_t);
const char* Ordinal_10205(int32_t);
void    Ordinal_10022(int32_t);
void    EnterCriticalSection(void*);
void    LeaveCriticalSection(void*);
int32_t* d2_multi_sub_8bd0(void);
void    d2_multi_sub_8c80(int32_t, int32_t);
void    d2_multi_sub_8d80(void);
void    d2_multi_sub_8790(void);
void    d2_multi_sub_aa60(void);
extern uint8_t DAT_6fa08f00[];            // CRITICAL_SECTION
extern int32_t *DAT_6fa0a0e8, *DAT_6fa0a0ec;
extern int32_t DAT_6fa0a0c8, DAT_6fa0a0d0, DAT_6fa0a0d4, DAT_6fa0a09c, DAT_6fa09160;
extern uint8_t* DAT_6fa09f14;
extern uintptr_t DAT_6fa0917c;
extern int32_t DAT_6fa09138[], DAT_6fa09150, DAT_6fa09154, DAT_6fa09158, DAT_6fa0915c, DAT_6fa09174, DAT_6fa09178,
    DAT_6fa0912c[], DAT_6fa09f7c;
extern uint8_t DAT_6f9e9e50[], DAT_6f9e9e80[], DAT_6f9e9eb0[], DAT_6f9e9ee0[], DAT_6f9e9f10[], DAT_6f9ea000[],
    DAT_6f9ea030[], DAT_6f9ea060[], DAT_6f9ea0c0[], DAT_6f9ea180[], LAB_6f9d8f60[], LAB_6f9d8f20[];
extern int32_t DAT_6fa0a018, DAT_6fa0a01c, DAT_6fa0a020, DAT_6fa0a024, DAT_6fa0a028, DAT_6fa0a02c, DAT_6fa0a030,
    DAT_6fa0a038;
}

// 1.13c D2Multi+0xB1F0
void d2_multi_sub_b1f0(void) {
    for (;;) {
        EnterCriticalSection(DAT_6fa08f00);
        int32_t* n = DAT_6fa0a0e8;
        if (n) DAT_6fa0a0e8 = (int32_t*)n[3];
        LeaveCriticalSection(DAT_6fa08f00);
        if (!n) break;
        if (n[2]) Ordinal_10106();
        if (n[0]) {
            d2_multi_sub_8c80(DAT_6fa0a0c8, 0);
            if (n[0]) Ordinal_10043(0xc32, 0);
        }
        Ordinal_10043(0xc33, 0);
        Ordinal_10037();
    }
}

// 1.13c D2Multi+0xB2A0
void d2_multi_sub_b2a0(void) {
    int32_t* n = d2_multi_sub_8bd0();
    if (!n) return;
    EnterCriticalSection(DAT_6fa08f00);
    if (DAT_6fa0a0e8) DAT_6fa0a0ec[3] = (int32_t)(intptr_t)n;  // append to tail
    else DAT_6fa0a0e8 = n;
    DAT_6fa0a0ec = n;
    LeaveCriticalSection(DAT_6fa08f00);
}

// 1.13c D2Multi+0xB300
// ponytail: the Unicode string assembly (win2Unicode / strcpy / strcat into locals that are
// never read back by a visible consumer) is dropped; the Ordinal calls and global writes remain.
void d2_multi_sub_b300(void) {
    memset(DAT_6fa09138, 0, 0x44);  // 6fa09138..6fa0917c cleared
    DAT_6fa09150 = Ordinal_10098(DAT_6f9e9e50);
    DAT_6fa09154 = Ordinal_10098(DAT_6f9e9e80);
    DAT_6fa09158 = Ordinal_10098(DAT_6f9e9eb0);
    DAT_6fa0915c = Ordinal_10098(DAT_6f9e9ee0);
    DAT_6fa09160 = Ordinal_10098(DAT_6f9e9f10);
    DAT_6fa09174 = Ordinal_10098(DAT_6f9ea000);
    DAT_6fa09178 = Ordinal_10098(DAT_6f9ea030);
    Ordinal_10147();
    const uint8_t* name = DAT_6f9ea060;
    for (int32_t* p = DAT_6fa0912c; (uintptr_t)p < 0x6fa09131; p++, name += 0x30) *p = Ordinal_10098(name);
    DAT_6fa09f7c = Ordinal_10098(DAT_6f9ea0c0);
    Ordinal_10205(DAT_6fa09f14[0x1ea]);
    Ordinal_10022((DAT_6fa09f14[0x3c7] >> 2) & 1);
    d2_multi_sub_8d80();
    Ordinal_10000(); Ordinal_10003(); Ordinal_10000(); Ordinal_10003(); Ordinal_10000();
    DAT_6fa0a0d4 = Ordinal_10098(DAT_6f9ea180);
    if (DAT_6fa0a0d4) { Ordinal_10003(); Ordinal_10000(); }
}

// 1.13c D2Multi+0xB670
bool d2_multi_sub_b670(void) {
    if (DAT_6fa09f14[0x229] == 0) return true;
    if (DAT_6fa0917c && ((int32_t(*)(void))*(uintptr_t*)(DAT_6fa0917c + 0x68))()) {
        if (Ordinal_10054()) return true;
        DAT_6fa0a09c = 0;
        Ordinal_10003();
        d2_multi_sub_aa60();
        uint32_t* p = ((uint32_t*(*)(void))*(uintptr_t*)(DAT_6fa0917c + 0x44))();
        if (p && *p > 8) {
            Ordinal_10035(p + 1);
            Ordinal_10014(LAB_6f9d8f60);
            d2_multi_sub_8790();
            if (DAT_6fa0a09c == 0 && Ordinal_10054() && Ordinal_10036() == 0) {
                Ordinal_10007(DAT_6fa09f14 + 0xb9);
                Ordinal_10014(LAB_6f9d8f20);
                d2_multi_sub_8790();
                return Ordinal_10036() == 0;
            }
            d2_multi_sub_8790();
        }
    }
    return false;
}

// 1.13c D2Multi+0xB900
void d2_multi_sub_b900(void) {
    Ordinal_10050(&DAT_6fa0a018); Ordinal_10050(&DAT_6fa0a01c); Ordinal_10050(&DAT_6fa0a020);
    Ordinal_10050(&DAT_6fa0a024); Ordinal_10050(&DAT_6fa0a028); Ordinal_10050(&DAT_6fa0a02c);
    Ordinal_10050(&DAT_6fa0a030); Ordinal_10050(&DAT_6fa0a038);
    Ordinal_10031(DAT_6fa09160);
    if (DAT_6fa0a0d0) Ordinal_10053(DAT_6fa0a0d0);
}
