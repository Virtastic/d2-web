// 1.13c D2Lang+0x030E4: thunk, leaves critical section 4 (the CRT heap lock, _free's __lock(4) pair).
void D2Lang_LeaveLock(int idx);
void D2Lang_LeaveHeapLock() { D2Lang_LeaveLock(4); }
