// 1.13c Storm+0x271D0: merge request node b (ESI) into the preceding node a (EAX) when b is the
// contiguous continuation (same +0x24 key, a.+0x10 + b.+0x34 == ... see spec). Then unlink + free b.
#include <cstdint>
void Storm_Sub4040(int* node);                                  // Storm+0x4040 (unlink)
void sub_1B960(int* esi, const char* name, int n);              // Storm+0x1B960 (SMemFree)

// 1.13c Storm+0x271D0
void Storm_Sub271D0(int* a /*EAX*/, int* b /*ESI*/) {
    if (b[9] != a[9]) return;
    if (b[4] + b[0xd] != a[4]) return;
    if ((uint32_t)(b[0xd] + a[0xd]) > 0x100000) return;
    if (b[0xe] == 0) return;
    if (b[0x10] || b[0xf] || b[0xc] || a[0xf] || b[10] || a[0xc] || a[10] || b[2]) return;
    a[4] = b[4]; a[5] = b[5]; a[6] = b[6]; a[8] = b[8];
    a[0xd] = b[0xd] + a[0xd];
    Storm_Sub4040(b);
    sub_1B960(b, "delete", -1);
}
