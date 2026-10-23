// 1.13c D2Lang+0x013F2: thunk, LeaveCriticalSection slot 8 via 0x2215.
void D2Lang_LeaveLock(int idx);
void D2Lang_13f2_unlock8() { D2Lang_LeaveLock(8); }
