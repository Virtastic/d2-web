// 1.13c Storm+0x2F250 (ordinal 464): SErrRegisterMessageSource. Lazy-init the SErr lock, push a 12-byte
// node {u16 id; module; next} on the source list head (0x6FC4537C), return 1.
#include <Windows.h>
#include "storm_state.h"
extern CRITICAL_SECTION g_stormErrLock;  // 0x6FC42FE0, defined in storm_serr.cpp

extern "C" uint32_t Storm_SErrRegisterMessageSource_2F250(uint16_t id, uint32_t source) {
    if (__atomic_add_fetch(&g_stormLockInit, 1, __ATOMIC_SEQ_CST) == 0)
        InitializeCriticalSection(&g_stormErrLock);
    else
        __atomic_sub_fetch(&g_stormLockInit, 1, __ATOMIC_SEQ_CST);
    EnterCriticalSection(&g_stormErrLock);
    auto* n = new StormSrcNode{};
    n->id = id;
    n->source = (void*)(uintptr_t)source;
    n->next = g_stormSrcList;
    g_stormSrcList = n;
    LeaveCriticalSection(&g_stormErrLock);
    return 1;
}
