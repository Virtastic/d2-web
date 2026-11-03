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

static uint32_t s_lastError;
WEAK uint32_t Win_GetLastError() { return s_lastError; }
WEAK void Win_SetLastError(uint32_t e) { s_lastError = e; }
// No locale provider: report "not implemented" (0x78), which the callers treat as "use the A/W fallback".
WEAK int Win_GetLocaleInfoW(uint32_t, uint32_t, wchar16*, int) { s_lastError = 0x78; return 0; }
WEAK int Win_GetLocaleInfoA(uint32_t, uint32_t, char*, int) { s_lastError = 0x78; return 0; }
WEAK int Win_MultiByteToWideChar(uint32_t, uint32_t, const char*, int, wchar16*, int) { s_lastError = 0x78; return 0; }
WEAK int Win_WideCharToMultiByte(uint32_t, uint32_t, const wchar16*, int, char*, int, const char*, int*) { s_lastError = 0x78; return 0; }
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
}
