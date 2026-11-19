// BNClient batch b143: 0xC860 0xC960 0xC9E0 0xCDD0 0xCE60 0xE740 0xF0A0.
// Win32/ordinal callees are injected as callbacks (garbled decompile; not byte-vectored).
#include <cstdint>
#include <cstring>

// 0xC860, 0xC960 and 0xC9E0 live in src/core/bnclient/bnclient_c860.cpp, bnclient_c960.cpp and bnclient_c9e0.cpp
// (0xC9E0 is what d2_realm_client_bncs_logon runs).

// 1.13c BNClient+0x0CDD0
// Walk a double-NUL multistring, CreateProcess each (flags 0x20); close handles; 0 on first failure.
extern "C" uint32_t d2_bnclient_cdd0(const char* list, int (*createProc)(const char*)) {
    while (*list) {
        if (!createProc(list)) return 0;
        list += strlen(list) + 1;
    }
    return 1;
}

// 1.13c BNClient+0x0CE60
// If Ordinal_268 nonzero: alloc, Ordinal_269 read, if ok and file opens write it, free. No return value.
extern "C" void d2_bnclient_ce60(int (*ready)(), int (*read)(), int (*open)(), void (*write)(), void (*close)(), void (*release)()) {
    if (!ready()) return;
    if (read() && open() != -1) { write(); close(); }
    release();
}

// 1.13c BNClient+0x0E740
// Download-thread starter: bail if Ordinal_10001 is 0; reset progress under the CS; empty path -> 0;
// if file times match (CompareFileTime==0) touch file and finish synchronously (return 0); else
// build the 0x230-dword request (len=strlen+0x21, 0x01 at +3, "68XIVD2D" tag) and start the thread.
extern "C" uintptr_t d2_bnclient_e740(const char* path, int (*init)(), void (*resetProgress)(), int (*statEqual)(const char*),
                                     void (*finishSync)(), uintptr_t (*startThread)(const char*)) {
    if (!init()) return 0;
    resetProgress();
    if (!path || !*path) return 0;
    if (statEqual(path)) { finishSync(); return 0; }
    return startThread(path);
}

// 0x0F0A0 moved to src/core/bnclient/bnclient_f0a0.cpp.
