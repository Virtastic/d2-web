// 1.13c D2Lang+0x013E9: thunk, __lock(8) (enter CRT lock 8). No-op on single-threaded wasm.
void D2Lang_EnterLock(int idx);
void D2Lang_Sub13E9(void) { D2Lang_EnterLock(8); }
