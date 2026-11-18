// D2Client rva 0xBF6C0: load a UI cel file. Builds "DATA\GLOBAL\UI\<name>" in a 0x104-byte stack buffer (wsprintfA
// for the prefix, the import thunk 0xBE90 (stdcall 1, the buffer) for its length, then the name copied in after it)
// and returns what the cel loader 0x2B420 (EAX = path, one stack word 0) returns.
#include <cstdint>
#include <cstdio>
#include "d2client_callee.h"

// 1.13c D2Client+0xBF6C0 (ESI = name)
extern "C" uint32_t d2_client_bf6c0(const D2cSvc& v, const char* name) {
    char buf[0x104];
    std::snprintf(buf, sizeof buf, "%s\\UI\\", "DATA\\GLOBAL");   // wsprintfA through the IAT
    char* p = buf + d2c_call(v, 0xbe90, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)buf);
    for (; *name; ++name) *p++ = *name;
    *p = 0;
    return d2c_call(v, 0x2b420, (uint32_t)(uintptr_t)buf, 0, 0, 0, 0, 0);
}
