// 1.13c D2Common+0x1B4B0 (retail-layout, 32-bit image): mark the edges of a four-vertex ring that border the given exits.
// stdcall(pool, first, box, direction, orth), ret 0x14. A vertex is x +0, y +4, direction byte +8, flags +0xC, next +0x10 (0x14 bytes).
// The ring is first -> second -> third -> fourth. Each exit node (direction +4, preset flag +8, own box +0x10 or the given box, next +0x14)
// picks one edge: 0 first/vertical/-1, 1 second/horizontal/+1, 2 third/vertical/+1, 3 fourth/horizontal/-1 (above 3 is fatal). With s the
// sign: if hi*s <= start*s the edge already starts inside, and it touches when start*s <= lo*s; else if hi*s <= end*s a vertex is split in at hi
// and the edge touches. A touching edge gets flags |= (preset ? 3 : 1) and, when lo*s < end*s, a further vertex is split in at lo.
// A split vertex (allocated from the pool, zeroed, direction byte stored) takes the edge's other coordinate and links in after the edge's vertex.
#include <cstdint>
#include <cstdlib>
struct D2Common1B4B0Env { uint32_t (*alloc)(uint32_t pool); };
extern "C" void D2Common_1B4B0_SplitVerticesRaw(const D2Common1B4B0Env* e, uint32_t pool, uint32_t first, uint32_t box, uint32_t dir, uint32_t orth)
{
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    auto mul = [](uint32_t a, int32_t s) { return static_cast<int32_t>(a * static_cast<uint32_t>(s)); };
    const uint32_t second = W(first + 0x10), third = W(second + 0x10), fourth = W(third + 0x10);
    for (; orth; orth = W(orth + 0x14)) {
        const uint32_t c = W(orth + 0x10) ? W(orth + 0x10) : box;
        uint32_t v, lo, hi, start, end; bool vertical; int32_t s;
        switch (W(orth + 4)) {
        case 0: v = first;  vertical = true;  s = -1; lo = W(c + 4); hi = W(c + 4) + W(c + 0xC); start = W(first + 4);  end = W(second + 4); break;
        case 1: v = second; vertical = false; s = 1;  lo = W(c) + W(c + 8);  hi = W(c);  start = W(second); end = W(third); break;
        case 2: v = third;  vertical = true;  s = 1;  lo = W(c + 4) + W(c + 0xC); hi = W(c + 4); start = W(third + 4);  end = W(fourth + 4); break;
        case 3: v = fourth; vertical = false; s = -1; lo = W(c);  hi = W(c) + W(c + 8); start = W(fourth); end = W(first); break;
        default: abort();
        }
        auto split = [&](uint32_t pos) {
            const uint32_t n = e->alloc(pool);
            for (uint32_t o = 0; o < 0x14; o += 4) W(n + o) = 0;
            *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(n + 8)) = static_cast<uint8_t>(dir);
            W(n) = vertical ? W(v) : pos; W(n + 4) = vertical ? pos : W(v + 4);
            W(n + 0x10) = W(v + 0x10); W(v + 0x10) = n; return n;
        };
        bool touches = false;
        if (mul(hi, s) <= mul(start, s)) touches = mul(start, s) <= mul(lo, s);
        else if (mul(hi, s) <= mul(end, s)) { v = split(hi); touches = true; }
        if (touches) {
            W(v + 0xC) |= W(orth + 8) ? 3u : 1u;
            if (mul(lo, s) < mul(end, s)) split(lo);
        }
    }
}
