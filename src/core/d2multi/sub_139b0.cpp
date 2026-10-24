// D2Multi sub_139B0. Globals are retail's, named by address. EBX (dest record, 0x3b bytes) is an explicit parameter.
#include <cstdint>
#include <cstdlib>
extern "C" {
uint32_t Ordinal_10265(int32_t);
void     Ordinal_10024(void*, int32_t);
extern uint8_t *DAT_6fa09f14, *DAT_6fa0917c;
extern char DAT_6f9e75d7[];
bool d2_multi_sub_134d0(void);
void d2_multi_sub_121b0(void);
}

// 1.13c D2MULTI+0x139B0
int32_t d2_multi_sub_139b0(uint8_t* ebx) {
    uint16_t w = *(uint16_t*)(DAT_6fa09f14 + 0x1eb);
    uint32_t a = 0, b = 0;
    if (w & 4) a = b = 4;
    bool lt;
    if (w & 0x20) { a |= 8; b |= 8; lt = (w & 0x1f00) < 0xa00; }
    else lt = (w & 0x1f00) < 0x800;
    if (lt) b |= (((w & 0x20) ? (w & 0x1f00) < 0x500 : (w & 0x1f00) < 0x400) ? 3 : 2);
    int32_t* p = (*(int32_t* (**)(uint32_t, uint32_t))(DAT_6fa0917c + 0x38))(a, b);
    if (!p) return 0;
    int32_t n = *p;
    if (n < 1) return 0;
    *(uint16_t*)(ebx + 0x33) = 1;
    *(int32_t*)(ebx + 0x37) = p[4];
    char* s = (char*)(p + 9);
    char* d = (char*)ebx + 1;
    for (const char* q = s; (*d++ = *q++); ) {}  // first string incl. NUL
    while (*s) s++;
    char* s2 = s + 1;           // second string
    while (*s2) s2++;
    char* s3 = s2 + 1;          // third string
    if (d2_multi_sub_134d0()) return 0;
    d = (char*)ebx + 0x11;
    do *d++ = *s3; while (*s3++);
    d2_multi_sub_121b0();
    return n;
}
