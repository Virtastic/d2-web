// 1.13c D2Common+0x7E8C0: EAX level; classifies cliff runs on the outdoor vertex ring, no grid placement.
// Retail's 32-bit layout is read directly (wasm64 pointers are twice as wide as the retail fields):
//   level +0x14 outdoor info; outdoor +0x00 flags, +0x64 ring head;
//   vertex +0x00 x, +0x04 y, +0x08 direction byte, +0x0C flags (bit 0 = fixed), +0x10 next.
// A run starts at a vertex whose edge rises in x with the predecessor below it (or falls in y with the
// predecessor to its right), neither end flagged. The walk follows edges that keep y non-increasing and
// x non-decreasing, remembering the last vertex before a convex turn; a found corner marks the run's
// direction bytes 1 from the start through it and sets outdoor flag 0x20. EAX on return is the vertex after
// the last one walked, which the probe compares.
#include <cstdint>

namespace {
inline uint8_t* P7E8C0(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
inline int32_t X(uint32_t v) { return *reinterpret_cast<int32_t*>(P7E8C0(v)); }
inline int32_t Y(uint32_t v) { return *reinterpret_cast<int32_t*>(P7E8C0(v) + 4); }
inline bool Fixed(uint32_t v) { return (P7E8C0(v)[0x0C] & 1) != 0; }
inline uint32_t Next(uint32_t v) { return *reinterpret_cast<uint32_t*>(P7E8C0(v) + 0x10); }
}

extern "C" uint32_t D2Common_7E8C0_ClassifyCliffRuns(uint32_t level)
{
    const uint32_t out = *reinterpret_cast<uint32_t*>(P7E8C0(level) + 0x14);
    uint32_t* outFlags = reinterpret_cast<uint32_t*>(P7E8C0(out));
    const uint32_t* head = reinterpret_cast<uint32_t*>(P7E8C0(out) + 0x64);
    uint32_t vertex = *head;
    uint32_t first = vertex;
    uint32_t previous = vertex;
    for (uint32_t p = Next(vertex); p != vertex; p = Next(p))
        previous = p;
    bool crossed = false;
    for (;;) {
        uint32_t walk = vertex;
        const uint32_t nx = Next(vertex);
        if ((X(vertex) < X(nx) && Y(vertex) < Y(previous) && !Fixed(vertex) && !Fixed(previous)) ||
            (Y(nx) < Y(vertex) && X(vertex) < X(previous) && !Fixed(vertex) && !Fixed(previous))) {
            uint32_t corner = 0;
            uint32_t step;
            do {
                if (walk == first)
                    crossed = true;
                step = Next(walk);
                if (Y(walk) < Y(step) || X(step) < X(walk) || Fixed(walk) || Fixed(step))
                    break;
                const uint32_t after = Next(step);
                if ((X(walk) < X(step) && Y(step) < Y(after) && !Fixed(step)) ||
                    (Y(step) < Y(walk) && X(step) < X(after) && !Fixed(step)))
                    corner = walk;
                walk = step;
            } while (step != vertex);
            if (corner) {
                for (; vertex != corner; vertex = Next(vertex))
                    P7E8C0(vertex)[0x08] = 1;
                P7E8C0(vertex)[0x08] = 1;
                *outFlags |= 0x20;
            }
        }
        vertex = Next(walk);
        if (crossed)
            return vertex;
        first = *head;
        previous = walk;
        if (vertex == first)
            return vertex;
    }
}
