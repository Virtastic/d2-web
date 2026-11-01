// 1.13c D2Common+0x1B6E0 (retail-layout, 32-bit image): build the four-vertex ring around a box and mark the edges bordering the given exits.
// AL = direction byte stored in every new vertex, ECX = box (x +0, y +4, w +8, h +0xC), stack (pool, out, orth), ret 0xC.
// The box (and the box of each exit node: +0x10, next +0x14) is shrunk by one in width and height, four vertices (0x14 bytes from the pool,
// zeroed, direction byte at +8) are made at (x, y+h) (x, y) (x+w, y) (x+w, y+h) and chained in a ring (next +0x10), *out = the first;
// 0x1B4B0 then splits/marks edges; every ring vertex is made relative to the box origin; the box and exit boxes are widened by one again.
#include <cstdint>
struct D2Common1B4B0Env { uint32_t (*alloc)(uint32_t pool); };
extern "C" void D2Common_1B4B0_SplitVerticesRaw(const D2Common1B4B0Env*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern "C" void D2Common_1B6E0_CreateVerticesRaw(const D2Common1B4B0Env* e, uint32_t dir, uint32_t box, uint32_t pool, uint32_t out, uint32_t orth)
{
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    auto adj = [&](int32_t d) {
        W(box + 8) += d; W(box + 0xC) += d;
        for (uint32_t o = orth; o; o = W(o + 0x14)) { W(W(o + 0x10) + 8) += d; W(W(o + 0x10) + 0xC) += d; }
    };
    adj(-1);
    auto mk = [&]() {
        const uint32_t n = e->alloc(pool);
        for (uint32_t k = 0; k < 0x14; k += 4) W(n + k) = 0;
        *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(n + 8)) = static_cast<uint8_t>(dir);
        return n;
    };
    const uint32_t v1 = mk(); W(out) = v1;
    W(v1) = W(box); W(v1 + 4) = W(box + 4) + W(box + 0xC);
    const uint32_t v2 = mk(); W(v1 + 0x10) = v2;
    W(v2) = W(box); W(v2 + 4) = W(box + 4);
    const uint32_t v3 = mk(); W(v2 + 0x10) = v3;
    W(v3) = W(box) + W(box + 8); W(v3 + 4) = W(box + 4);
    const uint32_t v4 = mk(); W(v3 + 0x10) = v4;
    W(v4) = W(box) + W(box + 8); W(v4 + 4) = W(box + 4) + W(box + 0xC);
    W(v4 + 0x10) = W(out);
    D2Common_1B4B0_SplitVerticesRaw(e, pool, W(out), box, dir, orth);
    const uint32_t start = W(out);
    uint32_t v = start;
    do { W(v) -= W(box); W(v + 4) -= W(box + 4); v = W(v + 0x10); } while (v != start);
    adj(1);
}
