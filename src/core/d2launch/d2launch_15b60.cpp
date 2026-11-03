// 1.13c D2Launch+0x15B60 (sub_15B60). No args, plain ret. Loads the realm/gateway list screen:
//  sub_14C60(); thunk 0x99A0(module+0x11b70);
//  list := (DAT_6fa656c4 ? (DAT_6fa65530 := (*DAT_6fa656c4)->vfn[0x40/4](ECX = 0)) : DAT_6fa65530);
//  frees the first control table (thunk 0x9922 on each slot of DAT_6fa653c0, count DAT_6fa65e34 re-read per step), count := 0;
//  list == 0: DAT_6fa65e78 := -1, return.
//  DAT_6fa65e78 := list[1] (n); n >= 0x200 is the fatal box + _exit (not reproduced); sub_12070();
//  n != 0: DAT_6fa65e74 := Fog alloc 4n (thunk 0x97DE: ECX size, EDX = module+0x1e068 file, stack line 0x1173, 0), zeroed;
//  per entry (from list+8: a 4-byte prefix, then two NUL-terminated strings): an 8-byte pair (alloc line 0x1179), each string copied
//  into its own allocation (lines 0x1186, 0x118B; copy thunk 0x9790 dst, src, len incl. NUL), then sub_12140(pair);
//  qsort(DAT_6fa65e74, DAT_6fa65e78, 4, module+0x12020) (cdecl CRT 0x1120). n < 0 skips the loop but still sorts (not captured).
#include <cstdint>

struct D2l15b60Seams {
    void (*f14c60)();
    void (*imp99a0)(uint32_t a);
    uint32_t (*vcall40)(uint32_t obj);
    void (*imp9922)(uint32_t* slot);
    void (*f12070)();
    uint32_t (*imp97de)(uint32_t size, uint32_t file, uint32_t line, uint32_t z);
    void (*imp9790)(uint32_t dst, uint32_t src, uint32_t len);
    void (*f12140)(uint32_t pair);
    void (*crt1120)(uint32_t base, uint32_t n, uint32_t w, uint32_t cmp);
};

static inline uint8_t* d2l15b60P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }

extern "C" void d2l_15b60(uint32_t* g256c4, uint32_t* g25530, uint32_t* count1, uint32_t* table1, int32_t* g25e78, uint32_t* g25e74,
                          uint32_t modbase, const D2l15b60Seams* s) {
    s->f14c60();
    s->imp99a0(modbase + 0x11b70);
    uint32_t list;
    if (*g256c4) { list = s->vcall40(*g256c4); *g25530 = list; }
    else list = *g25530;
    for (int32_t i = 0; i < (int32_t)*count1; i++) s->imp9922(&table1[i]);
    *count1 = 0;
    if (!list) { *g25e78 = -1; return; }
    int32_t n = *(int32_t*)d2l15b60P(list + 4);
    *g25e78 = n;
    uint32_t src = list + 8;
    s->f12070();
    n = *g25e78;
    if (!n) return;
    const uint32_t file = modbase + 0x1e068;
    uint32_t tab = s->imp97de(4u * (uint32_t)n, file, 0x1173, 0);
    for (uint32_t q = 0; q < 4u * (uint32_t)n; q++) d2l15b60P(tab)[q] = 0;
    *g25e74 = tab;
    for (int32_t i = 0; i < n; i++) {
        uint32_t pair = s->imp97de(8, file, 0x1179, 0);
        src += 4;
        uint32_t l = 1; while (d2l15b60P(src)[l - 1]) l++;
        uint32_t a = s->imp97de(l, file, 0x1186, 0);
        ((uint32_t*)d2l15b60P(pair))[0] = a;
        s->imp9790(a, src, l);
        src += l;
        uint32_t m = 1; while (d2l15b60P(src)[m - 1]) m++;
        uint32_t b = s->imp97de(m, file, 0x118b, 0);
        ((uint32_t*)d2l15b60P(pair))[1] = b;
        s->imp9790(b, src, m);
        s->f12140(pair);
        src += m;
    }
    s->crt1120(*g25e74, (uint32_t)*g25e78, 4, modbase + 0x12020);
}
