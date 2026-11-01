// D2Common 1.13c 0x1E880: returns pInv when v matches the word at pInv->pExtra+0x5C, else 0.
// Guest structs are 32-bit images: the port dereferences guest addresses directly (wasm32 pointers).
#include <cstdint>

// 1.13c D2Common+0x1E880 (stdcall, 2 args)
uint32_t D2Common_1E880_MatchInvItem(uint32_t v, uint32_t pInv) {
    if (v == 0 || pInv == 0) return 0;
    const uint32_t* inv = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pInv));
    if (inv[0] != 4 || inv[5] == 0) return 0;
    const uint32_t* ext = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(inv[5]));
    return v == ext[0x5c / 4] ? pInv : 0;
}
