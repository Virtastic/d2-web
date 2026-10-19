#pragma once
#include <cstdint>

// Native pointers model callbacks; allocator sizes remain retail bytes (four per slot).
struct D2McpOnexitLkEnv {
    uint32_t (*msize)(void*);
    void** (*realloc)(void**, uint32_t);
    void** onexit_begin; // retail +0x125D0
    void** onexit_end;   // retail +0x125CC
};
extern D2McpOnexitLkEnv g_mcp_onexit;
extern "C" void* d2_mcp_onexit_lk(void* func, D2McpOnexitLkEnv* env);
extern "C" void* d2_mcp_onexit_167c(void* func);
extern "C" void d2_mcp_lock8_1030(void);
extern "C" void d2_mcp_unlock8_1039(void);
