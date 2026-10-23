void D2Lang_LeaveLock(int idx);

// 1.13c D2Lang+0x043AA: thunk, leaves critical section 4 (heap lock for __heap_alloc).
void D2Lang_sub_43AA() { D2Lang_LeaveLock(4); }
