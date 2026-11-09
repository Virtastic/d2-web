// Link definitions for the retail callees and data storm_batch_e6bd.cpp references but no port defines yet.
// Every definition is weak: a real port of the same name replaces it at link time. Without these, any object that
// pulls storm_batch_e6bd.o into a link (its probes, its callers) fails with undefined symbols.
// The wasm build is single-threaded: critical sections and events do nothing, the interlocked increment is plain.
#include <cstdint>
typedef uint16_t wchar16;
#define WEAK __attribute__((weak))

extern "C" {
WEAK uint32_t g_lastErr_6fc45388;
WEAK int32_t g_locW_6fc42904, g_locA_6fc42908, g_acp_6fc42788;
WEAK void* g_cs_6fc45a24;
WEAK void* g_cs_6fc45940;
WEAK int32_t g_init_6fc45834, g_self_6fc41b4c, g_base_6fc45830, g_head_6fc45a6c;
WEAK uint32_t g_ver_6fc45344, g_event_6fc4584c;
WEAK int32_t g_cnt_6fc41a78, g_cnt_6fc41a98;
WEAK void *g_cb_6fc11c70, *g_cb_6fc11c30;
WEAK int (*g_cmp_6fc1fcf0)(const void*, const void*);

struct StormLocSeam {  // FN-probe seam (src/core/fn/storm_e6bd.cpp, storm_e7ed.cpp): answers the Win locale imports
    int (*getLocaleInfoW)(uint32_t, uint32_t, wchar16*, int);
    int (*getLocaleInfoA)(uint32_t, uint32_t, char*, int);
    int (*multiByteToWideChar)(uint32_t, uint32_t, const char*, int, wchar16*, int);
    int (*wideCharToMultiByte)(uint32_t, uint32_t, const wchar16*, int, char*, int, const char*, int*);
    uint32_t (*getLastError)();
};
WEAK const StormLocSeam* g_storm_locseam = nullptr;
WEAK void Storm_Sub16BD0(char* out, int32_t) { if (out) out[0] = 0; }   // not yet ported (install dir)
static uint32_t s_lastError;
WEAK uint32_t Win_GetLastError() { if (g_storm_locseam) return g_storm_locseam->getLastError(); return s_lastError; }
WEAK void Win_SetLastError(uint32_t e) { s_lastError = e; }
// No locale provider: report "not implemented" (0x78), which the callers treat as "use the A/W fallback".
WEAK int Win_GetLocaleInfoW(uint32_t a, uint32_t b, wchar16* c, int d) { if (g_storm_locseam) return g_storm_locseam->getLocaleInfoW(a, b, c, d); s_lastError = 0x78; return 0; }
WEAK int Win_GetLocaleInfoA(uint32_t a, uint32_t b, char* c, int d) { if (g_storm_locseam) return g_storm_locseam->getLocaleInfoA(a, b, c, d); s_lastError = 0x78; return 0; }
WEAK int Win_MultiByteToWideChar(uint32_t a, uint32_t b, const char* c, int d, wchar16* e, int f) { if (g_storm_locseam) return g_storm_locseam->multiByteToWideChar(a, b, c, d, e, f); s_lastError = 0x78; return 0; }
WEAK int Win_WideCharToMultiByte(uint32_t a, uint32_t b, const wchar16* c, int d, char* e, int f, const char* g, int* h) { if (g_storm_locseam) return g_storm_locseam->wideCharToMultiByte(a, b, c, d, e, f, g, h); s_lastError = 0x78; return 0; }
WEAK void Win_EnterCS(void*) {}
WEAK void Win_LeaveCS(void*) {}
WEAK void Win_SetEvent(uint32_t) {}
WEAK int32_t Win_InterlockedIncrement(int32_t* p) { return ++*p; }

// Retail Storm functions not yet ported.
WEAK void (*g_storm_grow_hook)(uint32_t*, uint32_t) = nullptr;   // test seam: a probe records the grow request (retail's 0x2950) instead of reallocating
WEAK void Storm_Grow(uint32_t* l, uint32_t n) { if (g_storm_grow_hook) g_storm_grow_hook(l, n); }
WEAK void Storm_Qsort(uint32_t, uint32_t, uint32_t, int (*)(const void*, const void*)) {}
WEAK int Storm_Sub1FD50(float*) { return 0; }
WEAK void Storm_Sub1FEA0(int) {}
WEAK void Storm_Sub207F0() {}
WEAK void Storm_Sub20A90(uint32_t, uint32_t, uint32_t) {}
WEAK void Storm_Sub20C90(uint32_t*, uint32_t, uint32_t, uint32_t, float*, uint32_t, uint32_t) {}
WEAK uint32_t* Storm_Sub2690(uint32_t, uint32_t) { return nullptr; }
WEAK void Storm_SubF2E0(void*, void*, void*, void*, void*, void*) {}
WEAK void Storm_Sub12F40(uint32_t, int8_t, uint32_t*, uint32_t) {}
// kernel32 seams of storm_batch_1efc0.cpp: ours never opens a retail archive.
WEAK uint32_t StormGetFileAttributes(const char*) { return 0xFFFFFFFFu; }
WEAK void* StormCreateFile(const char*, uint32_t, uint32_t, uint32_t, uint32_t) { return (void*)(intptr_t)-1; }
WEAK uint32_t StormSetFilePointer(void*, int32_t, int32_t*, uint32_t) { return 0xFFFFFFFFu; }
WEAK uint32_t StormGetFileSize(void*) { return 0xFFFFFFFFu; }
WEAK int32_t StormReadFile(void*, void*, uint32_t, uint32_t*) { return 0; }
WEAK void* StormCreateEvent(void) { return nullptr; }
WEAK void StormWaitForSingleObject(void*, uint32_t) {}
WEAK void StormCloseHandle(void*) {}
WEAK uint32_t g_storm_imp_w3;   // fourth stack word of CreateRectRgn / CombineRgn (storm_walker probe)
}

