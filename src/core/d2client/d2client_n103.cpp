// Skill-description formatter dispatch, retail D2Client sub_316C0. Switch on the byte at
// calc[0x42+k]; the sibling formatters it tail-calls stay behind a host seam (Ops.call) addressed by
// retail RVA. Cases that only build strings from Ordinal_10003 + strcat/sprintf chains go through
// Ops.compose(case) -- their string ids are register args the decompile does not keep.
#include <cstdint>
namespace d2client_n103 {
using U = uint32_t;
struct Args { U p, a, s1, s2, u7, u9, u4, u5; void *stk0, *stk1, *stk2; int32_t lvl; };
struct Ops {
    void* (*rec17F0)(U);                          // FUN_6fab17f0 (non-null gate)
    void* (*rec1580)(U);                          // FUN_6fab1580
    U (*calc)(U unit, U expr);                    // Ordinal_10435
    U (*call)(U rva, const Args*, U lit0, U lit1, U lit2); // sibling formatter by RVA
    U (*compose)(U sw, const Args*);              // strcat/sprintf chains; 1 on success
    U (*dmg)(U sw, const Args*);                  // 0x31: Ordinal_10567/10297 pair
    U (*statFmt)(U rva);                          // 0x35-0x38: FUN_6faecf50/f00/eb0/e60 -> 0x2C950
};
struct Row { U sw, rva, n; };
// case -> callee; remaining args are chosen below by sw
static const Row kPure[] = {
 {0x14,0x2D8E0,0},{0x15,0x2D8E0,0},{0x1f,0x2E330,0},{0x22,0x2E410,0},{0x27,0x2E900,0},{0x32,0x2EF20,0},
 {1,0x314D0,0},{2,0x2D9B0,1},{3,0x2D9B0,0},{4,0x2DCA0,1},{5,0x2DCA0,0},{6,0x2DBF0,1},{7,0x2DBF0,0},
 {0x10,0x2DF50,0},{0x16,0x2EDD0,0},{0x24,0x2DA70,0},{0x25,0x2F340,0},{0x26,0x2F6E0,0},
 {0x29,0x2E770,0},{0x2a,0x2CD20,1},{0x2b,0x2DE00,0},{0x2c,0x2DE00,0},{0x2d,0x2E6E0,0},{0x2f,0x2F520,0},
 {0x30,0x30270,0},{0x33,0x2DD50,0},{0x34,0x2F6E0,1},{0x39,0x2E060,0},{0x3a,0x2F600,1},{0x3e,0x2F600,0},
 {0x3f,0x2CE10,1},{0x40,0x2CC00,1},{0x41,0x2CB60,0},{0x42,0x2C9F0,0},{0x43,0x2CE10,0},{0x44,0x2DB30,0},
 {0x45,0x2CA80,0},
};
// 1.13c D2CLIENT+0x316C0
// Dispatch on an already-read switch byte `sw`; v4/v5 = Ordinal_10435 results, s1/s2 the two 16-bit ids.
U dispatch(U sw, Args* a, const Ops* o) {
    const U N = 0x1506;
    if (sw == 0x3c || sw == 0x3d) { U v5 = a->u5 ? a->u5 : 10; return o->call(0x2F870, a, sw == 0x3c, v5, 0) , 1; }
    if (sw == 0x17 || sw == 0x1e) { if (!o->rec1580(0)) return 0; return o->call(0x2E060, a, 1, 0, 0), 1; }
    if (sw == 0x1c) { if (!o->rec1580(0)) return 0; return o->call(0x2F430, a, 0, 0, 0), 1; }
    if (sw == 0x1d) { if (!o->rec1580(0)) return 0; return o->call(0x2DBF0, a, 0x10b6, 0, 0), 1; }
    if (sw == 0x21 && a->lvl < 2) return 1;
    if (sw == 0x35 || sw == 0x36 || sw == 0x37 || sw == 0x38) { o->call(0x2C950, a, o->statFmt(sw), 0, 0); return 1; }
    if (sw == 0x31) return o->dmg(sw, a);
    if (sw == 0x2e && !a->u5) return 0;
    if ((sw == 0x28 || sw == 0x2e || sw == 0x4a || sw == 0x4b) && a->s1 == N) return 0;
    if (sw == 0x47 && (a->s1 == N || a->s2 == N)) return 0;
    if ((sw == 0x48 || sw == 0x49) && (!(sw == 0x48 ? a->u4 : a->u4) || (int32_t)a->u5 < 1)) return 0;
    for (const Row& r : kPure)
        if (r.sw == sw) return o->call(r.rva, a, r.n, 0, 0), 1;
    return (sw >= 8 && sw <= 0x4b) ? o->compose(sw, a) : 0;
}
}
