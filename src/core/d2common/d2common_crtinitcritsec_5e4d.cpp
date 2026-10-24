// src/core/d2common/d2common_crtinitcritsec_5e4d.cpp -- 1.13c D2Common.dll ___crtInitCritSecAndSpinCount (VC7.1 CRT).
// Only the kernel32 lookup (Win32 boundary) is a seam; the fallback callee is the real 0x5E3D row.
#include <cstdint>
typedef void* LPCRITICAL_SECTION;

extern "C" int32_t D2Common_5E3D_InitCriticalSection(LPCRITICAL_SECTION section, uint32_t spinCount);

typedef int32_t (*D2CommonInitCsFn)(LPCRITICAL_SECTION cs, uint32_t spin);

struct D2CommonCritInitEnv {
    const int32_t* osFlag;     // DAT_6fdef5a4 (1 = no kernel32 lookup)
    void* (*lookup)();         // GetProcAddress(GetModuleHandleA("kernel32.dll"),"InitializeCriticalSectionAndSpinCount"), 0 if absent
};

static D2CommonInitCsFn g_init_cs;   // DAT_6fdef890

// 1.13c D2Common+0x5E4D (103 B): resolve once (OS export unless flag==1, else 0x5E3D), cache, call.
// 1.13c D2Common+0x05E4D
extern "C" void D2Common_5E4D_CrtInitCritSecAndSpinCount(LPCRITICAL_SECTION cs, uint32_t spin, const D2CommonCritInitEnv* env)
{
    if (!g_init_cs) {
        void* p = (*env->osFlag != 1) ? env->lookup() : nullptr;
        g_init_cs = p ? (D2CommonInitCsFn)p : D2Common_5E3D_InitCriticalSection;
    }
    g_init_cs(cs, spin);
}
