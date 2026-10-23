// MSVC CRT startup/teardown shims, D2sound 1.13c. Host seams stand in for CRT/Win32 calls.
#include <cstdint>

// 1.13c D2sound+0x016D6 -- _atexit: 0 on success, -1 on failure of __onexit
int D2sound_16D6(void* (*onexit)(void*), void* fn) { return (onexit(fn) != nullptr) - 1; }

// 1.13c D2sound+0x016E8 -- __RTC_Initialize: the RTC init-table range is empty (begin==end), loop body never runs
void D2sound_16E8() {}

// 1.13c D2sound+0x018C5 -- thunk: TlsAlloc(), result discarded
void D2sound_18C5(uint32_t (*tlsAlloc)()) { tlsAlloc(); }

// 1.13c D2sound+0x018CE -- __mtterm: free the TLS index if allocated, then __mtdeletelocks
void D2sound_18CE(uint32_t* tlsIndex, void (*tlsFree)(uint32_t), void (*deleteLocks)()) {
    if (*tlsIndex != 0xffffffffu) { tlsFree(*tlsIndex); *tlsIndex = 0xffffffffu; }
    deleteLocks();
}

struct D2sndPtdInit { uint32_t tid, thandle, terrno, tdoserrno, fpds, holdrand; uint32_t pad[11]; const void* initaddr; };
// 1.13c D2sound+0x018EB -- __initptd: initaddr and holdrand only; locale arg unused
void D2sound_18EB(D2sndPtdInit* p, const void* initAddr) { p->initaddr = initAddr; p->holdrand = 1; }
