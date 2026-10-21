// src/core/d2common/d2common_sub_8c7a.cpp -- 1.13c D2Common.dll CRT security-failure reporter.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>

// +0x9EFA40 DAT_6fdefa40: optional user handler for the failure; retail calls it instead of reporting.
static void (*g_pfnSecurityHandler)() = nullptr;

// Last report text retail would hand to MessageBoxA (host UI, a seam: kept, not shown).
static char g_szSecurityReport[0x200];

// 1.13c D2Common+0x08C7A
// nCode 1 = buffer overrun, anything else = unknown failure. With no handler installed, builds the
// "<text>\n\nProgram: <module path, tail-truncated with "..." to fit 60 chars>\n\n<code text>" report
// (the box itself is a host seam) and then, as always, __exit(3).
extern "C" void D2Common_8C7A_SecurityFailure(int32_t nCode)
{
    if (g_pfnSecurityHandler == nullptr) {
        const char* pszText = nCode == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
        char szProgram[0x104] = "<program name unknown>"; // no module file name under wasm
        char* pszName = szProgram;
        size_t nLen = strlen(pszName);
        if (nLen + 11 > 0x3C) {
            pszName = szProgram + nLen - 0x39;
            memcpy(pszName, "...", 3);
        }
        snprintf(g_szSecurityReport, sizeof g_szSecurityReport, "%s\n\nProgram: %s\n\n", pszText, pszName);
    } else {
        g_pfnSecurityHandler();
    }
    exit(3);
}
