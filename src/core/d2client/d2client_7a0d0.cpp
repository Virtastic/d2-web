// 1.13c D2Client+0x7A0D0 (162 B, caller 0x7A9F0): size the automap unit grid for the view in the screen object (ESI) and (re)allocate its cell block.
// Rule (1.13c disassembly decides): columns = (right - left) / 160 + 11; rows = (bottom - top) / 80 + 12; side = columns + rows - 1;
//   cells = side * side. A held block (handle != 0) of the same cell count is kept; a different count frees it (Fog 10043, pool 0xD4BFC, tag 0x3A)
//   and allocates cells * 36 bytes (Fog 10042, tag 0x3D). Stores side to +0xEAB0, columns to +0xEAB4, rows to +0xEAB8 on every path.
// Prior art: a 1.14d note gives the same grid (width / 160 + 11 by height / 80 + 11, square); the cell block, tags and fields are 1.13c's own.
#include <cstdint>
#include <cstddef>

struct D2c7a0d0View {                       // the screen object at ESI: only the bytes this function reads or writes
    uint32_t pad0;
    int32_t left, top, right, bottom;       // +0x04 +0x08 +0x0C +0x10
    uint8_t pad1[0xEAA8 - 0x14];
    uint32_t handle;                        // +0xEAA8 cell block
    int32_t cells;                          // +0xEAAC cell count the block was sized for
    int32_t side;                           // +0xEAB0
    int32_t columns;                        // +0xEAB4
    int32_t rows;                           // +0xEAB8
};
static_assert(offsetof(D2c7a0d0View, left) == 0x04 && offsetof(D2c7a0d0View, bottom) == 0x10, "view rect");
static_assert(offsetof(D2c7a0d0View, handle) == 0xEAA8 && offsetof(D2c7a0d0View, cells) == 0xEAAC, "cell block");
static_assert(offsetof(D2c7a0d0View, side) == 0xEAB0 && offsetof(D2c7a0d0View, rows) == 0xEAB8, "outputs");

struct D2c7a0d0Env {
    void* ctx;
    uint32_t (*alloc)(void*, uint32_t bytes);       // Fog 10042 through thunk 0xBF7A (pool 0xD4BFC, tag 0x3D)
    void (*release)(void*, uint32_t handle);        // Fog 10043 through thunk 0xBF86 (pool 0xD4BFC, tag 0x3A)
};

extern "C" void d2c_7a0d0(D2c7a0d0View* s, const D2c7a0d0Env* e) {
    const int32_t columns = (s->right - s->left) / 160 + 11;
    const int32_t rows = (s->bottom - s->top) / 80 + 12;
    const int32_t side = rows + columns - 1;
    const int32_t cells = int32_t(uint32_t(side) * uint32_t(side));
    if (s->handle == 0 || cells != s->cells) {
        if (s->handle != 0) e->release(e->ctx, s->handle);
        s->handle = e->alloc(e->ctx, uint32_t(cells) * 36u);
        s->cells = cells;
    }
    s->columns = columns; s->side = side; s->rows = rows;
}
