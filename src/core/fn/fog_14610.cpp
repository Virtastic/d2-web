// probe for Fog 0x14610 (sub_14610, symbol search path), over the HookEnv recorder. cdecl(dst): r[0] = dst (blob p, 2048 bytes), lg = r[16],
// sc = r[18]. The import stubs' answers are mirrored from three 16-byte blobs: exe = r[20] (the GetModuleFileNameA path, first byte 0 = failure),
// cwd = r[22] (GetCurrentDirectoryA, first byte 0 = failure), env = r[24] (GetEnvironmentVariableA, the one answer for all three names).
// Port: src/core/fog/fog_sub_14610.cpp through its host seams; the exe dir goes through fog_app_dir_143d0 as retail's 0x143D0 does.
// The only recorded call is the cdecl logger 0x190A0 (log 0x268EC, format).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
struct FogSymPathHost {
    bool (*exeDir)(char* out, unsigned cap);
    bool (*cwd)(char* out, unsigned cap);
    bool (*getenv)(const char* name, char* out, unsigned cap);
    void (*log)(const char* fmt, const char* arg);
};
void fog_symPathSetHost(const FogSymPathHost& h);
char* fog_buildSymbolPath(char* dst);
extern "C" int fog_app_dir_143d0(char* buf);
namespace {
HookEnv* H;
const char *gExe, *gCwd, *gEnv;
bool exeDir(char* out, unsigned) {
    out[0] = 0;
    if (!gExe[0]) return false;
    std::memcpy(out, gExe, 16);
    return fog_app_dir_143d0(out) != 0;
}
bool cwd(char* out, unsigned) { if (!gCwd[0]) return false; std::memcpy(out, gCwd, 16); return true; }
bool getenv_(const char*, char* out, unsigned) { if (!gEnv[0]) return false; std::memcpy(out, gEnv, 16); return true; }
void logp(const char* fmt, const char* arg) {
    // the recorder keeps the two words the hook is declared with (NST 2); the third word (dst) is not part of the record
    const bool ok = fmt[0] == 'U' && fmt[1] == 's' && fmt[2] == 'e';
    H->h(0x190A0, 0, 0, 0, 0, 0, 0xB20268ECu, ok ? 0xB20273A8u : 0xB2027380u, 0);
    (void)arg;
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14610(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[16], (const uint32_t*)(uintptr_t)r[18]);
    H = &e;
    gExe = (const char*)(uintptr_t)r[20]; gCwd = (const char*)(uintptr_t)r[22]; gEnv = (const char*)(uintptr_t)r[24];
    const FogSymPathHost hst = { exeDir, cwd, getenv_, logp };
    fog_symPathSetHost(hst);
    char* dst = (char*)(uintptr_t)r[0];
    fog_buildSymbolPath(dst);
    r[4] = 0;   // retmask 0: the retail result is dst or 0
}
