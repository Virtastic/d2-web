// D2Multi sub_14100. Globals are retail's, named by address.
// Retail's stack frame is modelled as one buffer: rec (0x3b) at -0x244, then ab[8] at -0x208, a200 at -0x200, a104, a100.
#include <cstdint>
#include <cstring>
extern "C" {
int32_t  Ordinal_10108(void);
uint16_t* Ordinal_10166(void);
void     Ordinal_10138(void*);
int32_t  Ordinal_10054(void);
char*    Ordinal_10014(void) __asm__("d2multi_Ordinal_10014");  // distinct import: flat name is declared with other signatures elsewhere
int32_t  Ordinal_10003(void);
void     unicode2Win(char*, const uint16_t*, int32_t);  // Unicode::unicode2Win
extern int32_t DAT_6fa09dcc;
extern uint8_t DAT_6f9fa598[];
void d2_multi_sub_cbd0(uint16_t, char*);
void d2_multi_sub_1da0(void);
void d2_multi_sub_aa60(void);
void d2_multi_sub_acb0(void);
int32_t d2_multi_sub_139b0(uint8_t*);
}

// 1.13c D2Multi+0x14100
uint32_t d2_multi_sub_14100(void) {
    uint8_t frame[0x244] = {0};
    uint8_t* rec = frame;
    char* ab = (char*)frame + 0x3c;
    char* a200 = (char*)frame + 0x44;
    char* a104 = (char*)frame + 0x140;
    char* a100 = (char*)frame + 0x144;
    if (!Ordinal_10108()) return 0;
    uint16_t* u1 = Ordinal_10166();
    uint16_t* u2 = Ordinal_10166();
    if (!u1) return 0;
    unicode2Win(a200, u1, 0x100);
    unicode2Win(a100, u2, 0x100);
    Ordinal_10138(a200);
    Ordinal_10138(a104);
    uint16_t id = 0;
    char* nm = 0;
    if (!Ordinal_10054()) {
        bool hit = false;
        for (int32_t i = 0; i < DAT_6fa09dcc; i++)
            if (!strcmp((char*)DAT_6f9fa598 + 1 + i * 0x3b, ab)) {
                nm = Ordinal_10014();
                id = *(uint16_t*)(DAT_6f9fa598 + 0x33 + i * 0x3b);
                hit = true;
                break;
            }
        if (!hit) {
            if (!d2_multi_sub_139b0(rec)) { Ordinal_10003(); d2_multi_sub_acb0(); return 0; }
            if (strcmp((char*)rec + 1, ab)) return 0;
            nm = Ordinal_10014();
        }
    }
    d2_multi_sub_cbd0(id, nm);
    d2_multi_sub_1da0();
    Ordinal_10003();
    d2_multi_sub_aa60();
    return 1;
}
