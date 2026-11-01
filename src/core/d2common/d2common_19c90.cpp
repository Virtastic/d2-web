// 1.13c D2Common+0x19C90 (retail-layout, 32-bit image): wire two levels of a DRLG together through a connecting pair of exits.
// AL = act selector (0..4 call the act's setup routine 0x17AE0 / 17AA0 / 17A00 / 17990 / 17930 with the DRLG, other values call none),
// stack (drlg, idA, idB), ret 0xC. Both levels come from the DRLG's list (+0x47C, next +0x1AC, id +0x1D0) or from 0x2D9B0; 0x2E360 runs
// for one whose +0x10 is still zero. Then up to 8 passes: the n-th slot of A's neighbour table (0x44CE0: 8 ids) that holds idB and the n-th
// slot of B's table that holds idA (either missing ends the function); 0x19950 is told each level's (+0x1B4, id); in each level's node list
// (+0x10, next +0x24) the first node whose flag word (+0x28) has bit slot+4 is the exit. For each of the two exits: flag 0x1000000 clear ->
// 0x68400(exit); flag 0x2000000 clear and +0x48 == 2 -> 0xBF70(exit); flag 0x100000 clear -> 0x7C0E0(exit, [[exit+0x58]+0x1B4]+0x478, [exit+0x58]).
// Each exit not yet marked gets flag 0x4000000 (an exit with 0x40000 is left alone); the walk of its +0x5C list for type 5 has no effect.
#include <cstdint>
struct D2Common19C90Env {
    void (*act[5])(uint32_t drlg);
    uint32_t (*lookup)(uint32_t drlg, uint32_t id);                    // 0x2D9B0
    void (*setup)(uint32_t level);                                     // 0x2E360
    uint32_t (*table)(uint32_t drlg, uint32_t id);                     // 0x44CE0: 8 neighbour ids
    void (*note)(uint32_t value1B4, uint32_t id);                      // 0x19950
    void (*f68400)(uint32_t exit);
    void (*fBF70)(uint32_t exit);
    void (*f7C0E0)(uint32_t exit, uint32_t ecx, uint32_t edx);
};
extern "C" void D2Common_19C90_LinkLevelsRaw(const D2Common19C90Env* e, uint32_t sel, uint32_t drlg, int32_t idA, int32_t idB)
{
    auto P = [](uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); };
    auto W = [&](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(P(a)); };
    if ((sel & 0xFF) <= 4) e->act[sel & 0xFF](drlg);
    auto find = [&](int32_t id) {
        uint32_t l = W(drlg + 0x47C);
        while (l && static_cast<int32_t>(W(l + 0x1D0)) != id) l = W(l + 0x1AC);
        if (!l) l = e->lookup(drlg, static_cast<uint32_t>(id));
        if (!W(l + 0x10)) e->setup(l);
        return l;
    };
    const uint32_t a = find(idA), b = find(idB);
    for (int32_t n = 0; n < 8; ++n) {
        auto nth = [&](int32_t from, int32_t want) -> int32_t {
            const uint32_t t = e->table(drlg, static_cast<uint32_t>(from));
            int32_t k = 0;
            for (uint32_t i = 0; i < 8; ++i)
                if (static_cast<int32_t>(W(t + 4 * i)) == want) { if (k == n) return static_cast<int32_t>(i); ++k; }
            return -1;
        };
        const int32_t sa = nth(idA, idB), sb = nth(idB, idA);
        if (sa == -1 || sb == -1) return;
        e->note(W(a + 0x1B4), W(a + 0x1D0));
        e->note(W(b + 0x1B4), W(b + 0x1D0));
        auto exitOf = [&](uint32_t level, int32_t slot) {
            uint32_t x = W(level + 0x10);
            while (x && !(W(x + 0x28) & (1u << ((slot + 4) & 31)))) x = W(x + 0x24);
            return x;
        };
        const uint32_t xa = exitOf(a, sa), xb = exitOf(b, sb);
        const uint32_t xs[2] = { xa, xb };
        for (const uint32_t x : xs) {
            if (!(W(x + 0x28) & 0x1000000)) e->f68400(x);
            if (!(W(x + 0x28) & 0x2000000) && W(x + 0x48) == 2) e->fBF70(x);
            if (!(W(x + 0x28) & 0x100000)) { const uint32_t p = W(x + 0x58); e->f7C0E0(x, W(W(p + 0x1B4) + 0x478), p); }
        }
        for (const uint32_t x : xs) {
            const uint32_t f = W(x + 0x28);
            if ((f & 0x40000) || !(f & 0x4000000)) {   // the walk after it only reads
                if (!(f & 0x40000)) W(x + 0x28) = f | 0x4000000;
            }
        }
    }
}
