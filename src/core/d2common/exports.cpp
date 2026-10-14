#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace d2common {

struct D2Common10003Target {
    std::uint8_t reserved[0x4C];
    std::uint32_t value;
};

static_assert(offsetof(D2Common10003Target, value) == 0x4C);

// 1.13c D2Common+0x2D0B0
void D2Common_10003(D2Common10003Target** target, std::int32_t multiplier,
                    std::int32_t addend) {
    if (target == nullptr || *target == nullptr) {
        std::exit(-1);
    }

    // Retail performs 32-bit x86 arithmetic before forwarding to ordinal 10118.
    (*target)->value = (static_cast<std::uint32_t>(multiplier) << 4) +
                       static_cast<std::uint32_t>(addend);
}

} // namespace d2common
