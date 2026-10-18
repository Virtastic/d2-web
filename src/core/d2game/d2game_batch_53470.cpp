// src/core/d2game/d2game_batch_53470.cpp -- the 1.13c D2Game.dll item-serialization wrapper at
// 0x53470, transcribed from the cached Ghidra decompile
// (build-wasm/ghidra-d2game/decomp/0x53470.c). Stage A batch lane: implementation + marker only, no
// spec, no vector.
//
// The unit is passed in EAX and the destination size in EDX (register-only arguments the decompiler
// dropped), the destination buffer is the single stack argument (the callee's `ret 4`), and the
// return is the serialized byte count. Callee 0xAFD0 is the D2Common.dll import thunk #10987
// (ITEMS_SerializeItemToBitstream, the 1.10f crosswalk), modelled as an Env seam in retail argument
// order. The fatal path is Fog #10265(id 0x75A) -> #10024(&0x6FD19FF9) -> _exit(-1); only the exit is
// observable.
// Prior art: 1.10f has no separate function (the serialize call is inlined); the same
// ITEMS_SerializeItemToBitstream(item, pBuffer, size, 1, 1, 0) call appears in
// reference/D2MOO/source/D2Game/src/PLAYER/PlrSaveItems.cpp.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seam for the callee src/ does not own. The ordinal is the 1.13c import.
struct D2GameBatch53470Env {
    // D2Common #10987 ITEMS_SerializeItemToBitstream(item, pBuffer, nSize, bServer, bSaveItemInv, bGamble)
    int32_t (*serializeItem)(void* pItem, void* pBuffer, uint32_t nSize, int32_t bServer,
                             int32_t bSaveItemInv, int32_t bGamble);
};

// 1.13c D2Game+0x53470
// EAX = pItem, stack = pBuffer, EDX = nSize. A null item, or an item whose dword at +0xC8 has bit
// 0xF set, serializes as zero bytes. Otherwise the item is written through D2Common #10987 with the
// flags (1, 1, 0) and its byte count returned; a zero-byte result is fatal (Fog 0x75A).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_53470(void* pItem, void* pBuffer, uint32_t nSize,
                                                const D2GameBatch53470Env* env)
{
    if (pItem == nullptr || ((rd32(pItem, 0xC8) >> 0xF) & 1) != 0) return 0;

    const int32_t written = env->serializeItem(pItem, pBuffer, nSize, 1, 1, 0);
    if (written == 0) d2game_fatal(0x75A);
    return written;
}
