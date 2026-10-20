// src/core/realm/mcp_crt_lock_init.cpp -- two 1.13c D2MCPClient.dll CRT lock-initialisation rows
// reimplemented from build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FA20000.
// The cached implementation pointer (+0x2ACB0), the OS-platform flag (+0x2A9E4) and the KERNEL32
// resolution calls are module state / host seams after the retail arguments, which keep their order
// (src/core/realm/mcp_crt_mem.cpp pattern). The D2Gfx/D2Direct3D/D2Glide copies of this VC7.1 pair
// are byte-identical; __mtinitlocknum (0x1503) is the live caller inside this module.
#include <cstdint>
#include <emscripten.h>

// GetModuleHandleA / GetProcAddress, InitializeCriticalSection and
// InitializeCriticalSectionAndSpinCount are host seams.
struct D2McpInitCritSecEnv {
    uint32_t os_platform;                                                  // +0x2A9E4 DAT_6fa2a9e4
    void*   (*get_module_handle)(const char* name);                        // GetModuleHandleA
    void*   (*get_proc_address)(void* module, const char* name);           // GetProcAddress
    void    (*initialize_critical_section)(void* cs);                      // InitializeCriticalSection
    int32_t (*initialize_critical_section_and_spin_count)(void* cs, uint32_t spin);
};

typedef int32_t (*D2McpCritSecInitFn)(void* cs, uint32_t spin, const D2McpInitCritSecEnv* env);

// +0x2ACB0 DAT_6fa2acb0: the implementation chosen on the first call, cached process-wide.
static D2McpCritSecInitFn g_mcp_crit_sec_impl;

// 1.13c D2MCPClient+0x0279D (16 B): VC7.1 `___crtInitCritSecNoSpinCount@8(lpCS, spin)`. Enter the
// critical section through the InitializeCriticalSection import and answer the constant 1; the spin
// argument is dead, and a failure would raise rather than return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_crt_init_crit_sec_no_spin_count(
        void* cs, uint32_t spin, const D2McpInitCritSecEnv* env) {
    (void)spin;
    env->initialize_critical_section(cs);
    return 1;
}

// 1.13c D2MCPClient+0x027AD (103 B, callers 0x01450/0x01503/0x01CFC):
// `___crtInitCritSecAndSpinCount(lpCS, spin)`. With no cached implementation, and unless the OS flag
// says version 1, resolve `InitializeCriticalSectionAndSpinCount` from kernel32.dll and cache it; if
// the module or the export is missing (or version 1), cache and call the built-in
// ___crtInitCritSecNoSpinCount (0x279D). Call the chosen implementation and return its answer.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_crt_init_crit_sec_and_spin_count(
        void* cs, uint32_t spin, const D2McpInitCritSecEnv* env) {
    D2McpCritSecInitFn impl = g_mcp_crit_sec_impl;
    if (impl == nullptr) {
        if (env->os_platform != 1) {
            void* kernel32 = env->get_module_handle("kernel32.dll");
            if (kernel32 != nullptr) {
                impl = (D2McpCritSecInitFn)env->get_proc_address(
                        kernel32, "InitializeCriticalSectionAndSpinCount");
                if (impl != nullptr) {
                    g_mcp_crit_sec_impl = impl;
                }
            }
        }
        if (impl == nullptr) {
            impl = d2_mcp_crt_init_crit_sec_no_spin_count;
            g_mcp_crit_sec_impl = impl;
        }
    }
    return impl(cs, spin, env);
}

// Test seam: forget the process-wide cache (the retail cold state, DAT_6fa2acb0 == 0).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_crt_init_crit_sec_cache_reset(void) {
    g_mcp_crit_sec_impl = nullptr;
}
