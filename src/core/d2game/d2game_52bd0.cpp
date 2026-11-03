// 1.13c D2Game+0x52BD0: decode one saved-item bit record into the 0x48-byte struct. ESI = out struct, stack (ret 4) = data pointer.
// Callees are seams (D2Game thunk RVAs): A2B6 bit-reader init (reader, data, 0x40), A2D4 read(reader, n), A2C8 signed read(reader, n), A2E0 end(reader) (the
// function's return in the modern paths), AC0A flag test (flags, mask), AE74 code to id (#10814), B198 (#10442), A53E row lookup (#10695, row+0x80 read).
// Retail narrows each read to the width of its store (al, movzx al, or the whole eax); the casts below mirror the listing store by store.
#include <cstdint>
#include <cstring>
struct D2G52BD0Seams {
    void     (*fA2B6)(uint32_t reader, uint32_t data, uint32_t size);
    uint32_t (*fA2D4)(uint32_t reader, uint32_t n);
    uint32_t (*fA2C8)(uint32_t reader, uint32_t n);
    uint32_t (*fA2E0)(uint32_t reader);
    uint32_t (*fAC0A)(uint32_t flags, uint32_t mask);
    uint32_t (*fAE74)(uint32_t code);
    uint32_t (*fB198)(uint32_t v);
    uint32_t (*fA53E)(uint32_t id);
};
static void s8(uint8_t* o, uint32_t k, uint32_t v) { o[k] = (uint8_t)v; }
static void s16(uint8_t* o, uint32_t k, uint32_t v) { uint16_t x = (uint16_t)v; memcpy(o + k, &x, 2); }
static void s32(uint8_t* o, uint32_t k, uint32_t v) { memcpy(o + k, &v, 4); }
extern "C" uint32_t d2game_52bd0(uint8_t* o, uint32_t data, uint32_t reader, const D2G52BD0Seams* s)
{
    memset(o, 0, 0x48);
    s->fA2B6(reader, data, 0x40);
    const uint32_t flags = s->fA2D4(reader, 0x20);
    s32(o, 0x24, flags);
    auto rd = [&](uint32_t n) { return s->fA2D4(reader, n); };
    auto rs = [&](uint32_t n) { return s->fA2C8(reader, n); };
    if (s->fAC0A(flags, 0x100000) != 0) {
        if (s->fAC0A(flags, 0x200000) != 0) {
            s16(o, 0x42, (uint8_t)rd(10));
            s8(o, 0, rd(5));
            s32(o, 2, 1);
            s8(o, 10, rd(3));
            s32(o, 0x18, (uint8_t)rd(5));
            s32(o, 0x1c, (uint8_t)rd(3));
            s8(o, 0x30, rd(8));
            if (s->fAC0A(flags, 0x10000) != 0) {
                s32(o, 0x44, 0x20726165);
                s32(o, 0x20, rd(3));
                s8(o, 0x31, rd(8));
                for (uint32_t i = 0; i < 0x10; i++) s8(o, 0x32 + i, rd(7));
            } else {
                s32(o, 0x44, rd(0x20));
            }
        } else {
            s16(o, 0x42, (uint8_t)rd(10));
            s32(o, 0x44, rd(0x20));
            s8(o, 0, rd(5));
            s8(o, 1, rd(3));
            s32(o, 2, (uint8_t)rd(12));
            s8(o, 10, rd(3));
            s8(o, 11, rd(4));
            s32(o, 0xc, rs(20));
            s32(o, 0x10, (uint8_t)rd(8));
            s32(o, 0x14, (uint8_t)rd(8));
            s32(o, 0x18, (uint8_t)rd(5));
            s32(o, 0x1c, (uint8_t)rd(3));
            s32(o, 0x20, rs(10));
            s32(o, 0x28, rd(0x20));
            s32(o, 0x2c, rd(0x20));
            s8(o, 0x30, rd(8));
        }
        uint32_t code; memcpy(&code, o + 0x44, 4);
        s32(o, 6, s->fAE74(code));
        return s->fA2E0(reader);
    }
    s16(o, 0x42, 0);
    if (s->fAC0A(flags, 0x10000) != 0) {
        s8(o, 0, rd(5));
        s32(o, 2, 1);
        rs(10);
        s32(o, 6, s->fAE74(0x20726165));
        s8(o, 10, rd(3));
        s32(o, 0x18, (uint8_t)rd(5));
        s32(o, 0x1c, (uint8_t)rd(3));
        s32(o, 0x20, rs(10));
        s8(o, 0x30, rd(8));
        s8(o, 0x31, rd(8));
        for (uint32_t i = 0; i < 0x10;) {
            const uint8_t c = (uint8_t)rd(7);
            o[0x32 + i] = c;
            if (c == 0) break;
            i++;
        }
        s32(o, 0x44, 0x20726165);
        return 0x19;
    }
    s8(o, 0, rd(5));
    s8(o, 1, rd(3));
    s32(o, 2, (uint8_t)rd(12));
    const int32_t old = (int32_t)rs(10);
    s8(o, 10, rd(3));
    s8(o, 11, rd(4));
    s32(o, 0xc, rs(20));
    s32(o, 0x10, (uint8_t)rd(8));
    s32(o, 0x14, (uint8_t)rd(8));
    s32(o, 0x18, (uint8_t)rd(5));
    s32(o, 0x1c, (uint8_t)rd(3));
    s32(o, 0x20, rs(10));
    s32(o, 0x28, rd(0x20));
    s32(o, 0x2c, rd(0x20));
    s8(o, 0x30, rd(8));
    if (old >= 0) {
        const uint32_t id = s->fB198((uint32_t)old);
        s32(o, 6, id);
        const uint32_t row = s->fA53E(id);
        if (row != 0) {
            uint32_t v; memcpy(&v, (const void*)(uintptr_t)(row + 0x80), 4);
            s32(o, 0x44, v);
            return 0x19;
        }
    }
    s32(o, 6, 0xffffffffu);
    s32(o, 0x44, 0);
    return 0x19;
}
