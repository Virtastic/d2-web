// 1.13c Storm+0x22420 -- adaptive-Huffman decode loop (Storm huffman decompressor).
// The decompile's implicit-register state: ESI = bit reader, EBX/this = tree object (first dword = "static tree" flag).
#include <cstdint>
struct Storm22420Bits { const uint16_t* p; uint32_t bits; uint32_t cnt; };
struct Storm22420Env {
    void (*setup)(void* tree, uint8_t mode);       // Storm+0x21E40
    uint32_t (*readSym)(void* tree);               // Storm+0x4980
    void (*addSym)(void* tree, uint32_t sym);      // Storm+0x21CB0
    void (*adapt)(void* tree);                     // Storm+0x4810
};
static uint32_t byte22420(Storm22420Bits& b) {
    if (b.cnt < 9) { b.bits |= (uint32_t)*b.p++ << (b.cnt & 31); b.cnt += 16; }
    uint32_t v = b.bits & 0xff; b.bits >>= 8; b.cnt -= 8; return v;
}
int32_t storm_sub_22420(const Storm22420Env& e, uint32_t* tree, Storm22420Bits& b, uint8_t* dst, int32_t cap) {
    if (cap == 0) return 0;
    uint8_t mode = (uint8_t)byte22420(b);
    e.setup(tree, mode);
    *tree = (mode == 0);
    uint8_t* out = dst;
    for (;;) {
        uint32_t s = e.readSym(tree);
        if (s == 0x101) {
            e.addSym(tree, byte22420(b));
            if (*tree == 0) e.adapt(tree);
        }
        if (s == 0x100) break;
        *out++ = (uint8_t)s;
        if (--cap == 0) break;
        if (*tree != 0) e.adapt(tree);
    }
    return (int32_t)(out - dst);
}
