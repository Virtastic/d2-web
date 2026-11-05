// 1.13c D2Common+0x41230 (sub_41230): walks the level's grid C from (width-2, 0) to (0, height-2) two cells per
// step, stamping each visited cell through 42A70. The cell value minus (0x371 + snow) indexes the eight-entry step
// table (retail .rdata at DAT_6fde3770: dx at +0, dy at +4); snow is 0x4C when the level type at +0x1D0 is 0x75.
// The walk ends with the corner stamps 0x38A and 0x389 (plus snow). Raw 1.13c offsets; pointers are wasm32 host
// addresses, as in d2common_42a70.cpp.
#include <cstdint>
#include <cstring>

extern "C" void D2Common_42A70(void* ctx, int32_t x0, int32_t y0, int32_t idx, int32_t flag, int32_t extra);

namespace {
// DAT_6fde3770 (retail .rdata, RVA 0x93770): the eight (dx, dy) pairs, read from the retail image.
const int32_t kStepDx[8] = {-1, 0, 1, 0, 0, 1, 0, -1};
const int32_t kStepDy[8] = {0, -1, 0, 1, -1, 0, 1, 0};

inline uint32_t R32(const void* p, uint32_t off)
{
    uint32_t v;
    std::memcpy(&v, static_cast<const uint8_t*>(p) + off, 4);
    return v;
}
inline const void* P(uint32_t addr) { return reinterpret_cast<const void*>(static_cast<uintptr_t>(addr)); }
}

// 1.13c D2Common+0x41230: stdcall (ret 4), one argument (the level). The stamp is 0x42A70 (ECX level, EDX x,
// stack y, tile, -1, 0); D2Common_41230With takes it as a seam so a probe can log retail's call records.
typedef void (*D2Common41230Stamp)(void* ctx, int32_t x0, int32_t y0, int32_t idx, int32_t flag, int32_t extra);
extern "C" void D2Common_41230With(void* level, D2Common41230Stamp stamp)
{
    const void* out = P(R32(level, 0x14));                          // level+0x14: pOutdoors
    const int32_t lastX = static_cast<int32_t>(R32(out, 0x5C)) - 2;  // width - 2
    const int32_t lastY = static_cast<int32_t>(R32(out, 0x60)) - 2;  // height - 2
    const int32_t snow = R32(level, 0x1D0) == 0x75 ? 0x4C : 0;
    const auto* cells = static_cast<const uint32_t*>(P(R32(out, 0x04)));
    const auto* rows = static_cast<const int32_t*>(P(R32(out, 0x08)));

    int32_t x = lastX, y = 0;
    while (x != 0 || y != lastY) {
        const int32_t k = static_cast<int32_t>(cells[rows[y] + x]) - (0x371 + snow);
        if (k < 0 || k > 7) return;   // outside the step table: retail reads past it; no capture reaches this
        stamp(level, x, y, k + snow + 0x37D, -1, 0);
        x += kStepDx[k] * 2;
        y += kStepDy[k] * 2;
    }
    stamp(level, lastX, 0, snow + 0x38A, -1, 0);
    stamp(level, 0, lastY, snow + 0x389, -1, 0);
}

extern "C" void D2Common_41230(void* level) { D2Common_41230With(level, D2Common_42A70); }
