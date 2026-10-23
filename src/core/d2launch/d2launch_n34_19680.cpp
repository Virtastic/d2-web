#include <cstdint>
extern "C" {
void d2launch_sub_11CB0(void);
int32_t d2launch_ord_10191(void);
void d2launch_sub_14920(void);
void d2launch_ord_10027(char*, int32_t, int32_t);
void d2launch_sub_18160(void);
}
// 1.13c D2Launch+0x19680 -- play the 2-state video slot (0 normal, 2 low-res) with an uninitialised path buffer, then run 0x18160.
extern "C" void d2launch_sub_19680(void) {
    char buf[260];
    d2launch_sub_11CB0();
    int32_t mode = d2launch_ord_10191() != 0 ? 2 : 0;
    d2launch_sub_14920();
    d2launch_ord_10027(buf, mode, 0);
    d2launch_sub_18160();
}
