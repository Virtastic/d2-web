// 1.13c Storm+0x2FB30 -- SErrReportNamedResourceLeak: the "handle never released" debug report.
// stdcall (name, detail), ret 8. The first call latches the "Internal"/"Debug Memory" option through SRegLoadValue
// (0x2C1D0) into the option word; with the option on, the name (and "(detail)" when the detail is non-empty) is formatted
// into a 256-byte buffer and handed to SErrDisplayError (0x2F590, code 0x8510007F, line -3, recoverable, exit 1) unless the
// debug-output switch is set, in which case it goes to OutputDebugStringA under "Storm Error : handle never released -- ".
// The callees and the three module words come in through Storm2FB30Env so the FN probe can pin and record them.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <Windows.h>

struct Storm2FB30Env {
    uint32_t* latch;    // 0x45A74  first-call latch
    uint32_t* on;       // 0x45A70  "Debug Memory" option (SRegLoadValue writes it)
    uint32_t* dbgOut;   // 0x43138  non-zero routes the report to OutputDebugStringA
    void (*regLoad)(const char* key, const char* value, uint32_t flags, uint32_t* out);                    // 0x2C1D0
    void (*display)(uint32_t code, const char* text, int32_t line, void* ctx, uint32_t recov, uint32_t exitc); // 0x2F590
    void (*debugOut)(const char* s);                                                                         // OutputDebugStringA
};

// 1.13c Storm+0x2FB30
extern "C" void Storm13c_2FB30(const Storm2FB30Env* e, const char* name, const char* detail) {
    if (*e->latch == 0) {
        *e->latch = 1;
        if (e->regLoad) e->regLoad("Internal", "Debug Memory", 0, e->on);
    }
    if (*e->on == 0) return;
    char msg[0x100];
    if (detail == nullptr || *detail == '\0') std::snprintf(msg, sizeof msg, "%s", name);
    else std::snprintf(msg, sizeof msg, "%s (%s)", name, detail);
    if (*e->dbgOut == 0) {
        if (e->display) e->display(0x8510007f, msg, -3, nullptr, 1, 1);
        return;
    }
    char out[0x100 + 0x40];
    std::snprintf(out, sizeof out, "Storm Error : handle never released -- %s\n", msg);
    if (e->debugOut) e->debugOut(out);
}

namespace {
uint32_t g_dbgLatched;     // 0x45A74
uint32_t g_dbgMemOn;       // 0x45A70 (no registry in the web build: the option reads as off)
uint32_t g_dbgUiSink;      // 0x43138
void debugOutA(const char* s) { OutputDebugStringA(s); }
}  // namespace

// The engine's callers (resource-list teardown): the option is never on, so this is the no-op retail is with it unset.
void Storm_Sub2FB30(const char* name, const char* detail) {
    static const Storm2FB30Env env = { &g_dbgLatched, &g_dbgMemOn, &g_dbgUiSink, nullptr, nullptr, debugOutA };
    Storm13c_2FB30(&env, name, detail);
}
