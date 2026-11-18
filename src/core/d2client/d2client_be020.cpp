// D2Client rva 0xBE020: load the loading-screen palette. Formats the .dat and .pl2 paths under DATA\GLOBAL into two
// 0x100-byte stack buffers (in-module sprintf) and hands both to the import thunk at 0xD396.
#include <cstdint>
#include <cstdio>
#include "d2client_callee.h"

// 1.13c D2Client+0xBE020
extern "C" void d2_client_be020(const D2cSvc& v) {
    char b[0x100], a[0x100];   // retail: B at esp, A at esp+0x100
    std::snprintf(a, sizeof a, "%s\\Palette\\Loading\\pal.dat", "DATA\\GLOBAL");
    std::snprintf(b, sizeof b, "%s\\Palette\\Loading\\pal.pl2", "DATA\\GLOBAL");
    d2c_call(v, 0xd396, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)a, (uint32_t)(uintptr_t)b);
}
