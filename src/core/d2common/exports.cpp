#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <D2DataTbls.h>

namespace d2common {

// 1.13c D2Common+0x2D0B0 (ordinal 10003): SetBit on the bitfield `**target`. The inner call is Fog.dll
// ordinal 10118 (Fog+0x14090): byte[(bit >> 3)] |= gdwBitMasks[bit & 7], with bit = multiplier * 16 + addend.
void D2Common_10003(std::uint8_t** target, std::int32_t multiplier, std::int32_t addend) {
    if (target == nullptr) {
        std::exit(-1);
    }

    // Retail does 32-bit arithmetic, then an arithmetic shift: negative bits address bytes before the field.
    const std::int32_t bit = static_cast<std::int32_t>(static_cast<std::uint32_t>(multiplier) << 4) + addend;
    (*target)[bit >> 3] |= static_cast<std::uint8_t>(1u << (bit & 7));
}

// 1.13c D2Common+0x70F40 (ordinal 10352): `ret 4`, a stdcall no-op that discards its single argument.
void D2Common_10352(std::uint32_t) {}

// 1.13c D2Common+0x71280 (ordinal 10041): returns the address of the gamble.txt table header (record count, then
// the selection array pointer), which retail keeps as a standalone global; ours is a member of sgptDataTables.
D2GambleDataTbl* D2Common_10041() { return &sgptDataTables->pGambleDataTables; }

// 1.13c D2Common+0x4DA00 (ordinal 10095 in 1.13c; unrelated to 1.10f's D2Common_10095 room-index lookup):
// isometric tile coords (a, b) -> pixel offsets: *x = (a - b) * 80 - 80, *y = (a + b) * 40 + 80. 32-bit wrap.
void Iso4DA00(std::int32_t a, std::int32_t b, std::int32_t* x, std::int32_t* y) {
    *x = static_cast<std::int32_t>(static_cast<std::uint32_t>(a - b) * 0x50u - 0x50u);
    *y = static_cast<std::int32_t>(static_cast<std::uint32_t>(a + b) * 0x28u + 0x50u);
}

} // namespace d2common
