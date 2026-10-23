// 1.13c D2Lang+0x2215: LeaveCriticalSection(table[idx]), table at 0x6FC0F1E8 stride 8.
// wasm is single-threaded: no lock is ever held, nothing to release.
void D2Lang_LeaveLock(int idx) { (void)idx; }
