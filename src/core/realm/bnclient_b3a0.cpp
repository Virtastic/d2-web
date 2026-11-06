// 1.13c BNClient+0xB3A0 (394 B, callers 0x11F80, 0x12000): SRP client proof step. EDI = the SRP context, stack (ret 0xC): grpCell (the cell holding the
// group handle block P), salt (32 bytes), B (the server value, 32 bytes). Rule, call for call:
//   h0 = new; from_bytes(h0, B, 0x20); h1 = new; set(h1, 0); r = cmp(h0, h1); del(h1); r != 1 -> del(h0), 0.
//   x = 0xA9D0(EAX = ctx, ESI = ctx+0x20, salt, &x); h2 = new; set(h2, 0); h3 = new; from_bytes(h3, ctx+0x40, 0x20);
//   s = 0xB0C0(EAX = &cell(h0), *grpCell, &cell(h3), &cell(x), &cell(h2)); del(h3); del(h0); del(x); s == 0 -> del(h2), 0.
//   to_bytes(h2, buf, 0x20, &len) (len reuses x's cell); len < 0x20 -> zero buf[len..0x20); 0x9E90(EAX = len, ctx+0x80, buf); del(h2);
//   0xA680(*grpCell, ctx, salt, ctx+0x60, B, ctx+0x80, ctx+0xA8); 1.
// Storm thunks: 0x8740 #624 new, 0x8728 #609 from bytes, 0x872E #612 set, 0x871C #603 cmp, 0x870A #606 del, 0x8716 #638 to bytes; #494 (0x806C) zero fill.
#include <cstdint>
#include <cstring>
struct BnB3a0Env {
    void (*big_new)(uint32_t* slot);                                               // 0x8740
    void (*big_from)(uint32_t h, const uint8_t* p, uint32_t n);                    // 0x8728
    void (*big_set)(uint32_t h, uint32_t v);                                       // 0x872E
    int32_t (*big_cmp)(uint32_t a, uint32_t b);                                    // 0x871C
    void (*big_del)(uint32_t h);                                                   // 0x870A
    void (*big_to)(uint32_t h, uint8_t* buf, uint32_t n, uint32_t* len);           // 0x8716
    void (*srp_x)(uint8_t* ctx, uint8_t* ctx20, const uint8_t* salt, uint32_t* out); // 0xA9D0 (EAX, ESI, stack salt, out)
    uint32_t (*srp_s)(uint32_t* vcell, uint32_t grp, uint32_t* a1, uint32_t* c, uint32_t* r); // 0xB0C0 (EAX = vcell)
    void (*srp_hash)(uint32_t len, uint8_t* out, const uint8_t* buf);              // 0x9E90 (EAX = len)
    void (*srp_m1)(uint32_t grp, uint8_t* ctx, const uint8_t* salt, uint8_t* c60, const uint8_t* b, uint8_t* c80, uint8_t* ca8); // 0xA680
};
extern "C" uint32_t d2_bnclient_b3a0(uint8_t* ctx, const uint32_t* grpCell, const uint8_t* salt, const uint8_t* b, const BnB3a0Env* e) {
    uint32_t slot = 0;
    e->big_new(&slot);
    e->big_from(slot, b, 0x20);
    uint32_t h0 = slot, vcell = h0;
    e->big_new(&slot);
    e->big_set(slot, 0);
    const int32_t r = e->big_cmp(h0, slot);
    e->big_del(slot);
    if (r != 1) { e->big_del(h0); return 0; }
    uint32_t xcell = 0;
    e->srp_x(ctx, ctx + 0x20, salt, &xcell);
    e->big_new(&slot);
    e->big_set(slot, 0);
    uint32_t h2 = slot, rcell = h2;
    e->big_new(&slot);
    e->big_from(slot, ctx + 0x40, 0x20);
    const uint32_t h3 = slot;
    const uint32_t s = e->srp_s(&vcell, *grpCell, &slot, &xcell, &rcell);
    e->big_del(h3);
    e->big_del(h0);
    e->big_del(xcell);
    if (s == 0) { e->big_del(h2); return 0; }
    uint8_t buf[0x20];
    e->big_to(h2, buf, 0x20, &xcell);
    const uint32_t len = xcell;
    if (len < 0x20) std::memset(buf + len, 0, 0x20 - len);
    e->srp_hash(len, ctx + 0x80, buf);
    e->big_del(h2);
    e->srp_m1(*grpCell, ctx, salt, ctx + 0x60, b, ctx + 0x80, ctx + 0xA8);
    return 1;
}
