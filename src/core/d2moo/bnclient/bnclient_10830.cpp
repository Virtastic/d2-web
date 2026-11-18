// 1.13c BNClient+0x10830 (ECX = bytes, EDX = size): keep a copy of an answer.
// The block kept from the last call, when there is one, is released (line 0x1BD); a block of the given size is
// allocated (line 0x1BE), stored as the kept block and filled with the bytes. Returns 1.
#include "bnclient_send.h"
#include <cstring>

// 1.13c BNClient+0x10830
extern "C" uint32_t bnclient_10830(const BnCallEnv& e, uint32_t* kept_slot, const uint8_t* bytes, uint32_t size) {
    if (*kept_slot != 0) bn_call(e, 0x801e, 0, 0, 0, 0, 0, *kept_slot, BN_MOD(0x1aff0), 0x1BD, 0);
    const uint32_t block = bn_call(e, 0x8018, 0, 0, 0, 0, 0, size, BN_MOD(0x1aff0), 0x1BE, 0);
    *kept_slot = block;
    memcpy((void*)(uintptr_t)block, bytes, size);
    return 1;
}
