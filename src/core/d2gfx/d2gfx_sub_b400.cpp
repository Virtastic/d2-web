#include <cstdint>

// 1.13c D2Gfx+0x0B400: load the video back-end DLL for mode ESI. dllNames = table 0x6FA90C18; *outModule is
// +0x1126C, *outTable +0x11268. Host seams: load = LoadLibraryA, initMgr = Fog 10233, getProc = GetProcAddress(mod,
// 0x2710) (nullptr when the export is missing), lastError = GetLastError, log = Fog 10029 / the wsprintfA into a
// 64-byte local that is never shown. The export's return is stored unchecked: a null table still returns 1.
typedef void* (*D2GfxB400Proc)();
struct D2GfxB400Host {
    void* (*load)(const char*);
    void  (*initMgr)();
    D2GfxB400Proc (*getProc)(void* mod);
    uint32_t (*lastError)();
    void  (*logMode)(const char* fmt, int32_t mode);
    void  (*logName)(const char* fmt, const char* name);
    void  (*logError)(const char* fmt, uint32_t err);
};
extern "C" int d2gfx_sub_b400(int32_t mode, const char* const* dllNames, void** outModule, void** outTable, const D2GfxB400Host* h) {
    if (!dllNames[mode]) { h->logMode("Unsupported video mode - %d", mode); return 0; }
    *outModule = h->load(dllNames[mode]);
    if (!*outModule) { h->logName("Can't find DLL: %s", dllNames[mode]); return 0; }
    h->initMgr();
    D2GfxB400Proc p = h->getProc(*outModule);
    if (!p) { h->logError("Error interfacing with Gfx DLL #%i", h->lastError()); return 0; }
    *outTable = p();
    return 1;
}
