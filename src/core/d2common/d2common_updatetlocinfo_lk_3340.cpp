#include <cstdint>

// CRT ___updatetlocinfo_lk: switch the calling thread's locale-info block to
// the global one (PTR_DAT_6fde91c4), moving reference counts across. Block
// layout (32-bit words): [0] own refcount; [0xb],[0xc],[0xd],[0x10] pointers
// to sub-counters; [0x13] pointer to a record whose word at +0xb4 is a count.
// Env seam: thread slot, global block, static default block and the free
// routine (___freetlocinfo, +0x3270) are passed in.
struct TLocInfo {
    int32_t refs;
    int32_t* sub[0x13 - 1]; // only indices 0xb,0xc,0xd,0x10 are used
    struct { uint8_t pad[0xb4]; int32_t count; }* rec;
};

static inline void dec(int32_t* p) { if (p) --*p; }
static inline void inc(int32_t* p) { if (p) ++*p; }

// 1.13c D2Common+0x03340
TLocInfo* d2common_updatetlocinfo_lk_3340(TLocInfo** threadSlot, TLocInfo* global,
                                          TLocInfo* staticDefault, void (*freeTLoc)(TLocInfo*))
{
    TLocInfo* old = *threadSlot;
    if (old != global) {
        if (old) {
            --old->refs;
            dec(old->sub[0xb - 1]); dec(old->sub[0xd - 1]);
            dec(old->sub[0xc - 1]); dec(old->sub[0x10 - 1]);
            --old->rec->count;
        }
        *threadSlot = global;
        ++global->refs;
        inc(global->sub[0xb - 1]); inc(global->sub[0xd - 1]);
        inc(global->sub[0xc - 1]); inc(global->sub[0x10 - 1]);
        ++global->rec->count;
        if (old && old->refs == 0 && old != staticDefault) freeTLoc(old);
    }
    return *threadSlot;
}
