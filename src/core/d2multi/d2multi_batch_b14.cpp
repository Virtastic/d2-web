// D2Multi batch 14: 8C80 text-lines draw, 8D80 string-id select, 8E70 button create, 9000 HKLM query, 90D0 resource release.
// Retail ordinals / registry / globals are weak seams (d2multi_*); not linked here.
#include <cstdint>
#include <cstring>
#define W __attribute__((weak))
extern void d2multi_ord10022(const char* line) W;      // draw one text line
extern const char16_t* d2multi_ord10003(int id) W;     // D2Lang string by id
extern void d2multi_wstrcpy(char16_t*, const char16_t*) W;
extern int d2multi_ord10227() W;
extern void* d2multi_ord10154(uint8_t, int, void*, void*) W;
extern void d2multi_ord10140(void*, int) W; extern void d2multi_ord10179(void*, int) W;
extern void d2multi_ord10080(void*, int) W; extern void d2multi_ord10172(void*, int) W;
extern void d2multi_ord10126(void*) W;                 // release resource
extern void* DAT_6fa09f38 W;
extern int d2multi_RegOpenKeyExA(uintptr_t hk, const char* sub, void** out) W;
extern int d2multi_RegQueryValueExA(void* k, const char* val, uint8_t* out, uint32_t* len) W;
extern int d2multi_RegCloseKey(void* k) W;

// 1.13c D2Multi+0x8C80  (EAX = string, param_2 = also draw second pass)
void d2multi_8C80(char* s, int twice) {
    if (!*s) { d2multi_ord10022(s); return; }   // retail: length 0 -> one call
    for (;;) {
        char* p = s; while (*p && *p != '\n') p++;
        char* nl = (*p == '\n') ? p : nullptr;
        if (nl) *nl = 0;
        d2multi_ord10022(s);
        if (twice) d2multi_ord10022(s);
        if (nl) *nl = '\n';
        s = nl ? nl + 1 : p;
        if (!*s) return;
    }
}

// 1.13c D2Multi+0x8D80  (EAX = selector, ESI = dest wide string)
void d2multi_8D80(unsigned sel, char16_t* dst) {
    static const int id[8] = {0xfab, 0xfaa, 0xfa9, 0xfa8, 0xfa7, 0x2771, 0x2772, 0x1507};
    d2multi_wstrcpy(dst, d2multi_ord10003(id[sel > 6 ? 7 : sel]));
}

// 1.13c D2Multi+0x8E70  (ESI = options struct)
void d2multi_8E70(uint8_t* o) {
    uint16_t f = *(uint16_t*)(o + 0x3c7);
    int kind = 5;
    if ((f & 4) && !(f & 8) && (d2multi_ord10227() || o[0x1ea] < 5)) kind = 1;
    DAT_6fa09f38 = d2multi_ord10154(o[0x1ea], kind, o + 0x39d, o + 0x3ad);
    if (DAT_6fa09f38) {
        d2multi_ord10140(DAT_6fa09f38, 0x42); d2multi_ord10179(DAT_6fa09f38, 0x23b);
        d2multi_ord10080(DAT_6fa09f38, 0); d2multi_ord10172(DAT_6fa09f38, 0);
    }
}

// 1.13c D2Multi+0x9000  (ECX = HKLM subkey, ESI = out length)
bool d2multi_9000(const char* sub, const char* val, uint8_t* out, uint32_t cap, uint32_t* len) {
    void* k; *len = 0;
    if (d2multi_RegOpenKeyExA(0x80000002, sub, &k)) return false;
    *len = cap;
    if (d2multi_RegQueryValueExA(k, val, out, len)) { d2multi_RegCloseKey(k); return false; }
    return d2multi_RegCloseKey(k) == 0;
}

// 1.13c D2Multi+0x90D0: release then null each global in retail order.
#define X(a) extern void* DAT_6fa##a W;
#define LIST X(09f48) X(09f4c) X(09f40) X(09f44) X(09f50) X(09f54) X(09f58) X(09f5c) X(09f64) X(09f68) X(09f6c) X(09f70) X(09f74) X(09f78) X(09f8c) X(09f98) X(09f9c) X(09ffc) X(0a000) X(0a004) X(0a008) X(0a00c) X(09fa0) X(0a010) X(0a014) X(0a048) X(0a04c) X(09f60) X(0a054) X(0a068) X(0a06c) X(0a084) X(0a088) X(09f90) X(09f94) X(0a070) X(0a074) X(0a078) X(0a080) X(0a07c) X(09fa4) X(09fac) X(09fb0) X(09fb4) X(09fb8) X(09fbc) X(09fc0) X(09fc4) X(09fc8) X(09fcc) X(09fd0) X(09ff4) X(09ff8)
LIST
extern void* DAT_6fa09fa8 W;
#undef X
#define X(a) if (DAT_6fa##a) { d2multi_ord10126(DAT_6fa##a); DAT_6fa##a = nullptr; }
void d2multi_90D0() {
    LIST
    // retail quirk: 9fa8 is released but 9fa4 (already handled above) is cleared, so 9fa8 stays stale
    if (DAT_6fa09fa8) { d2multi_ord10126(DAT_6fa09fa8); DAT_6fa09fa4 = nullptr; }
}
