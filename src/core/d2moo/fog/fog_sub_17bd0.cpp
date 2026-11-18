// Fog+0x17BD0 (__fastcall): exe directory into buf (size = capacity): module path, NUL-terminated, cut after last '\\'.
#include <cstdint>
#include <cstring>
// GetModuleFileNameA(NULL, buf, size): the host seam. Unset (the wasm build has no Win32 module path) it yields "".
extern "C" uint32_t (*fog_17bd0_getmodule)(uint32_t hModule, char* buf, uint32_t size) = nullptr;
// 1.13c Fog+0x17BD0
extern "C" void fog_sub_17bd0(char* buf, uint32_t size) {
    if (!buf || !size) return;
    if (fog_17bd0_getmodule) fog_17bd0_getmodule(0, buf, size);
    else buf[0] = 0;
    buf[size - 1] = 0;
    char* p = strrchr(buf, '\\'); // Ordinal_572 = strrchr
    if (p) p[1] = 0;
}
