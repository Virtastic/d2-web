// probe for Fog 0x16430 (sub_16430, deadlock report): __fastcall, ECX = thread handle (r[0]), plain ret; blob lg = r[14] (the hook log).
// Port: fog_sub_16430 (src/core/fog/fog_batch_19040.cpp) over its FogAbortApi seam. The seam members are the recorded hooks: the log
// 190A0 (cdecl, default_log 0x268EC, fmt 0x277D8, handle), the fatal 10F30 (cdecl, level 1, file 0x277A0, line 0x2B4, fmt 0x277C4),
// the stack dump 15C00 (EAX = handle) and the abort 184A0. Module addresses are tagged 0xB2000000|rva as the capture logs them.
#include <emscripten.h>
#include <cstdint>
#include <cstdarg>
#include "hookenv.h"

struct FogAbortSeam {
    void (*log)(void* logfile, const char* fmt, ...);
    void (*fatal)(int level, const char* file, int line, const char* fmt, ...);
    void (*dumpThread)(uint32_t thread_handle);
    void (*dumpStack)(uint32_t thread_handle);
    void (*abort_)(void);
    void* defaultLog;
};
extern "C" void fog_sub_16430(uint32_t thread_handle, const FogAbortSeam* api);

namespace {
HookEnv* H;
const uint32_t kDefaultLog = 0xB20268ECu, kFmtDump = 0xB20277D8u, kFile = 0xB20277A0u, kFmtDeadlock = 0xB20277C4u;
void lg(void*, const char* fmt, ...) {
    va_list ap; va_start(ap, fmt); const uint32_t handle = va_arg(ap, uint32_t); va_end(ap);
    H->h(0x190A0, 0, 0, 0, 0, 0, kDefaultLog, kFmtDump, handle);
}
void fatal(int level, const char*, int line, const char*, ...) {
    H->h(0x10F30, 0, 0, 0, 0, 0, (uint32_t)level, kFile, (uint32_t)line, kFmtDeadlock);
}
void dumpThread(uint32_t) {}   // not reached on this path (16470 only)
void dumpStack(uint32_t handle) { H->h(0x15C00, handle); }
void abortFn(void) { H->h(0x184A0); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16430(uint32_t* r) {
    static const uint32_t sc[2] = { 0, 0 };
    HookEnv e((uint32_t*)(uintptr_t)r[14], sc);
    H = &e;
    const FogAbortSeam api = { lg, fatal, dumpThread, dumpStack, abortFn, nullptr };
    fog_sub_16430(r[0], &api);
    r[4] = 0;
}
