// D2Multi sub_13AF0. Ordinals' arguments are not recovered by the decompile; called bare.
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
extern "C" {
uint16_t* Ordinal_10166(void);
int32_t   Ordinal_10054(void);
int32_t   Ordinal_10003(void);
void      Ordinal_10185(void);
void      Ordinal_10147(void);
void      unicode2Win(char*, const uint16_t*, int32_t);  // Unicode::unicode2Win
}

// 1.13c D2MULTI+0x13AF0
void d2_multi_sub_13af0(void) {
    char buf[256];
    uint16_t* u = Ordinal_10166();
    if (u) unicode2Win(buf, u, 0x10);
    if (Ordinal_10054()) {
        u = Ordinal_10166();
        if (u) {
            unicode2Win(buf, u, 3);
            if (strlen(buf) > 0) {
                long v = atol(buf);
                if (v < 1 || v >= 9) {
                    v = v < 1 ? 1 : 8;
                    sprintf(buf, "%d", (int)v);
                    Ordinal_10185();
                }
            }
        }
        if (Ordinal_10003()) {
            u = Ordinal_10166();
            unicode2Win(buf, u, 3);
            if (strlen(buf) > 0) atol(buf);
        }
    }
    Ordinal_10147();
}
