// 1.13c D2Lang tail thunks: SEH epilogue stubs. Callees are host seams (wasm has no SEH/fhandle locks).
extern "C" {
void (*d2lang_n15_sub_13F2)(void);     // 0x13F2
void (*d2lang_n15_unlockFhandle)(int); // 0x70AA; arg is the caller's EBX (fh)

// 1.13c D2Lang+0x23D5
void d2lang_n15_sub_23D5(void) { if (d2lang_n15_sub_13F2) d2lang_n15_sub_13F2(); }

// 1.13c D2Lang+0x5AA6
void d2lang_n15_sub_5AA6(int fh) { if (d2lang_n15_unlockFhandle) d2lang_n15_unlockFhandle(fh); }

// 1.13c D2Lang+0x5D0D
void d2lang_n15_sub_5D0D(int fh) { if (d2lang_n15_unlockFhandle) d2lang_n15_unlockFhandle(fh); }
}
