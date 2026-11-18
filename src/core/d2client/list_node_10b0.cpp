#include "native_list_abi.h"
#include "storm_link.h"
#include <emscripten.h>

// 1.13c D2Client+0x10B0: ECX = anchor, EAX = list base; stack = node (0 -> the terminator at base+1
// word), mode. Unlinks the node, then links it before (mode != 1) or after (mode == 1) the anchor,
// storing the raw node argument as the neighbour's next word (storm_link.h, d2c_link::splice10b0).
// Native entry: pointer-width words. The FN probe drives the same rule on retail's 32-bit words.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_10b0(uintptr_t* ecxThis, uintptr_t* eaxBase, uintptr_t* arg1, int32_t arg2) {
    d2c_link::splice10b0<uintptr_t>(reinterpret_cast<uintptr_t>(ecxThis), reinterpret_cast<uintptr_t>(eaxBase),
                                    reinterpret_cast<uintptr_t>(arg1), arg2);
}
