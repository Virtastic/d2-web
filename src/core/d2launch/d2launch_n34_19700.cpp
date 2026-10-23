#include <cstdint>
#include <cstdlib>
#include <unistd.h>
extern "C" {
extern uint8_t* g_d2launch_state;  // DAT_6fa65abc
int32_t d2launch_ord_10265(int32_t);
void d2launch_ord_10024(const char*, int32_t);
void d2launch_ord_10149(int32_t);
int32_t d2launch_ord_10148(void);
int32_t d2launch_ord_10191(void);
int32_t d2launch_ord_10227(void);
void d2launch_ord_10027(const char*, int32_t, int32_t);
void d2launch_ord_10141(int32_t);
void d2launch_sub_15D30(void);  // 0x15D30
void d2launch_sub_11CB0(void);
int32_t d2launch_sub_12940(void);  // 0x12940 (decomp: FUN_6fa52940)
uint8_t d2launch_sub_128D0(void);
void d2launch_sub_12770(void);
void d2launch_sub_19680(void);
void d2launch_sub_18160(void);
}
// 1.13c D2Launch+0x19700 -- logo/intro movie sequence; fatal (error 0x8d1) when state byte 0x35b is set.
extern "C" void d2launch_sub_19700(void) {
    if (g_d2launch_state[0x35b]) {
        d2launch_ord_10024("", d2launch_ord_10265(0x8d1));
        _exit(-1);
    }
    d2launch_ord_10149(0);
    d2launch_sub_15D30();
    d2launch_sub_11CB0();
    if (d2launch_ord_10148() != 0) {
        const bool low = d2launch_ord_10191() != 0;
        d2launch_ord_10027(low ? "Data\\Local\\Video\\New_BLIZ640x240.bik" : "Data\\Local\\Video\\New_BLIZ640x480.bik", low ? 2 : 0, 0);
        d2launch_ord_10027(low ? "Data\\Local\\Video\\BlizNorth640x240.bik" : "Data\\Local\\Video\\BlizNorth640x480.bik", low ? 2 : 0, 0);
        if (d2launch_sub_12940() == 0) {
            d2launch_sub_19680();
            d2launch_ord_10141(1);
            d2launch_sub_18160();
            return;
        }
        if (d2launch_ord_10227() != 0 && d2launch_sub_128D0() != 0) d2launch_sub_12770();
    }
    d2launch_ord_10141(1);
    d2launch_sub_18160();
}
