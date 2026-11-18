// 1.13c D2Client+0xDF00: register a message source, stdcall(source). First use (the flag at 0x11C56C clear) creates the lock (host
// no-op), queues the teardown 0xDD20 through the exit-list thunk 0x3F82 and sets the flag. Under the lock a node is taken from the
// list at 0x1233F8 through 0x2F70(list, 0, 0) and its word at +8 := source. Tail-leaves the lock: EAX is the leave's 0.
#include "d2c_seam.h"
extern "C" uint32_t d2c_df00(const D2cSeam* s, uint32_t* flag, uint32_t source) {
    if (!*flag) { (*s)(0x3f82, 0, 0, 0, 0, 0, s->mod(0xdd20)); *flag = 1; }
    uint8_t* node = (uint8_t*)(uintptr_t)(*s)(0x2f70, 0, 0, 0, 0, 0, s->mod(0x1233f8), 0, 0);
    std::memcpy(node + 8, &source, 4);
    return 0;
}
