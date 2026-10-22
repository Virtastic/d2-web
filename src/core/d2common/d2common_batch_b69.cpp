// b69 batch: plain-function recoveries. Stored 32-bit guest pointers are read as u32 and widened.
#include <cstdint>
#include <cstdlib>
#include <initializer_list>

namespace {
inline int32_t rd(const void* b, int off) { return *reinterpret_cast<const int32_t*>(static_cast<const char*>(b) + off); }
inline void* gp(const void* b, int off) { return reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<uint32_t>(rd(b, off)))); }
}

extern "C++" {
__attribute__((weak)) extern int32_t g_d2common_distTable_3f20[64]; // DAT_6fde3f20
__attribute__((weak)) int sub_681A0(int a, int b, int c);
__attribute__((weak)) int32_t* sub_69950(void* thisp, int a);
__attribute__((weak)) uint32_t sub_68B90(void* edi, void* p);
__attribute__((weak)) void sub_68800(void* a, void* b);
__attribute__((weak)) int sub_69090(void* edi, int x, int y, void* p);
__attribute__((weak)) int32_t* sub_68F00(void* edi, int32_t* list, int x, int y, uint32_t v, uint32_t u, void* p);
__attribute__((weak)) int D2Common_Ordinal_10265(int);
__attribute__((weak)) void D2Common_Ordinal_10024(const void* fmt, int);
__attribute__((weak)) extern const char g_d2common_fmt_dda728[]; // DAT_6fdda728
__attribute__((weak)) void sub_7BCD0();
__attribute__((weak)) void sub_7C0E0();
}

// 1.13c D2Common+0x68780
int sub_68780(int32_t* p1, int p2, int p3, int p4, int32_t* out) {
    int n = p1[0x2c / 4];
    int32_t* arr = static_cast<int32_t*>(gp(p1, 8));
    for (int i = 0; i < n; i++) {
        int r;
        if (static_cast<uint32_t>(arr[i]) != static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p1)) ? (r = sub_681A0(p2, p3, p4)) != 0 : false) {
            *out = arr[i];
            return r;
        }
    }
    *out = 0;
    return 0;
}

// 1.13c D2Common+0x69360 (thiscall; hidden input: edi = `edi`, explicit leading param)
void sub_69360(void* thisp, void* edi, char* p1, uint32_t p2, void* p3) {
    int32_t* ent = static_cast<int32_t*>(gp(edi, 0x4c));
    int32_t* tbl = sub_69950(thisp, rd(gp(edi, 0x58), 0x1d0));
    for (;; ent = static_cast<int32_t*>(gp(ent, 4))) {
        int code = 0;
        if (!ent) code = 0x2b1;
        else if (rd(gp(ent, 0x10), 0) == tbl[(p2 >> 0x14) & 0x3f]) {
            char c = static_cast<char>(p2 >> 8);
            if ((c != 0 && c != 4) ||
                sub_69090(edi, rd(p1, 8) + rd(edi, 0x34), rd(edi, 0x38) + rd(p1, 0xc), p3) != 0) {
                sub_68800(p3, p3);
                *reinterpret_cast<int32_t*>(p1 + 0x20) = rd(ent, 0xc);
                int32_t* sub = static_cast<int32_t*>(gp(ent, 0x10));
                *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(ent) + 0xc) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p1));
                if (!sub) code = 0x2cb;
                else {
                    if (rd(sub, 0x24) != 0) {
                        int v = rd(sub, 0x28);
                        uint32_t u = sub_68B90(edi, p3);
                        int32_t* r = sub_68F00(edi, reinterpret_cast<int32_t*>(reinterpret_cast<char*>(ent) + 0x14),
                                               rd(p1, 8) + rd(edi, 0x34), rd(edi, 0x38) + rd(p1, 0xc),
                                               (static_cast<uint32_t>(v) << 8) | p2, u, p3);
                        if (r) r[5] |= 8;
                    }
                    return;
                }
            } else return;
        }
        if (code) {
            D2Common_Ordinal_10024(g_d2common_fmt_dda728, D2Common_Ordinal_10265(code));
            _Exit(-1);
        }
    }
}

// 1.13c D2Common+0x3E2B0
int sub_3E2B0(uint32_t a, uint32_t b) {
    int dx = static_cast<int>(a & 0xffff) - static_cast<int>(b & 0xffff);
    if (dx < 0) dx = -dx;
    int dy = static_cast<int>(a >> 16) - static_cast<int>(b >> 16);
    if (dy < 0) dy = -dy;
    if (dx < 8 && dy < 8) {
        int v = g_d2common_distTable_3f20[dx + dy * 8];
        return v < 0 ? 0 : v + 1;
    }
    return dy < dx ? dy + dx * 2 : dx + dy * 2;
}

// 1.13c D2Common+0x7C370 (fastcall: ecx = unit)
void sub_7C370(char* u) {
    if (rd(u, 0x30) == 0 && (static_cast<uint32_t>(rd(u, 0x28)) & 0x100000) == 0) {
        char* pl = static_cast<char*>(gp(gp(u, 0x58), 0x1b4));
        sub_7C0E0();
        pl[0x45c] = static_cast<char>((~(pl[0x8c] << 1) & 2) | 5);
    }
    if (static_cast<uint8_t>(u[0x44]) > 1) sub_7BCD0();
}
