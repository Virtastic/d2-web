// D2Multi sub_13AF0: the create-character form check. Reads five widget handles (6fa09d2c name, 6fa09d38 / 6fa09d3c / 6fa09d40 the numeric fields,
// 6fa09d28 the OK button), validates the text of each and enables the OK button when all three checks pass. The D2Win/D2Lang thunks keep their
// register arguments (ECX/EDX) as leading parameters.
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
extern "C" {
extern __attribute__((weak)) int32_t DAT_6fa09d2c, DAT_6fa09d38, DAT_6fa09d3c, DAT_6fa09d40, DAT_6fa09d28;
__attribute__((weak)) uint16_t* d2m_13af0_t7c50(int32_t);
__attribute__((weak)) int32_t   d2m_13af0_t7e2a(void);
__attribute__((weak)) void      d2m_13af0_t7c44(int32_t, const char*);
__attribute__((weak)) int32_t   d2m_13af0_t7cda(int32_t);
__attribute__((weak)) void      d2m_13af0_t7c14(int32_t, int32_t);
__attribute__((weak)) void      d2m_13af0_u2w(char*, const uint16_t*, int32_t);
}

// 1.13c D2MULTI+0x13AF0
void d2_multi_sub_13af0(void) {
    char buf[256];
    int ebp = 0, edi = 0, ebx = 0;
    uint16_t* u = d2m_13af0_t7c50(DAT_6fa09d2c);
    if (u) {
        d2m_13af0_u2w(buf, u, 0x10);
        int len = (int)strlen(buf);
        if (buf[0] != ' ' && len >= 1 && buf[len - 1] != ' ') ebp = 1;
    }
    if (d2m_13af0_t7e2a() == 0) { edi = 1; ebx = 1; }
    else {
        u = d2m_13af0_t7c50(DAT_6fa09d38);
        if (u) {
            d2m_13af0_u2w(buf, u, 3);
            if (strlen(buf) != 0) {
                edi = 1;
                long v = atol(buf);
                if (v < 1 || v > 8) {
                    v = v < 1 ? 1 : 8;
                    sprintf(buf, "%d", (int)v);
                    d2m_13af0_t7c44(DAT_6fa09d38, buf);
                }
            }
        }
        if (d2m_13af0_t7cda(DAT_6fa09d40) == 0) ebx = 1;
        else {
            u = d2m_13af0_t7c50(DAT_6fa09d3c);
            d2m_13af0_u2w(buf, u, 3);
            if (strlen(buf) != 0 && atol(buf) >= 0) ebx = 1;
        }
    }
    d2m_13af0_t7c14(DAT_6fa09d28, ebp && edi && ebx ? 1 : 0);
}