// Retail seams of storm_batch_1efc0.cpp that no port defines yet (C++ linkage, like their declarations); a real port replaces them.
__attribute__((weak)) uint32_t* Storm_Sub4290(uint32_t* anchor, uint32_t, uint32_t) { return {}; }
__attribute__((weak)) int32_t Storm_SubB960(void* p, const char* tag, int32_t line) { return {}; }
__attribute__((weak)) void Storm_Sub179F0(int32_t h) {}
__attribute__((weak)) void* Storm_Sub40D0(uint32_t* anchor, uint32_t, uint32_t) { return {}; }
__attribute__((weak)) uint32_t Storm_Sub2B080(uint8_t* dst, const uint32_t* src, uint32_t n) { return {}; }
__attribute__((weak)) char* Storm_Sub2B2D0(const char* s, int32_t c) { return {}; }
__attribute__((weak)) int32_t Storm_Sub16AC0(void* archive, void* name) { return {}; }
__attribute__((weak)) void Storm_Sub19560(void) {}
__attribute__((weak)) int32_t Storm_Sub18E80(uint32_t pos, void* buf, uint32_t len) { return {}; }
__attribute__((weak)) uint32_t Storm_Sub12C80(void* dst, const void* src, uint32_t n) { return {}; }
__attribute__((weak)) void Storm_Sub16E20(int32_t* rec) {}
__attribute__((weak)) uint32_t Storm_CryptTable(uint32_t index) { return {}; }
__attribute__((weak)) int32_t Storm_Sub16750(void) { return {}; }
__attribute__((weak)) void Storm_Sub15F30(uint32_t key, uint32_t* table) {}
__attribute__((weak)) void Storm_Sub15E50(uint32_t count) {}
__attribute__((weak)) void Storm_Sub16CB0(uint32_t* rec) {}
__attribute__((weak)) void Storm_Sub19F60(uint8_t* rec, void* blockTable, int32_t count) {}
__attribute__((weak)) void Storm_Sub181F0(uint8_t* rec) {}
__attribute__((weak)) int32_t Storm_Sub19DF0(uint32_t magic, int32_t* size) { return {}; }
__attribute__((weak)) int32_t Storm_ProviderCreate(void* iface, void* desc, void** out, uint32_t) { return {}; }
__attribute__((weak)) void Storm_ProviderPlay(void* iface, uint32_t) {}
__attribute__((weak)) void Storm_ProviderSetVolume(void* iface, uint32_t v) {}
__attribute__((weak)) void Storm_ProviderSetPan(void* iface, uint32_t v) {}
__attribute__((weak)) const uint8_t* Storm_ScdTemplate(uint32_t offset) { return {}; }
__attribute__((weak)) uint8_t Storm_Byte369C0(uint32_t i) { return {}; }
__attribute__((weak)) const uint8_t* Storm_Ptr36938(void) { return {}; }
__attribute__((weak)) uint32_t Storm_Dword3692C(void) { return {}; }
__attribute__((weak)) uint8_t Storm_Byte36930(void) { return {}; }
__attribute__((weak)) void Storm_Sub2AE00(uint8_t* dst, const uint8_t* table, uint32_t n) {}
__attribute__((weak)) void* Storm_Sub6460(const void*, const char*) { return {}; }
__attribute__((weak)) void Storm_Sub37E80(uint32_t*, uint32_t, uint32_t*) {}   // 0x37E80 with EAX = state; the port in storm_batch_36d40.cpp takes (code, out)
// Walker FN entry points whose ports do not exist yet (rows 0x1CF00 / 0x1D2E0 differ until they do); 0x1D6B0 links through the same probe file.
extern "C" {
WEAK void storm_1cf00_fnrun(int32_t*, void*, int32_t, uint32_t, int, void*, uint32_t*) {}
WEAK void storm_1d2e0_fnrun(uintptr_t, uintptr_t, uintptr_t, uint32_t*) {}
}
