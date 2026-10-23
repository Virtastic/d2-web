// D2Multi batch n27: 14680 LVL_REST setting load, 14850 enter-state-3 / request. Ordinals and globals are weak seams.
#include <cstdint>
#define W __attribute__((weak))
extern int d2multi_ord423(const char* name, int, void* val) W;                    // read setting
extern void d2multi_ord426(const char* app, const char* name, int, const char* app2) W; // write default
extern void d2multi_ord578(void* val, int len, const void* tbl, const char* app) W;
extern void d2multi_ord10185() W; extern void d2multi_ord10097() W;
extern void d2multi_ord10060(int, int, int, int, int, int) W;
extern void d2multi_13DE0() W; extern void d2multi_13F20() W;
extern const uint8_t DAT_6f9e74e8[] W;
extern int32_t DAT_6fa09f20 W, DAT_6fa09dcc W, DAT_6fa09f18 W; extern uint8_t* DAT_6fa09f14 W;
extern int16_t DAT_6f9e9c34 W;

// 1.13c D2Multi+0x14680
void d2multi_14680(void) {
    int32_t v = 4;
    if (d2multi_ord423("LVL_REST", 0, &v)) {
        int32_t k = 0x34;
        d2multi_ord578(&k, 4, DAT_6f9e74e8, "Diablo II");
        d2multi_ord10185(); d2multi_ord10097(); d2multi_13DE0();
        return;
    }
    d2multi_ord426("Diablo II", "LVL_REST", 0, "Diablo II");
}

// 1.13c D2Multi+0x14850
void d2multi_14850(void) {
    DAT_6fa09f20 = 3; DAT_6fa09dcc = 0;
    if (DAT_6fa09f14[0x35a] != 2) { d2multi_13F20(); return; }
    int flag = (DAT_6fa09f14[0x1eb] & 4) ? 0x800 : 0;
    int16_t s;
    do { s = DAT_6f9e9c34 + 1; DAT_6f9e9c34 = 0; } while (s == 0); // 16-bit counter, skips 0
    DAT_6f9e9c34 = s;
    d2multi_ord10060(0, 0, 0, 0, 0, flag);
    DAT_6fa09f18 = DAT_6f9e9c34;
}
