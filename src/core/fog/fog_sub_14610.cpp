// 1.13c Fog+0x14610 -- build the ';'-joined symbol search path: exe dir, cwd (if different),
// _NT_SYMBOL_PATH, _NT_ALTERNATE_SYMBOL_PATH, SYSTEMROOT; trailing ';' trimmed; logs the result.
#include <cstring>
#include <strings.h>

namespace {
// Host seams: exe dir (Fog+0x143D0), GetCurrentDirectoryA, GetEnvironmentVariableA, Fog log (0x190A0).
struct FogSymPathHost {
    bool (*exeDir)(char* out, unsigned cap);
    bool (*cwd)(char* out, unsigned cap);
    bool (*getenv)(const char* name, char* out, unsigned cap);
    void (*log)(const char* fmt, const char* arg);
};
FogSymPathHost g_host = {};
void add(char* dst, const char* s) { std::strcat(dst, s); std::strcat(dst, ";"); }
}

void fog_symPathSetHost(const FogSymPathHost& h) { g_host = h; }

// dst needs >= 0x800 bytes. Returns dst, or nullptr when empty.
char* fog_buildSymbolPath(char* dst) {
    char tmp[512], cwd[512];
    dst[0] = 0;
    bool haveExe = g_host.exeDir && g_host.exeDir(tmp, 512);
    if (haveExe) add(dst, tmp); else tmp[0] = 0;
    if (g_host.cwd && g_host.cwd(cwd, 512) && !(tmp[0] && !strcasecmp(tmp, cwd))) add(dst, cwd);
    static const char* const vars[] = {"_NT_SYMBOL_PATH", "_NT_ALTERNATE_SYMBOL_PATH", "SYSTEMROOT"};
    for (const char* v : vars)
        if (g_host.getenv && g_host.getenv(v, tmp, 512)) add(dst, tmp);
    size_t n = std::strlen(dst);
    if (!n) { if (g_host.log) g_host.log("Unable to generate symbols search path", nullptr); return nullptr; }
    dst[n - 1] = 0;
    if (g_host.log) g_host.log("User defined symbols path: %s", dst);
    return dst;
}
