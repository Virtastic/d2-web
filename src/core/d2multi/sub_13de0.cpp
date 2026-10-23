// D2Multi sub_13DE0. Ordinal arguments are not recovered by the decompile except 10003's.
#include <cstdint>
extern "C" {
int32_t Ordinal_10003();
void    Ordinal_10131(void);
void    Ordinal_10147(void);
void    Ordinal_10000(void);
void    d2_multi_sub_13af0(void);
}

// 1.13c D2Multi+0x13DE0
void d2_multi_sub_13de0(void) {
    int32_t k = Ordinal_10003() ? 4 : 5;
    for (int i = 0; i < 4; i++) Ordinal_10131();
    for (int i = 0; i < 3; i++) Ordinal_10147();
    Ordinal_10003();
    Ordinal_10000();
    Ordinal_10003();
    Ordinal_10000();
    d2_multi_sub_13af0();
}
