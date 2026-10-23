// D2Multi sub_13F20. Globals are retail's, named by address.
#include <cstdint>
#include <cstdlib>
extern "C" {
uint32_t Ordinal_10265(int32_t);
void     Ordinal_10024(void*, int32_t);
void     Ordinal_10167(void);
extern uint8_t *DAT_6fa09f14, *DAT_6fa0917c;
extern int32_t DAT_6fa09cc0, DAT_6fa09dcc;
extern uint8_t DAT_6f9ebf57[];
extern char DAT_6f9e75d7[];
void d2_multi_sub_11f80(void);
}

// 1.13c D2Multi+0x13F20
void d2_multi_sub_13f20(void) {
    uint16_t w = *(uint16_t*)(DAT_6fa09f14 + 0x1eb);
    uint32_t a = 0, b = 0;
    if (w & 4) a = b = 4;
    bool lt;
    if (w & 0x20) { a |= 8; b |= 8; lt = (w & 0x1f00) < 0xa00; }
    else lt = (w & 0x1f00) < 0x800;
    if (lt) b |= (((w & 0x20) ? (w & 0x1f00) < 0x500 : (w & 0x1f00) < 0x400) ? 3 : 2);
    int32_t* p = (*(int32_t* (**)(uint32_t, uint32_t))(DAT_6fa0917c + 0x38))(a, b);
    if (p) {
        int32_t n = *p, cnt = 0;
        uint8_t* s = (uint8_t*)(p + 1);
        uint8_t* rec = DAT_6f9ebf57;
        DAT_6fa09dcc = n;
        while (cnt < n) {  // do-while in retail; n>0 guard matches its entry test
            *(uint16_t*)(rec - 4) = 1;
            *(int32_t*)rec = *(int32_t*)(s + 0xc);
            uint8_t* t = s + 0x20;
            uint8_t* d = rec - 0x36;
            uint8_t c;
            do { c = *t++; *d++ = c; } while (c);
            if (DAT_6fa09cc0) { Ordinal_10167(); n = DAT_6fa09dcc; }
            t = s + 0x20;
            while (*t) t++;
            t++;
            while (*t) t++;
            uint8_t* fl = t + 1;  // flag char, then a string
            uint8_t b0 = *fl;
            int32_t lim = ((w >> 8 & 0x1f) / 5) + 1;
            bool ok = (w & 4) ? !(lim < (int32_t)(b0 - 0x33) || b0 < 0x33) : (int32_t)(b0 - 0x30) <= lim;
            if (ok) {
                uint8_t* u = t + 2;
                d = rec - 0x26;
                do { c = *u++; *d++ = c; } while (c);
                cnt++;
                rec += 0x3b;
            } else DAT_6fa09dcc = --n;
            s = fl;
            while (*s) s++;
            s++;
        }
    }
    d2_multi_sub_11f80();
}
