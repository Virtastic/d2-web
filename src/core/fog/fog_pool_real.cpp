// Fog pool allocator tail: 1.13c Fog+0x190A0, +0x1AD20, +0x1B190 as real extern "C" (decomp names).
// Retail 32-bit pool layout (as fog_sub_1b390.cpp); hidden EAX/EBX args are explicit params.
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <emscripten.h>
#include <Windows.h>

extern "C" {
void fog_sub_11690(uint32_t*);
void FUN_6ff5fc20(void);
void Ordinal_116(void);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff684a0(void);
char* fog_sub_1f640(char*, uint32_t, uint32_t, uint32_t);
extern const char DAT_6ff768ec[];
void FUN_6ff68770(const char*, int, int, const char*, va_list);

// 1.13c Fog+0x190A0
// ordinal 10030: log(channel, fmt, ...); NULL channel = DAT_6ff768ec.
EMSCRIPTEN_KEEPALIVE void FUN_6ff690a0(const char* ch, const char* fmt, ...) {
    if (!fmt) { FUN_6ff5fc20(); Ordinal_116(); FUN_6ff60f30(6, "", 0x129, "Unrecoverable internal error %08x"); FUN_6ff684a0(); _Exit(-1); }
    if (!ch) ch = DAT_6ff768ec;
    va_list ap; va_start(ap, fmt);
    FUN_6ff68770(ch, 1, 1, fmt, ap);
    va_end(ap);
}

// 1.13c Fog+0x1AD20: out-of-memory report; vsprintf(fmt, ...) then the memory-status dump.
EMSCRIPTEN_KEEPALIVE void FUN_6ff6ad20(const char* fmt, ...) {
    char msg[4092], num[32];
    va_list ap; va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);
    // host has no GlobalMemoryStatus: the six figures read 0 (diagnostic text only)
    struct { uint32_t dwTotalPhys, dwAvailPhys, dwTotalPageFile, dwAvailPageFile, dwTotalVirtual, dwAvailVirtual; } ms{};
    FUN_6ff690a0(DAT_6ff768ec, "--------------------------------------------------------");
    FUN_6ff690a0(DAT_6ff768ec, "ERROR: Out of memory!");
    FUN_6ff690a0(DAT_6ff768ec, "%s", msg);
    const struct { const char* f; uint32_t v; } t[] = {
        {"total physical:     %s", (uint32_t)ms.dwTotalPhys}, {"available physical: %s", (uint32_t)ms.dwAvailPhys},
        {"total page:         %s", (uint32_t)ms.dwTotalPageFile}, {"available page:     %s", (uint32_t)ms.dwAvailPageFile},
        {"total virtual:      %s", (uint32_t)ms.dwTotalVirtual}, {"available virtual:  %s", (uint32_t)ms.dwAvailVirtual}};
    for (auto& e : t) FUN_6ff690a0(DAT_6ff768ec, e.f, fog_sub_1f640(num, 0x20, e.v, 0));
    FUN_6ff690a0(DAT_6ff768ec, "");
}

// 1.13c Fog+0x1B190 (EAX=size, EBX=manager): malloc(size+4), chain into hash bucket p%0x3ff at mgr+0x7ac
// under the manager critical section (mgr+4); returns p+4.
EMSCRIPTEN_KEEPALIVE uint32_t* FUN_6ff6b190(int32_t size, uint8_t* mgr) {
    uint32_t* p = (uint32_t*)malloc((size_t)size + 4);
    if (!p) {
        FUN_6ff6ad20("(3) Pool Blocks overflowed at %d");
        FUN_6ff5fc20(); Ordinal_116();
        FUN_6ff60f30(6, "", 0x388, "Unrecoverable internal error %08x");
        FUN_6ff684a0();
        _Exit(-1);
    }
    fog_sub_11690((uint32_t*)(mgr + 4));
    EnterCriticalSection((LPCRITICAL_SECTION)(mgr + 4));
    uint32_t* slot = (uint32_t*)(mgr + 0x7ac) + (int32_t)(intptr_t)p % 0x3ff;
    *p = *slot;
    *slot = (uint32_t)(uintptr_t)p;
    fog_sub_11690((uint32_t*)(mgr + 4));
    LeaveCriticalSection((LPCRITICAL_SECTION)(mgr + 4));
    return p + 1;
}
}
