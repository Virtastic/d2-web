// Fog+0x17BD0 (__fastcall): exe directory into buf (size = capacity): module path, NUL-terminated, cut after last '\\'.
#include <cstdint>
#include <cstring>
// 1.13c Fog+0x17BD0
extern "C" void fog_sub_17bd0(char* buf, uint32_t size) {
    if (!buf || !size) return;
    buf[0] = 0; // no Win32 module path under wasm: GetModuleFileNameA(NULL) yields ""
    buf[size - 1] = 0;
    char* p = strrchr(buf, '\\'); // Ordinal_572 = strrchr
    if (p) p[1] = 0;
}
