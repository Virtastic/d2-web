#include <cstdint>
#include <cstring>

// Guest-address host seam (retail uint32 addresses); each hook is the sibling D2Gfx routine.
struct D2GfxBcoHost {
    uint32_t (*rd32)(uint32_t a);
    uint8_t (*rd8)(uint32_t a);
    void (*wr32)(uint32_t a, uint32_t v);
    uint32_t (*validate_read)(uint32_t p, uint32_t n);   // 0xC986-family
    uint32_t (*validate_write)(uint32_t p, uint32_t n);  // 0xC9A2
    uint32_t (*validate_execute)(uint32_t p);            // 0xC96A
    uint32_t (*adjust_pointer)(uint32_t obj, uint32_t pmd_addr);  // 0x0C119
    void (*memmove_)(uint32_t dst, uint32_t src, uint32_t n);     // 0x61E0
    void (*call_member_fn1)(uint32_t pthis, uint32_t fn);         // 0x0BBEE
    void (*inconsistency)();                                      // 0x0C878
};

// 1.13c D2Gfx+0x0C2FC (BuildCatchObject, VS2003 CRT): copy/construct the thrown object into the
// catch parameter slot. handler = HandlerType, catchable = CatchableType, frame = EDX base.
// Exception record +0x18 = thrown object. Validation failure -> _inconsistency().
extern "C" void d2gfx_build_catch_object(const D2GfxBcoHost* h, uint32_t exc, uint32_t handler,
                                         uint32_t catchable, uint32_t frame) {
    uint32_t type = h->rd32(handler + 4);
    if (!type || !h->rd8(type + 8)) return;
    if (!h->rd32(handler + 8) && !(h->rd8(handler + 3) & 0x80)) return;
    uint32_t adj = h->rd32(handler);
    uint32_t dst = frame;
    if ((int32_t)adj >= 0) dst = h->rd32(handler + 8) + 0xc + frame;
    uint32_t src = h->rd32(exc + 0x18);
    uint32_t pmd = catchable + 8, size = h->rd32(catchable + 0x14), copy = h->rd32(catchable + 0x18);
    bool ok = h->validate_read(src, 1) && h->validate_write(dst, 1);
    if (ok && (adj & 8)) {  // by reference: store (adjusted) pointer
        h->wr32(dst, h->adjust_pointer(src, pmd));
        return;
    }
    if (ok) {
        if (h->rd8(catchable) & 1) {  // simple type: bitwise copy
            h->memmove_(dst, src, size);
            if (size == 4 && h->rd32(dst)) h->wr32(dst, h->adjust_pointer(h->rd32(dst), pmd));
            return;
        }
        if (!copy) { h->memmove_(dst, h->adjust_pointer(src, pmd), size); return; }
        if (h->validate_execute(copy)) {
            h->call_member_fn1(dst, copy);
            return;
        }
    }
    h->inconsistency();
}
