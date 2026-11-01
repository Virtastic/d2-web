// 1.13c D2Common+0x19B60 (retail-layout, 32-bit image): pick a random entry of a level's 12-byte list that matches a mode key.
// thiscall-like: ECX = level, EBX = mode key, stack (pY out, pX out), ret 8. Entries sit at level+0x2C (words +0x2C, +0x30, key +0x34),
// count at +0x1D8, a 64-bit seed at +0x1C4 / +0x1C8 (next = lo * 0x6AC690C5 + hi, kept as 64 bits). An entry matches when its key is the
// mode, or when the mode's flag word in the 8-byte table at 0x94F48 is nonzero and both keys share its group word. With n matches the
// pick is ((n - 1) & seed) + 1 for a power of two, else seed % n + 1 (no match: 0, no seed step). Then 0x2E5C0 gets (word0, word1,
// level+0x1B4, level) with EAX = 0.
#include <cstdint>
struct D2Common19B60Env {
    const uint32_t* table;                                                           // 0x94F48: {flag, group} per key
    void (*finish)(uint32_t word0, uint32_t word1, uint32_t value1B4, uint32_t level);   // 0x2E5C0
};
extern "C" void D2Common_19B60_PickEntryRaw(const D2Common19B60Env* e, uint32_t level, int32_t mode, uint32_t pY, uint32_t pX)
{
    auto P = [](uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); };
    auto W = [&](uint32_t a) -> int32_t& { return *reinterpret_cast<int32_t*>(P(a)); };
    const int32_t count = W(level + 0x1D8);
    auto match = [&](int32_t key) { return mode == key || (e->table[mode * 2] != 0 && e->table[mode * 2 + 1] == e->table[key * 2 + 1]); };
    uint32_t n = 0;
    for (int32_t i = 0; i < count; ++i) n += match(W(level + 0x34 + 12 * i));
    int32_t pick = 0;
    if (n) {
        const uint64_t s = static_cast<uint64_t>(static_cast<uint32_t>(W(level + 0x1C4))) * 0x6AC690C5ull + static_cast<uint32_t>(W(level + 0x1C8));
        *reinterpret_cast<uint64_t*>(P(level + 0x1C4)) = s;
        pick = (n & (n - 1)) == 0 ? static_cast<int32_t>(((n - 1) & static_cast<uint32_t>(s)) + 1) : static_cast<int32_t>(static_cast<uint32_t>(s) % n + 1);
    }
    int32_t i = 0;
    for (; i < count; ++i) {
        if (match(W(level + 0x34 + 12 * i))) --pick;
        if (pick < 1) break;
    }
    W(pY) = W(level + 0x2C + 12 * i);
    W(pX) = W(level + 0x30 + 12 * i);
    e->finish(static_cast<uint32_t>(W(pY)), static_cast<uint32_t>(W(pX)), static_cast<uint32_t>(W(level + 0x1B4)), level);
}
