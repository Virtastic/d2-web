// src/core/fog/fog_sub_164a0.cpp -- 1.13c Fog.dll sub_164A0 (dbghelp loader for stack crawls), from
// build-wasm/ghidra-fog/decomp/0x164A0.c. Host seams stand in for Win32; default: no dbghelp.
#include <cstdint>

namespace {
constexpr int kN = 13;
const char* const kSyms[kN] = {"SymCleanup", "SymFunctionTableAccess", "SymGetLineFromAddr",
    "SymGetModuleBase", "SymGetModuleInfo", "SymGetOptions", "SymGetSymFromAddr", "SymInitialize",
    "SymSetOptions", "SymLoadModule", "SymUnloadModule", "StackWalk", "UnDecorateSymbolName"};
// SymGetLineFromAddr (idx 2) is optional in retail; every other export is required.
void* (*g_load)(const char*) = [](const char*) -> void* { return nullptr; };
void* (*g_proc)(void*, const char*) = [](void*, const char*) -> void* { return nullptr; };
void (*g_log)(const char*) = [](const char*) {};
void (*g_start)() = []() {};  // 2 auto-reset events + worker thread (FUN_65d50)
bool g_ready;
void* g_fn[kN];
}  // namespace

// 1.13c Fog+0x164A0
uint32_t fog_sub_164a0() {
    if (g_ready) return 1;
    void* h = g_load(".\\dbghelp.dll");
    if (!h) h = g_load("dbghelp.dll");
    if (!h) h = g_load("imagehlp.dll");
    if (!h) { g_log("Missing imagehlp.dll.  Can't generate better stack crawls!"); return 0; }
    bool ok = true;
    for (int i = 0; i < kN; i++) { g_fn[i] = g_proc(h, kSyms[i]); if (!g_fn[i] && i != 2) ok = false; }
    if (!ok) { g_log("Missing functions in imagehlp.dll.  Can't generate better stack crawls!"); return 0; }
    g_ready = true;
    g_log("imagehlp.dll loaded for better stack crawls.");
    g_start();
    return 1;
}
