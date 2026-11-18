// D2Client rva 0xAA3E0: load the waygate panel cels once. Each of the three cached cel handles (unaligned module dwords at
// D2Client+0xFCDC1/5/9) still 0 is built as "DATA\GLOBAL\ui\menu\<name>" in a 0x104-byte stack buffer and loaded through
// 0x2B420 (EAX = path, one stack word 0). The tabs cel is the expansion one when thunk 0xBF8C (no args) and the module
// dword D2Client+0x119854 are both non-zero.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "d2client_callee.h"

static void aa3e0_load(const D2cSvc& v, uint8_t* slot, const char* name) {
    char buf[0x104];
    std::snprintf(buf, sizeof buf, "%s\\ui\\menu\\%s", "DATA\\GLOBAL", name);   // in-module sprintf
    const uint32_t h = d2c_call(v, 0x2b420, (uint32_t)(uintptr_t)buf, 0, 0, 0, 0, 0);
    std::memcpy(slot, &h, 4);
}

// 1.13c D2Client+0xAA3E0. cels = D2Client+0xFCDC1 (three dwords), expFlag = D2Client+0x119854.
extern "C" void d2_client_aa3e0(const D2cSvc& v, uint8_t* cels, const uint32_t* expFlag) {
    uint32_t h;
    std::memcpy(&h, cels, 4);
    if (!h) aa3e0_load(v, cels, "waygatebackground");
    std::memcpy(&h, cels + 4, 4);
    if (!h) aa3e0_load(v, cels + 4, d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0) && *expFlag ? "expwaygatetabs" : "waygatetabs");
    std::memcpy(&h, cels + 8, 4);
    if (!h) aa3e0_load(v, cels + 8, "waygateicons");
}
