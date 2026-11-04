// src/core/fog/fog_sub_164a0.cpp -- 1.13c Fog.dll sub_164A0 (dbghelp loader for stack crawls), from
// build-wasm/ghidra-fog/decomp/0x164A0.c. Host seams stand in for Win32 (module load, events, thread, atexit, log).
// The module state is the retail block at DAT_6ffa6e20 (18 dwords) plus the module handle at DAT_6ff82ae8, so a vector
// can pin both and read them back.
#include <cstdint>

struct Fog164a0Api {
    void (*init_cs)();                                 // InitializeCriticalSection(&DAT_6ff82acc)
    void (*at_exit)(uint32_t handler);                 // Fog+0x2D1D(0x6ff65ee0)
    uint32_t (*load)(const char* name);                // LoadLibraryA
    uint32_t (*proc)(uint32_t module, const char* n);  // GetProcAddress
    void (*log)(const char* msg);                      // Fog+0x19100
    uint32_t (*create_event)();                        // CreateEventA(0, 0, 0, 0)
    uint32_t (*create_thread)();                       // CreateThread(0, 0, Fog+0x15D50, 0, 0, &id)
};

namespace {
// st[] indices = (rva - 0x56E20) / 4
enum { kCleanup, kFnTable, kLine, kModBase, kModInfo, kGetOpts, kSymFromAddr, kInit, kLoadMod, kUnloadMod, kSetOpts,
       kStackWalk, kUndecorate, kFlag, kThread, kWake, kDone };
}

// 1.13c Fog+0x164A0
extern "C" uint32_t fog_sub_164a0(const Fog164a0Api* api, uint32_t* st, uint32_t* hmod) {
    if (st[kFlag] != 0) return 1;
    api->init_cs();
    api->at_exit(0x6ff65ee0);
    *hmod = api->load(".\\dbghelp.dll");
    if (*hmod == 0) {
        *hmod = api->load("dbghelp.dll");
        if (*hmod == 0) {
            *hmod = api->load("imagehlp.dll");
            if (*hmod == 0) { api->log("Missing imagehlp.dll.  Can't generate better stack crawls!"); return 0; }
        }
    }
    st[kCleanup] = api->proc(*hmod, "SymCleanup");
    st[kFnTable] = api->proc(*hmod, "SymFunctionTableAccess");
    st[kLine] = api->proc(*hmod, "SymGetLineFromAddr");        // optional: not part of the success test
    st[kModBase] = api->proc(*hmod, "SymGetModuleBase");
    st[kModInfo] = api->proc(*hmod, "SymGetModuleInfo");
    st[kGetOpts] = api->proc(*hmod, "SymGetOptions");
    st[kSymFromAddr] = api->proc(*hmod, "SymGetSymFromAddr");
    st[kInit] = api->proc(*hmod, "SymInitialize");
    st[kSetOpts] = api->proc(*hmod, "SymSetOptions");
    st[kLoadMod] = api->proc(*hmod, "SymLoadModule");
    st[kUnloadMod] = api->proc(*hmod, "SymUnloadModule");
    st[kStackWalk] = api->proc(*hmod, "StackWalk");
    st[kUndecorate] = api->proc(*hmod, "UnDecorateSymbolName");
    if (st[kCleanup] && st[kFnTable] && st[kModBase] && st[kModInfo] && st[kGetOpts] && st[kSymFromAddr] && st[kInit] &&
        st[kSetOpts] && st[kStackWalk] && st[kUndecorate] && st[kLoadMod] && st[kUnloadMod]) {
        st[kFlag] = 1;
        api->log("imagehlp.dll loaded for better stack crawls.");
        st[kWake] = api->create_event();
        st[kDone] = api->create_event();
        st[kThread] = api->create_thread();
        return 1;
    }
    api->log("Missing functions in imagehlp.dll.  Can't generate better stack crawls!");
    return 0;
}
