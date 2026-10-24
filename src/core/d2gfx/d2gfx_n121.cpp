// D2Gfx MSVC CRT glue, batch n121. Win32 / CRT calls go through host seam pointers.
#include <cstdint>

struct N121Host {
    void (*ExitProcess)(uint32_t);
    void* (*GetModuleHandleA)(const char*);
    void* (*GetProcAddress)(void*, const char*);
    void (*lock)(int);                 // D2Gfx+0x1A71
    void (*LeaveCriticalSection)(void*);
    void* locktable;                   // 0x6FA90048, 8 bytes per entry, cs at +0
    void (*unlock_8)();                // D2Gfx+0x13BD
    void (*doexit_tail)(uint32_t);
};

// 1.13c D2GFX+0x01384
extern "C" void d2gfx_crtExitProcess(N121Host* h, uint32_t code) {
    void* m = h->GetModuleHandleA("mscoree.dll");
    if (m) {
        typedef void (*Fn)(uint32_t);
        Fn f = (Fn)h->GetProcAddress(m, "CorExitProcess");
        if (f) f(code);
    }
    h->ExitProcess(code);
}

// 1.13c D2GFX+0x013B4
extern "C" void d2gfx_lock_8(N121Host* h) { h->lock(8); }

// 1.13c D2GFX+0x014F7
extern "C" void d2gfx_unlock(N121Host* h, int n);
extern "C" void d2gfx_doexit_tail(N121Host* h, uint32_t retcaller) {
    if (retcaller == 0) d2gfx_unlock(h, 8);   // retail: cmp [ebp+0x10],edi(0); je skip
}

// 1.13c D2GFX+0x0166E
extern "C" void d2gfx_sub_166e(N121Host* h) { h->unlock_8(); }

// 1.13c D2GFX+0x019BC
extern "C" void d2gfx_unlock(N121Host* h, int n) {
    h->LeaveCriticalSection((void*)((uint64_t*)h->locktable)[n]);
}

// SEH frame glue: model of the registration chain (fs:0) as a pointer.
struct N121Seh { void* prev; void* handler; int32_t trylevel; };

// 1.13c D2GFX+0x01B2C
extern "C" void d2gfx_SEH_prolog(N121Seh** fs0, N121Seh* rec, void* handler) {
    rec->prev = *fs0; rec->handler = handler; rec->trylevel = -1; *fs0 = rec;
}

// 1.13c D2GFX+0x01B67
extern "C" void d2gfx_SEH_epilog(N121Seh** fs0, N121Seh* rec) { *fs0 = (N121Seh*)rec->prev; }
