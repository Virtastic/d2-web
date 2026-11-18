// 1.13c BNClient+0x0A4E0 -- install the SRP group. ESI = pointer to the group pointer; the group is {ready, g (+4), n (+8)}
// (32-bit handles). Stack (ret 0x10): a0, a1, a2, a3. With the group ready, the old n (+8) then g (+4) are released
// (Storm #606). Then g := SBigFromBinary(new, a1, a0) and n := SBigFromBinary(new, a3, a2) (#624 creates each handle
// through a pointer, #609 loads it), ready := 1, and 1 is returned (the trailing "not ready" release is dead).
#include <cstdint>

struct BnA4e0Host {
    void (*big_new)(uint32_t* slot);                                   // 0x8740 #624
    void (*big_from_binary)(uint32_t h, const void* data, uint32_t n); // 0x8728 #609 (retail stack order: handle, data, length)
    void (*big_del)(uint32_t h);                                       // 0x870A #606
};

extern "C" uint32_t bn_srp_group_a4e0(uint32_t* grp, uint32_t a0, const void* a1, uint32_t a2, const void* a3, const BnA4e0Host* h) {
    if (grp[0]) { h->big_del(grp[2]); h->big_del(grp[1]); }
    uint32_t g = 0;
    h->big_new(&g);
    h->big_from_binary(g, a1, a0);
    grp[1] = g;
    uint32_t n = 0;
    h->big_new(&n);
    h->big_from_binary(n, a3, a2);
    grp[2] = n;
    grp[0] = 1;
    return 1;
}
