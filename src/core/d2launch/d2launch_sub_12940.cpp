#include <cstdint>
#include <cstdio>
__attribute__((weak)) extern int32_t Ordinal_422(const char*, const char*, int32_t, void*, int32_t);
__attribute__((weak)) extern void Ordinal_425(const char*, const char*, int32_t, void*);

// 1.13c D2Launch+0x12940 -- seed the "Aux Battle.net" registry value with 216.148.246.34 when unset; 1 if present, else 0.
extern "C" int32_t d2launch_sub_12940() {
    char buf[100];
    if (Ordinal_422("Diablo II", "Aux Battle.net", 0, buf, 100)) return 1;
    snprintf(buf, sizeof buf, "%d.%d.%d.%d", 0xd8, 0x94, 0xf6, 0x22);
    Ordinal_425("Diablo II", "Aux Battle.net", 0, buf);
    return 0;
}
