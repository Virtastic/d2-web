// D2Multi batch n23: 11270, 11290, 11460, 11510, 11600. Globals are retail's, named by address.
#include <cstdint>
#include <cstdio>

extern "C" {
void Ordinal_10050(void*);
void Ordinal_10031(int32_t);
void Ordinal_10053(int32_t);
int32_t Ordinal_10098(const void*);
intptr_t Ordinal_10020(intptr_t);
int32_t Ordinal_10003(void);
void d2_multi_sub_10b70(void);
uint32_t* d2_multi_sub_10d10(uint8_t* edi, int32_t p1, uint32_t eax);
int32_t d2_multi_sub_fc90(char*, int32_t);
extern int32_t DAT_6fa0a090, DAT_6fa09dac, DAT_6fa09160, DAT_6fa0a0d0;
extern int32_t DAT_6fa09d64, DAT_6fa09d68, DAT_6fa09d6c, DAT_6fa09d70, DAT_6fa09d74, DAT_6fa09d78,
    DAT_6fa09d7c, DAT_6fa09d80, DAT_6fa09d84, DAT_6fa09d88, DAT_6fa09d8c, DAT_6fa09d90, DAT_6fa09d94,
    DAT_6fa09d98, DAT_6fa09d9c, DAT_6fa09da0, DAT_6fa09da4, DAT_6fa09da8;
extern uint8_t DAT_6f9eb740[], DAT_6f9eb770[], DAT_6f9eb7a0[], DAT_6f9eb7d0[], DAT_6f9eb890[];

// 1.13c D2Multi+0x11270
int32_t d2_multi_sub_11270(void) { d2_multi_sub_10b70(); return 1; }

// 1.13c D2Multi+0x11290  (register args: EDI=unit name ptr, ECX=param_1, EAX=param_3)
void d2_multi_sub_11290(uint8_t* edi, int32_t param_1, uint32_t eax) {
    uint32_t* p = d2_multi_sub_10d10(edi, param_1, eax);
    if (DAT_6fa0a090 != 0) {
        char buf[500];
        snprintf(buf, sizeof buf, "%s has joined the channel.", (char*)p + 0x36);
        d2_multi_sub_fc90((char*)buf, 5);
    }
}

// 1.13c D2Multi+0x11460
int32_t d2_multi_sub_11460(int32_t, int32_t param_2) {
    d2_multi_sub_11290(nullptr, param_2, 0);  // ponytail: retail passes EAX/EDI through unchanged; caller seam
    return 1;
}

// 1.13c D2Multi+0x11510
void d2_multi_sub_1510(void) {
    if (DAT_6fa09dac != 0) {
        if (DAT_6fa09dac == 1) {
            Ordinal_10050(&DAT_6fa09d64); Ordinal_10050(&DAT_6fa09d68); Ordinal_10050(&DAT_6fa09d6c);
            Ordinal_10050(&DAT_6fa09d70); Ordinal_10050(&DAT_6fa09d74);
        } else {
            Ordinal_10050(&DAT_6fa09d78); Ordinal_10050(&DAT_6fa09d7c); Ordinal_10050(&DAT_6fa09d80);
            Ordinal_10050(&DAT_6fa09d84); Ordinal_10050(&DAT_6fa09d88); Ordinal_10050(&DAT_6fa09d8c);
        }
        DAT_6fa09dac = 0;
        Ordinal_10050(&DAT_6fa09d90); Ordinal_10050(&DAT_6fa09d94); Ordinal_10050(&DAT_6fa09d98);
        Ordinal_10050(&DAT_6fa09d9c); Ordinal_10050(&DAT_6fa09da0); Ordinal_10050(&DAT_6fa09da4);
        Ordinal_10050(&DAT_6fa09da8);
    }
    Ordinal_10031(DAT_6fa09160);
    if (DAT_6fa0a0d0 != 0) Ordinal_10053(DAT_6fa0a0d0);
}

// 1.13c D2Multi+0x11600
void d2_multi_sub_1600(void) {
    DAT_6fa09d90 = Ordinal_10098(DAT_6f9eb740); Ordinal_10003(); Ordinal_10020(0);
    DAT_6fa09d94 = Ordinal_10098(DAT_6f9eb770); Ordinal_10003(); Ordinal_10020(0);
    DAT_6fa09d98 = Ordinal_10098(DAT_6f9eb7a0); Ordinal_10003(); Ordinal_10020(0);
    DAT_6fa09d9c = Ordinal_10098(DAT_6f9eb7d0); Ordinal_10003(); Ordinal_10020(0);
    DAT_6fa09da8 = Ordinal_10098(DAT_6f9eb890); Ordinal_10003(); Ordinal_10020(0);
}
}
