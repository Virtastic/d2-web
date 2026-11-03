// 1.13c D2Launch+0x15D30 (sub_15D30). No arguments, plain ret. Frees the second control table (DAT_6fa65e30 entries at 0x252b8, D2Win#10050 on
// each slot address), clears 25e38/25e3c/25e30, frees the first table (DAT_6fa65e34 entries at 0x253c0), clears 25e34, D2Win#10006(0). If
// DAT_6fa65e5c is set: sub_12BF0 (EAX = "palette\fechar\pal.pl2" at 0x1e9c8, stack 0x1e9b0) and 25e5c := 0. Then sub_AE60, sub_BDF0, D2Win#10149(1).
// The counts are re-read every iteration, as retail does. t2/t1 are the tables' addresses as the callees see them; sPal/sName the string addresses.
#include <cstdint>

struct D2l15d30Seams { void (*win10050)(uint32_t); void (*win10006)(uint32_t); void (*f12bf0)(uint32_t, uint32_t); void (*fae60)(); void (*fbdf0)(); void (*win10149)(uint32_t); };

extern "C" void d2l_15d30_core(int32_t* g5e30, int32_t* g5e34, int32_t* g5e38, int32_t* g5e3c, int32_t* g5e5c, uint32_t t2, uint32_t t1,
                               uint32_t sArg, uint32_t sEax, const D2l15d30Seams* s) {
    for (int32_t i = 0; i < *g5e30; i++) s->win10050(t2 + 4u * (uint32_t)i);
    *g5e38 = 0; *g5e3c = 0; *g5e30 = 0;
    for (int32_t i = 0; i < *g5e34; i++) s->win10050(t1 + 4u * (uint32_t)i);
    *g5e34 = 0;
    s->win10006(0);
    if (*g5e5c != 0) { s->f12bf0(sEax, sArg); *g5e5c = 0; }
    s->fae60();
    s->fbdf0();
    s->win10149(1);
}
