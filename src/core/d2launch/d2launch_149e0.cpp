// 1.13c D2Launch+0x149E0 (sub_149E0). The count (DAT_6fa65e34) and the handle table
// (DAT_6fa653c0) are passed in, so the caller owns the storage; the D2Win import (thunk 0x9922, stdcall, one dword) is
// passed as imp9922. Table entries are 4-byte slots, so the port works on uint32_t cells (wasm32 layout kept).
#include <cstdint>

extern "C" void d2l_149e0(uint32_t* count, uint32_t* table, void (*imp9922)(uint32_t*)) {
    int32_t n = (int32_t)*count;
    for (int32_t i = 0; i < n; i++) imp9922(&table[i]);
    *count = 0;
}
