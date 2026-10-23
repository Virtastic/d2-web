// BNClient batch b143: 0xC860 0xC960 0xC9E0 0xCDD0 0xCE60 0xD280 0xE740 0xF0A0.
// Win32/ordinal callees are injected as callbacks (garbled decompile; not byte-vectored).
#include <cstdint>
#include <cstring>

// 1.13c BNClient+0x0D280
// Open file (Ordinal_10107), if handle != -1: Ordinal_10111 write, SetFileTime, close (Ordinal_10109).
extern "C" void d2_bnclient_d280(intptr_t (*open)(), void (*write)(), void (*setTime)(intptr_t), void (*close)()) {
    intptr_t h = open();
    if (h != -1) { write(); setTime(h); close(); }
}

// 1.13c BNClient+0x0C960
// Hash seed: lowercased 0x100-byte string hashed (FUN_372c0), result stored at +0x15, len 0x3d. Always 1.
extern "C" uint32_t d2_bnclient_c960(void (*gather)(), void (*hashStr)(), void (*store)()) {
    gather(); hashStr(); store();
    return 1;
}

// 1.13c BNClient+0x0C9E0
// As C960 but clears DAT+0x3EE40/0x3EFC0 first, also hashes {param, tick, DAT+0x3F874} (0x1c bytes); store +0x1d len 0x3a.
extern "C" uint32_t d2_bnclient_c9e0(uint32_t* g0, uint32_t* g1, void (*gather)(), void (*hashStr)(), void (*hashSeed)(), void (*store)()) {
    *g0 = 0; *g1 = 0;
    gather(); hashStr(); hashSeed(); store();
    return 1;
}

// 1.13c BNClient+0x0C860
// Two lowercased strings + tick seed hashed, store +0x31 len 0x31. Always 1.
extern "C" uint32_t d2_bnclient_c860(void (*gather)(), void (*hashStr)(), void (*hashSeed)(), void (*store)()) {
    gather(); hashStr(); hashSeed(); store();
    return 1;
}

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

// 1.13c BNClient+0x0F0A0
// Resolve host (digit-leading -> inet_addr, else gethostbyname; failure -> 0), then Fisher-Yates shuffle the
// address list (rand()*n>>15 + i), then join as strings into out. `rnd`=rand(). Returns 1.
extern "C" uint32_t d2_bnclient_f0a0(uint32_t** list, int (*rnd)(), void (*cat)(uint32_t)) {
    int n = 0;
    while (list[n]) ++n;
    int cnt = n - 1;
    for (int i = 0, rem = cnt; i < cnt; ++i, --rem) {
        int j = ((rnd() * rem) >> 15) + i;
        if (j < i) return 0; // retail: error string + _exit(-1)
        uint32_t t = *list[i]; *list[i] = *list[j]; *list[j] = t;
    }
    for (int i = 0; list[i]; ++i) cat(*list[i]);
    return 1;
}
