// D2Gfx VC7.1 CRT FindHandler, read against the cached 1.13c decompile (alpha lane: no vector).
// Seams: the CRT callees are caller-supplied. CatchIt (RtlUnwind + a call into the handler's x86 frame)
// is a seam, not reproduced; the decompile loses several register args of CatchIt/TypeMatch.
#include <cstdint>

struct D2GfxFindHandlerSeams {
    uint32_t (*rethrow_record)(void* ctx, uint32_t* ctx_out);        // __getptd()->_unexpected / _translator, 0 = none
    int  (*validate_read)(uint32_t p);                                 // _ValidateRead(p,1)
    void (*inconsistency)();                                           // _inconsistency
    void (*get_range)(uint32_t* lo, uint32_t* hi);                     // _GetRangeOfTrysToCheck
    int  (*type_match)(uint32_t handler, uint32_t catchable, uint32_t throwinfo);
    void (*catch_it)(uint32_t handler);
    void (*destruct_exception_object)(uint32_t rec);
    void (*foreign)();                                                 // FindHandlerForForeignException
    void (*terminate)();
};

// 1.13c D2Gfx+0x0C59D FindHandler. rd(a) reads a guest dword. Try table entry = 20 bytes:
// tryLow, tryHigh, catchHigh, nCatches, pHandlerArray. state = *(int*)(regnode+8).
extern "C"
void d2gfx_find_handler(uint8_t* mem, uint32_t rec, uint32_t regnode, uint32_t funcinfo,
                        uint8_t is_rethrow_ok, const D2GfxFindHandlerSeams* s) {
    auto rd = [&](uint32_t a) { return *(int32_t*)(mem + a); };
    int32_t state = rd(regnode + 8);
    if (state < -1 || rd(funcinfo + 4) <= state) s->inconsistency();   // maxState
    auto isCpp = [&](uint32_t r) {
        return (uint32_t)rd(r) == 0xE06D7363u && rd(r + 0x10) == 3 &&
               ((uint32_t)rd(r + 0x14) == 0x19930520u || (uint32_t)rd(r + 0x14) == 0x19930521u);
    };
    if (isCpp(rec) && rd(rec + 0x1C) == 0) {            // rethrow: swap in the saved record
        uint32_t c; uint32_t r = s->rethrow_record(nullptr, &c);
        if (!r) return;
        rec = r;
        if (!s->validate_read(rec)) s->inconsistency();
        if ((uint32_t)rd(rec) == 0xE06D7363u && isCpp(rec) && rd(rec + 0x1C) == 0) s->inconsistency();
    }
    if (isCpp(rec)) {
        uint32_t lo, hi; s->get_range(&lo, &hi);
        uint32_t tbl = (uint32_t)rd(funcinfo + 0x10);   // pTryBlockMap
        for (uint32_t i = lo; i < hi; i++) {
            uint32_t e = tbl + i * 20;
            if (rd(e) > state || state > rd(e + 4)) continue;
            for (int32_t n = rd(e + 12); n > 0; n--) {
                uint32_t arr = (uint32_t)rd((uint32_t)rd(rec + 0x1C) + 0xC);
                for (int32_t k = rd(arr); k > 0; k--)
                    if (s->type_match(rd(e + 16), rd(arr + 4 * (rd(arr) - k + 1)), (uint32_t)rd(rec + 0x1C))) {
                        s->catch_it(rd(e + 16)); goto next;
                    }
            }
        next:;
        }
        if (is_rethrow_ok) s->destruct_exception_object(rec);
        return;
    }
    if (!is_rethrow_ok) s->foreign(); else s->terminate();
}
