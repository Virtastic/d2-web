// probe for Fog 0x14880 (sub_14880, annotated stack dump), over the HookEnv recorder. regs eax = ctx (r[0]), stdcall(hProcess = r[1], module chain = r[2]).
// Blobs: ctx r[14] (eip +0xB8, esp +0xC4 points at sk), sk r[16], mod r[18] (module node: base +0x204, size +0x208, next +0x20C), lg r[20] (hook log),
// sv r[22] (the tuple's pins: IsBadReadPtr result, sym result, disp, three name dwords; the descriptor's iat/hook/hookw lines carry the same values).
// Port: fog_annotatedStackDump in src/core/fog/fog_sub_14880.cpp through its host seams. The probe records each seam as the call retail makes:
// 0x190A0 for the two plain lines, 0x18AF0 for the formatted ones, the symbol lookup behind the pointer at Fog+0x56E38 (hooked at 0x16920), 0x1F710.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include "hookenv.h"
struct Module { uint8_t pad[0x204]; uint32_t base; uint32_t size; Module* next; };
struct FogStackDumpHost {
    void (*log)(const char* fmt, ...);
    int (*symLookup)(uint32_t proc, uint32_t addr, int* disp, char* name);
    bool (*badRead)(const void* p, uint32_t n);
    void (*fatal)(uint32_t code);
    void (*readString)(char* dst, uint32_t addr, uint32_t n);
};
void fog_stackDumpSetHost(const FogStackDumpHost& h);
void fog_annotatedStackDump(uint32_t proc, const uint8_t* ctx, const Module* mods);
namespace {
HookEnv* H;
const uint32_t* gSv;
bool gEipMod;
const uint32_t kLog = 0xB20268ECu;
const Module* gMods;
// the %s name word retail logs: a pointer into the module blob (node + 0x104), or the module's "" literal
uint32_t nameWord(const char* s) {
    const uint8_t* p = (const uint8_t*)s;
    return p >= (const uint8_t*)gMods && p < (const uint8_t*)gMods + 544 ? (uint32_t)(uintptr_t)s : 0xB2026790u;
}
void logf(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    if (!*fmt) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, kLog);
    else if (!std::strcmp(fmt, "Annotated stack dump")) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB202752Cu);
    else if (std::strstr(fmt, "EIP")) { const uint32_t eip = va_arg(ap, uint32_t); const char* nm = va_arg(ap, const char*); H->h(0x18AF0, 0, 0, 0, 0, 0, kLog, 0xB20274F4u, eip, nameWord(nm)); }
    else if (std::strstr(fmt, "%08X:")) H->h(0x18AF0, 0, 0, 0, 0, 0, kLog, 0xB20274E0u, va_arg(ap, uint32_t));
    else if (!std::strcmp(fmt, "%s\n")) H->h(0x18AF0, 0, 0, 0, 0, 0, kLog, 0xB2026E4Cu, 0xB3000000u);
    else { const uint32_t tgt = va_arg(ap, uint32_t); const char* nm = va_arg(ap, const char*); H->h(0x18AF0, 0, 0, 0, 0, 0, kLog, std::strstr(fmt, "='%s'") ? 0xB2027488u : 0xB2027450u, tgt, nameWord(nm)); }
    va_end(ap);
}
int sym(uint32_t proc, uint32_t addr, int* disp, char* name) {
    uint32_t st[16] = {};
    const uint32_t ret = H->h(0x16920, 0, 0, 0, 0, 0, proc, addr, (uint32_t)(uintptr_t)disp, (uint32_t)(uintptr_t)st);
    std::memcpy(name, st + 5, 11); name[11] = 0;
    return (int)ret;
}
bool badRead(const void*, uint32_t) { return gSv[0] != 0; }
void fatal(uint32_t) { abort(); }
void readString(char* dst, uint32_t addr, uint32_t) {
    H->h(0x1F710, 0, (uint32_t)(uintptr_t)dst, addr, 0, 0, 0x50, 0);
    std::strcpy(dst, "lit");
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14880(uint32_t* r) {
    gSv = (const uint32_t*)(uintptr_t)r[22];
    const uint32_t sc[] = { 0x16920, gSv[1], (3u << 24) | 0x16920, gSv[2], (4u << 24) | 0x16920, 0, (4u << 24) | 0x16920, 0, (4u << 24) | 0x16920, 0,
                            (4u << 24) | 0x16920, 0, (4u << 24) | 0x16920, 0, (4u << 24) | 0x16920, gSv[3], (4u << 24) | 0x16920, gSv[4],
                            (4u << 24) | 0x16920, gSv[5], 0x1F710, 0x5000, 0, 0 };
    HookEnv e((uint32_t*)(uintptr_t)r[20], sc);
    H = &e;
    const uint8_t* ctx = (const uint8_t*)(uintptr_t)r[0];
    const Module* mods = (const Module*)(uintptr_t)r[2];
    gMods = mods;
    uint32_t eip; std::memcpy(&eip, ctx + 0xB8, 4);
    gEipMod = eip >= mods[0].base && eip < mods[0].base + mods[0].size;
    const FogStackDumpHost hst = { logf, sym, badRead, fatal, readString };
    fog_stackDumpSetHost(hst);
    fog_annotatedStackDump(r[1], ctx, mods);
    r[4] = 0;
}
