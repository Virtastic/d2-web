// 1.13c D2Gfx+0x0A760: copy a clipped 15-line cell through the selected palette transform.
#include <cstddef>
#include <cstdint>

struct D2GfxCellA760 {
    uint8_t opaque[8];
    uint8_t flags;
    uint8_t opaque0[7];
    const uint8_t* pixels;
};
static_assert(offsetof(D2GfxCellA760, flags) == 8);
static_assert(offsetof(D2GfxCellA760, pixels) == 16);

struct D2GfxA760State {
    int32_t clipTop;            // D2Gfx+0x9D464
    int32_t clipBottom;         // D2Gfx+0x948CC
    int32_t clipLeft;           // D2Gfx+0x95444
    int32_t clipRight;          // D2Gfx+0x9D46C
    int32_t pitch;              // D2Gfx+0x9477C
    int32_t dstX[15];           // D2Gfx+0x90C44, indexed by clipped source row
    uint32_t srcRight[15];      // D2Gfx+0x90C80
    uint32_t srcOffset[15];     // D2Gfx+0x90CBC
    const uint8_t* palette;     // D2Gfx+0x91678 (32 adjacent 256-byte transforms)
    void (*rleFallback)(const uint8_t*, int32_t, int32_t, uint8_t*, uint32_t,
                        const uint8_t*, const D2GfxA760State*);
};

// `dst` is the destination surface origin. The renderer globals used by retail are
// passed as state so the same operation can be replayed from a pinned snapshot.
extern "C" void d2gfx_sub_a760(uint8_t* dst, const D2GfxCellA760* cell,
                                int32_t x, int32_t y, uint32_t gamma,
                                const D2GfxA760State* st) {
    const uint8_t* transform = st->palette + ((gamma >> 3u) & 0x1fu) * 256u;
    if ((cell->flags & 4u) != 0) {
        st->rleFallback(cell->pixels, x, y, dst, 15, transform, st);
        return;
    }

    uint32_t first = st->clipTop - y > 0 ? static_cast<uint32_t>(st->clipTop - y) : 0u;
    int32_t end = st->clipBottom - y;
    if (end > 15) end = 15;
    uint8_t* row = dst + static_cast<uint32_t>(st->pitch) * first;
    for (uint32_t line = first; static_cast<int32_t>(line) < end; ++line) {
        int32_t left = st->clipLeft - (x + st->dstX[line]);
        if (left < 0) left = 0;
        uint32_t right = static_cast<uint32_t>(st->clipRight - (x + st->dstX[line]));
        if (static_cast<int32_t>(right) < 0) right = 0;
        if (right > st->srcRight[line]) right = st->srcRight[line];
        if (static_cast<int32_t>(right) > left) {
            const uint8_t* src = cell->pixels + st->srcOffset[line] + static_cast<uint32_t>(left);
            uint8_t* out = row + st->dstX[line] + left;
            for (uint32_t i = static_cast<uint32_t>(left); i < right; ++i, ++src, ++out)
                *out = transform[*src];
        }
        row += st->pitch;
    }
}
