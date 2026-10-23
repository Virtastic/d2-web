#include <cstdint>
// 1.13c D2Gfx+0x0C28D
// sub_C28D: CallCatchBlock cleanup funclet. EBP-frame slots and ESI (EXCEPTION_RECORD*)
// are passed explicitly; callees are caller-supplied seams.
struct D2GfxC28dHost {
    void (*find_and_unlink_frame)(uint32_t frame);
    uint8_t* (*getptd)(void);                 // ptd+0x0C _unexpected, +0x10 _translator? see spec
    int (*is_exception_object_to_be_destroyed)(uint32_t obj);
    void (*destruct_exception_object)(uint32_t rec);
};
extern "C"
void d2gfx_sub_c28d(uint8_t* mem, const D2GfxC28dHost* h, uint32_t edi, uint32_t ebp, uint32_t rec,
                    uint32_t unexpected_off, uint32_t translator_off) {
    *(uint32_t*)(mem + edi - 4) = *(uint32_t*)(mem + ebp - 0x38);
    h->find_and_unlink_frame(*(uint32_t*)(mem + ebp - 0x3c));
    *(uint32_t*)(h->getptd() + unexpected_off) = *(uint32_t*)(mem + ebp - 0x40);
    *(uint32_t*)(h->getptd() + translator_off) = *(uint32_t*)(mem + ebp - 0x44);
    const uint32_t* r = (const uint32_t*)(mem + rec);
    if (r[0] == 0xE06D7363u && r[4] == 3 && (r[5] == 0x19930520u || r[5] == 0x19930521u) &&
        *(int32_t*)(mem + ebp - 0x48) == 0 && *(int32_t*)(mem + ebp - 0x20) != 0 &&
        h->is_exception_object_to_be_destroyed(r[6]))
        h->destruct_exception_object(rec);   // __abnormal_termination() then ___DestructExceptionObject
}
