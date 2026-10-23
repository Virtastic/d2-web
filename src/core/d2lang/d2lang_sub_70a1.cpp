// 1.13c D2Lang+0x070A1: thunk, LeaveCriticalSection slot 10 (atexit/cleanup unlock).
void D2Lang_LeaveLock(int idx);
void D2Lang_sub_70A1(void) { D2Lang_LeaveLock(10); }
