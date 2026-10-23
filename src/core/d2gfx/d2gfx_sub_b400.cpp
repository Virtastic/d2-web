#include <cstdint>
#include <cstdio>

// 1.13c D2Gfx+0xB400: load the video back-end DLL for mode ESI. dllNames = table
// 0x6FA90C18. Host seam: load(name) -> module or 0; init(module) runs GetProcAddress(0x2710) and returns the
// interface table (stored at +0x1126C by the caller); *-1 sentinel (init == nullptr) = proc missing.
struct D2GfxB400Host {
    void* (*load)(const char*);
    void  (*initMgr)();                       // Ordinal_10233 (Fog)
    void* (*getTable)(void* mod);             // GetProcAddress(mod, 0x2710)() ; nullptr when the export is missing
    void  (*log)(const char*, long);          // Ordinal_10029 / wsprintfA messages, never raised to the user
};
extern "C" int d2gfx_sub_b400(int mode, const char* const* dllNames, void** outModule, void** outTable, const D2GfxB400Host* h) {
    if (!dllNames[mode]) { h->log("Unsupported video mode - %d", mode); return 0; }
    *outModule = h->load(dllNames[mode]);
    if (!*outModule) { h->log("Can't find DLL: %s", (long)dllNames[mode]); return 0; }
    h->initMgr();
    void* t = h->getTable(*outModule);
    if (!t) { h->log("Error interfacing with Gfx DLL #%i", 0); return 0; }
    *outTable = t;
    return 1;
}
