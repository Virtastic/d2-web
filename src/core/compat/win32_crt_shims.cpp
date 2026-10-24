// Win32 callees of the recovered MSVC CRT rows, as weak libc-backed externs so the link resolves.
// Plain types, no windows.h. Weak: a real definition elsewhere in src/ wins.
// ponytail: heaps are the process malloc heap (HeapCreate returns a token), no per-heap state.
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define W extern "C" __attribute__((weak))

W uint32_t GetACP(void) { return 1252; }
W uint32_t GetOEMCP(void) { return 1252; }

static char g_heap_token;
W void* HeapCreate(uint32_t, uintptr_t, uintptr_t) { return &g_heap_token; }
W int32_t HeapDestroy(void*) { return 1; }
W int32_t HeapFree(void*, uint32_t, void* p) { free(p); return 1; }
W int32_t VirtualFree(void* p, uintptr_t, uint32_t) { free(p); return 1; }

W void RtlUnwind(void*, void*, void*, void*) {}  // no SEH frames in wasm

W uint32_t GetFileType(void* h) {
    uintptr_t v = (uintptr_t)h;
    return v <= 2 ? 2u /*FILE_TYPE_CHAR*/ : 1u /*FILE_TYPE_DISK*/;
}
W uint32_t SetHandleCount(uint32_t n) { return n; }

W void GetStartupInfoA(void* si) {
    if (!si) return;
    memset(si, 0, 68);
    *(uint32_t*)si = 68;  // cb
}

static const char g_env_a[2] = {0, 0};
static const uint16_t g_env_w[2] = {0, 0};
W void* GetEnvironmentStringsW(void) { return (void*)g_env_w; }
W char* GetEnvironmentStrings(void) { return (char*)g_env_a; }
W char* GetEnvironmentStringsA(void) { return (char*)g_env_a; }
W int32_t FreeEnvironmentStringsA(void*) { return 1; }
W int32_t FreeEnvironmentStringsW(void*) { return 1; }

// Latin-1 narrowing: code units above 0xFF become '?'. Returns bytes written, or needed when cb==0.
W int32_t WideCharToMultiByte(uint32_t, uint32_t, const uint16_t* w, int32_t wn, char* out, int32_t cb,
                              const char*, int32_t* used_default) {
    if (used_default) *used_default = 0;
    if (!w) return 0;
    if (wn < 0) { wn = 0; while (w[wn]) ++wn; ++wn; }
    if (cb == 0) return wn;
    if (cb < wn) return 0;
    for (int32_t i = 0; i < wn; ++i) {
        out[i] = w[i] > 0xFF ? '?' : (char)w[i];
        if (w[i] > 0xFF && used_default) *used_default = 1;
    }
    return wn;
}
