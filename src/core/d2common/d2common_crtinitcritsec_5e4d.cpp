// src/core/d2common/d2common_crtinitcritsec_5e4d.cpp -- 1.13c D2Common.dll ___crtInitCritSecAndSpinCount (VC7.1 CRT).
// OS lookups are host seams, passed in (mcp_crt_lock_init.cpp pattern).
#include <cstdint>

struct D2CommonCritInitEnv {
    void  (**cached)(void* cs, uint32_t spin);   // DAT_6fdef890
    const int32_t* osFlag;                       // DAT_6fdef5a4 (1 = no kernel32 lookup)
    void* (*lookup)();                           // GetProcAddress(GetModuleHandleA("kernel32.dll"),"InitializeCriticalSectionAndSpinCount"), 0 if absent
    void  (*noSpin)(void* cs, uint32_t spin);    // ___crtInitCritSecNoSpinCount@8 (0x5E3D)
};

// 1.13c D2Common+0x5E4D (103 B): resolve the init routine once (OS export unless flag==1, else the
// no-spin fallback), cache it, then call it with (cs, spin).
extern "C" void D2Common_5E4D_CrtInitCritSecAndSpinCount(void* cs, uint32_t spin, const D2CommonCritInitEnv* env)
{
    if (!*env->cached) {
        void* p = (*env->osFlag != 1) ? env->lookup() : nullptr;
        *env->cached = p ? (void (*)(void*, uint32_t))p : env->noSpin;
    }
    (*env->cached)(cs, spin);
}
