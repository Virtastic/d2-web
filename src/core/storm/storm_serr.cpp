// 1.13c Storm SErr: real bodies over the shared state in storm_state.h.
#include <Windows.h>
#include "storm_state.h"

extern "C" int LoadStringA(void*, uint32_t, char*, int) __attribute__((weak));
extern "C" uint32_t FormatMessageA(uint32_t, const void*, uint32_t, uint32_t, char*, uint32_t, void*) __attribute__((weak));

int32_t g_stormLockInit;
char g_stormStringBuf[0x100];
uint32_t g_stormMod;
int32_t g_stormLastError;
StormSrcNode* g_stormSrcList;
uint32_t g_stormSrcInit;
CRITICAL_SECTION g_stormErrLock;  // 0x6FC42FE0

// 0x6FC35CB0 default strings (retail table, read from the 1.13c image; index 12 is unused here)
static const char* const kDefaultStrings[12] = {
    "ERROR #%u (0x%08x)", "This application has encountered a critical error:\n\n%s\n",
    "Program:\t%s\n", "File:\t%s\nLine:\t%d\n", "Function:\t%s\n", "Object:\t%s\n", "Handle:\t%s\n",
    "Expr:\t%s\n\n", "\n%s\n\n", "Press OK to terminate the application.",
    "Do you wish to terminate the application?", "File:\t%s\n"};

// 1.13c Storm+0x2F180
extern "C" void Storm_SErrLock_2F180() {
    if (InterlockedIncrement((long volatile*)&g_stormLockInit) == 0) {
        InitializeCriticalSection(&g_stormErrLock);
        EnterCriticalSection(&g_stormErrLock);
        return;
    }
    InterlockedDecrement((long volatile*)&g_stormLockInit);
    EnterCriticalSection(&g_stormErrLock);
}

// 1.13c Storm+0x2F1C0 (ESI = idx)
extern "C" const char* Storm_SErrString_2F1C0(int32_t idx) {
    if (LoadStringA((void*)(uintptr_t)g_stormMod, (uint32_t)idx + 0x5100, g_stormStringBuf, 0x100) == 0)
        return kDefaultStrings[idx];
    return g_stormStringBuf;
}

// 1.13c Storm+0x2F200: append a source node for g_stormMod to the list tail.
extern "C" void Storm_SErrAddSource_2F200(uint16_t id) {
    StormSrcNode** link = &g_stormSrcList;
    while (*link) link = &(*link)->next;
    auto* n = new StormSrcNode{};
    *link = n;
    n->id = id;
    n->source = (void*)(uintptr_t)g_stormMod;
}

// 1.13c Storm+0x2F3F0 (ordinal 462): SErrGetErrorStr.
extern "C" uint32_t Storm_SErrGetErrorStr_2F3F0(uint32_t msgId, char* buf, uint32_t count) {
    if (buf == nullptr || count == 0) {
        g_stormLastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    if (InterlockedIncrement((long volatile*)&g_stormLockInit) == 0)
        InitializeCriticalSection(&g_stormErrLock);
    else
        InterlockedDecrement((long volatile*)&g_stormLockInit);
    EnterCriticalSection(&g_stormErrLock);
    if (g_stormSrcInit == 0) {
        g_stormSrcInit = 1;
        Storm_SErrAddSource_2F200(0x510);
        Storm_SErrAddSource_2F200(0x876);
        Storm_SErrAddSource_2F200(0x878);
    }
    const void* source = nullptr;
    for (StormSrcNode* n = g_stormSrcList; n; n = n->next)
        if (n->id == (uint16_t)((msgId >> 16) & 0xfff)) { source = n->source; break; }
    LeaveCriticalSection(&g_stormErrLock);
    *buf = 0;
    uint32_t flags = (source ? 0xFFFFF800u : 0u) + 0x1000u;
    return FormatMessageA(flags, source, msgId, 0x400, buf, count, nullptr) != 0;
}
