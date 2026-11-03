// 1.13c D2Game+0xD1C00: can the item EBX go into / swap with the slot item. EAX = pointer to a unit-type word
// (read, and rewritten through 0xD1720 on a swap), EBX = item unit, ESI = pointer to the slot's item pointer, stack (ret 8) = A1 (the
// owner unit: type at +0, class at +4), A2 = pointer to a word (the cursor item pointer; swapped with *ESI). Returns 0 or 1.
// The thunks are seams: a9b2 = #10543, aa18 = #11144, a430 = #10744 (unit, id), a8fe = #10326, aa60 = #10364 (A1, unit).
#include <cstdint>
#include <cstring>
struct D2GD1C00Seams {
    int32_t  (*fA9B2)(uint32_t unit);
    int32_t  (*fAA18)(uint32_t unit);
    uint32_t (*fA430)(uint32_t unit, uint32_t id);
    uint32_t (*fA8FE)(uint32_t unit);
    uint32_t (*fAA60)(uint32_t a1, uint32_t unit);
};
static uint32_t rd(uint32_t p) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)p, 4); return v; }
static void wr(uint32_t p, uint32_t v) { memcpy((void*)(uintptr_t)p, &v, 4); }
extern "C" uint32_t d2game_d1c00(uint32_t eax, uint32_t ebx, uint32_t esi, uint32_t a1, uint32_t a2, uint32_t (*d1720)(uint32_t), const D2GD1C00Seams* s)
{
    if (a1 == 0 || ebx == 0 || rd(ebx) != 4) return 0;
    uint32_t slot = rd(esi);
    if (rd(a2) == 0) { if (slot == 0) return 1; }
    else if (slot != 0) return 0;
    uint32_t mapped = d1720(rd(eax));
    uint32_t cur = rd(a2);                       // EDX after the call: the old *A2, ECX = slot
    if (cur != 0) { wr(a2, slot); wr(esi, cur); wr(eax, mapped); }
    uint32_t t = rd(eax);
    if (!(t == 4 || t == 5 || t == 0xb || t == 0xc)) return rd(a2) == 0;
    int32_t n = s->fA9B2(ebx);
    if (n > 0) {
        if (rd(esi) == 0) return 1;
        uint32_t v = (uint32_t)s->fA9B2(ebx);
        return s->fA430(rd(esi), v) != 0;
    }
    uint32_t cmpA;
    if (s->fAA18(ebx) != 0) {
        if (rd(esi) == 0) return 1;
        if (s->fA9B2(rd(esi)) <= 0) return 0;
        cmpA = (uint32_t)s->fA9B2(rd(esi));
        return s->fA430(ebx, cmpA) != 0;
    }
    if (s->fA9B2(rd(esi)) > 0) {
        cmpA = (uint32_t)s->fA9B2(rd(esi));
        return s->fA430(ebx, cmpA) != 0;
    }
    if (s->fAA18(rd(esi)) > 0) {
        cmpA = (uint32_t)s->fAA18(rd(esi));
        return s->fA430(ebx, cmpA) != 0;
    }
    if (s->fA8FE(ebx) != 0 && s->fAA60(a1, ebx) == 0) return 0;
    if (s->fA8FE(rd(esi)) != 0 && s->fAA60(a1, rd(esi)) == 0) return 0;
    uint32_t r1 = s->fA430(ebx, 0x2d);
    uint32_t r2 = s->fA430(rd(esi), 0x2d);
    if (r1 == 0 || r2 == 0) return r1 == 0 && r2 == 0 ? 0 : (uint32_t)(rd(a2) == 0);
    if (rd(a1) == 1) return 1;
    uint32_t k = rd(a1 + 4);
    if (k == 4) return 1;
    if (k != 6) return 0;
    if (s->fA430(ebx, 0x43) == 0) return 0;
    if (s->fA430(rd(esi), 0x43) == 0) return 0;
    return 1;
}
