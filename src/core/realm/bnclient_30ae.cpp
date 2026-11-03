// 1.13c BNClient+0x030AE (21 B, 15 callers). Own file.
// __unlock(lock_num): LeaveCriticalSection on the pointer in the CRT lock table DAT_6FF3D090, entry lock_num (8-byte
// stride: pointer, flag). The table is owned by bnclient_batch_2558.cpp (bn_unlock_lock); the seam below takes the
// table and the leave call as arguments so the rule is a pure function of them (the vector drives it).
#include <cstdint>
#include <emscripten.h>

extern "C" void bn_unlock_lock(int32_t lock_num);   // bnclient_batch_2558.cpp: the module table + LeaveCriticalSection

// The rule: slot = table[lock_num*2] (dword index, so entry n sits at byte 8n); leave(slot). lock_num outside the
// 0x24 slots is not released (the table has no such entry).
extern "C" void bn_unlock_30ae_seam(int32_t lock_num, const uint32_t* table, void (*leave)(uint32_t cs)) {
    if (lock_num < 0 || lock_num >= 0x24) return;
    leave(table[lock_num * 2]);
}

extern "C" EMSCRIPTEN_KEEPALIVE void bn_unlock_30ae(int32_t lock_num) {
    bn_unlock_lock(lock_num);
}
