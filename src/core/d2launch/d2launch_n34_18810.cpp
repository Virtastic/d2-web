#include <cstdint>
extern "C" {
extern int32_t* g_d2launch_bnet;  // DAT_6fa656c4
int32_t d2launch_ord_10027(void) __asm__("d2launch_ord_10027_v");  // distinct import (other files declare 3 args)
int32_t d2launch_ord_425(const char*, const char*, int, const char*);
void d2launch_BNGateway_SetCurGateway(int32_t);
const char* d2launch_BNGateway_Realm(int32_t);
void d2launch_sub_15D30(void);
void d2launch_sub_17DF0(void);
}
// 1.13c D2Launch+0x18810 -- gateway chosen: remember it as "Preferred Realm", then rebuild the menu.
extern "C" int32_t d2launch_sub_18810(void) {
    int32_t gw = (*(int32_t (**)(void))((uint8_t*)g_d2launch_bnet + 0x98))();
    if (d2launch_ord_10027() != -1) {
        d2launch_BNGateway_SetCurGateway(gw);
        d2launch_ord_425("Diablo II", "Preferred Realm", 0, d2launch_BNGateway_Realm(gw));
    }
    d2launch_sub_15D30();
    d2launch_sub_17DF0();
    return 1;
}
