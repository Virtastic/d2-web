// Fog+0x17C00 (__fastcall): ensure the string in buf ends in '\\' if room remains (size = capacity incl. NUL).
#include <cstdint>
#include <cstring>
// 1.13c Fog+0x17C00
extern "C" void fog_sub_17c00(char* buf, uint32_t size) {
    if (!buf || !size) return;
    uint32_t n = (uint32_t)strlen(buf);
    if ((n == 0 || buf[n - 1] != '\\') && n + 1u < size) { buf[n] = '\\'; buf[n + 1] = 0; }
}
