// 1.13c D2Lang+0x022C1: thunk, leaves lock 10 (sub_2215(10)). No-op on single-threaded wasm.
void D2Lang_LeaveLock(int idx);
void D2Lang_Sub22C1(void) { D2Lang_LeaveLock(10); }
