// D2Multi MSVC CRT locale/stdio (FN-path ours): 32-bit x86 record layouts, retail callees as seams.
// 1.13c D2Multi+0x2BFC: VC7.1 CRT ___freetlocinfo. Retail globals and callees are explicit seams.
#include <cstdint>

struct D2MultiFreeTlocinfoEnv {
    void (*free_)(uint32_t token);                 // D2Multi+0x3A59
    void (*free_lconv_mon)(uint32_t lconv);        // D2Multi+0x5114
    void (*free_lconv_num)(uint32_t lconv);        // D2Multi+0x50B5
    void (*free_lc_time)(uint32_t time);           // D2Multi+0x4F25
    uint32_t default_lconv;                       // DAT_6f9ebd70, RVA 0x1BD70
    uint32_t static_ctype;                        // DAT_6fa0a360, RVA 0x3A360
    uint32_t static_mon;                          // DAT_6fa0a364, RVA 0x3A364
    uint32_t static_num;                          // DAT_6fa0a368, RVA 0x3A368
    uint32_t static_lc_time;                      // DAT_6f9ebd6c, RVA 0x1BD6C
};

// The retail record and every pointer stored in it are four-byte x86 values, also in wasm64.
// 1.13c D2Multi+0x2BFC
extern "C" void d2multi_freetlocinfo_env(uint8_t* locale, const D2MultiFreeTlocinfoEnv* env) {
    const uint32_t lconv = *(uint32_t*)(locale + 0x3C);
    const uint32_t collate = *(uint32_t*)(locale + 0x2C);
    if (lconv != env->default_lconv && lconv != 0 &&
        *(uint32_t*)(uintptr_t)collate == 0) {
        const uint32_t monetary = *(uint32_t*)(locale + 0x34);
        if (monetary != 0 && *(uint32_t*)(uintptr_t)monetary == 0 &&
            monetary != env->static_mon) {
            env->free_(monetary);
            env->free_lconv_mon(lconv);
        }
        const uint32_t numeric = *(uint32_t*)(locale + 0x30);
        if (numeric != 0 && *(uint32_t*)(uintptr_t)numeric == 0 &&
            numeric != env->static_num) {
            env->free_(numeric);
            env->free_lconv_num(lconv);
        }
        env->free_(collate);
        env->free_(lconv);
    }

    const uint32_t ctype = *(uint32_t*)(locale + 0x40);
    if (ctype != env->static_ctype && ctype != 0 &&
        *(uint32_t*)(uintptr_t)ctype == 0) {
        env->free_(ctype);
        env->free_(*(uint32_t*)(locale + 0x44) - 0xFEu);
    }

    const uint32_t time = *(uint32_t*)(locale + 0x50);
    if (time != env->static_lc_time && time != 0 &&
        *(uint32_t*)(uintptr_t)(time + 0xB4) == 0) {
        env->free_lc_time(time);
        env->free_(time);
    }
    env->free_((uint32_t)(uintptr_t)locale);
}

// 0x2E01 __getptd, 0x2BFC ___freetlocinfo; the two locale blocks are host state (the port has no retail
// image): 0x190AC holds the process default locale pointer, 0x19058 is the image's static locale block.
struct D2MultiUpdatetlocinfoLkHost {
    void*    (*getptd)(void);
    void     (*freetlocinfo)(void* ptloci);
    uint32_t default_tlocinfo;   // ds:PTR_DAT_6f9e90ac (RVA 0x190AC), the process default locale block
    uint32_t static_tlocinfo;    // DAT_6f9e9058 (RVA 0x19058), static locale, never freed
};

// 1.13c D2Multi+0x2CCC ___updatetlocinfo_lk (193 bytes): int __cdecl ___updatetlocinfo_lk(void).
// ptd = __getptd(); old = ptd->ptlocinfo (+0x64). When old is not the process default, release it: its
// own refcount and the refcounts of its non-null sub-records at +0x2C/+0x34/+0x30/+0x40, then the
// lconv at +0x4C/+0xB4 (unconditional -- the image has no null check there). Install the default, bump
// its refcount and the same non-null sub-records, then +0x4C/+0xB4. Free the old block through
// ___freetlocinfo when it is not the static 0x19058 block and its refcount reached zero. Return the
// thread's (now default) locale value. Runs under lock 0xC, its only caller 0x535E having taken it.
// 1.13c D2Multi+0x2CCC
extern "C"
uint32_t d2multi_updatetlocinfo_lk_env(const D2MultiUpdatetlocinfoLkHost* host) {
    uint8_t* ptd = (uint8_t*)host->getptd();
    uint32_t* old = (uint32_t*)(uintptr_t)*(uint32_t*)(ptd + 0x64);
    uint32_t* def = (uint32_t*)(uintptr_t)host->default_tlocinfo;
    if (old != def) {
        if (old != nullptr) {
            uint32_t* p = (uint32_t*)(uintptr_t)old[0x0B];
            *old -= 1;
            if (p != nullptr) *p -= 1;
            p = (uint32_t*)(uintptr_t)old[0x0D];
            if (p != nullptr) *p -= 1;
            p = (uint32_t*)(uintptr_t)old[0x0C];
            if (p != nullptr) *p -= 1;
            p = (uint32_t*)(uintptr_t)old[0x10];
            if (p != nullptr) *p -= 1;
            *(uint32_t*)(uintptr_t)(old[0x13] + 0xB4) -= 1;
        }
        *(uint32_t*)(ptd + 0x64) = host->default_tlocinfo;
        *def += 1;
        if (def[0x0B] != 0) *(uint32_t*)(uintptr_t)def[0x0B] += 1;
        if (def[0x0D] != 0) *(uint32_t*)(uintptr_t)def[0x0D] += 1;
        if (def[0x0C] != 0) *(uint32_t*)(uintptr_t)def[0x0C] += 1;
        if (def[0x10] != 0) *(uint32_t*)(uintptr_t)def[0x10] += 1;
        *(uint32_t*)(uintptr_t)(def[0x13] + 0xB4) += 1;
        if (old != nullptr && *old == 0 && old != (uint32_t*)(uintptr_t)host->static_tlocinfo)
            host->freetlocinfo(old);
    }
    return *(uint32_t*)(ptd + 0x64);
}

struct D2MultiUpdatetlocinfoHost { void (*lock)(int32_t); uint32_t (*updatetlocinfo_lk)(void); void (*unlock)(int32_t); };
// 1.13c D2Multi+0x2D8D ___updatetlocinfo: lock 0xC, ___updatetlocinfo_lk (0x2CCC), unlock (0x2DBF).
// 1.13c D2Multi+0x2D8D
extern "C" uint32_t d2multi_updatetlocinfo_env(const D2MultiUpdatetlocinfoHost* h) {
    h->lock(0xC); uint32_t r = h->updatetlocinfo_lk(); h->unlock(0xC); return r;
}

struct D2MultiStream32 { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; int32_t file; uint32_t charbuf; int32_t bufsiz; uint32_t tmpfname; };
// 1.13c D2Multi+0x218D __flsbuf (same MSVC 2003 body as D2Gfx+0x1C81; __write/__getbuf are wasm-less: __write accepts the whole buffer).
// 1.13c D2Multi+0x218D
extern "C" int32_t d2multi_flsbuf_env(int32_t ch, D2MultiStream32* f) {
    uint32_t flag = f->flag;
    if ((flag & 0x82) == 0 || (flag & 0x40) != 0) { f->flag = flag | 0x20; return -1; }   // _IOWRT|_IORW, _IOSTRG
    if (flag & 0x01) {                                                                    // _IOREAD
        f->cnt = 0;
        if ((flag & 0x10) == 0) { f->flag = flag | 0x20; return -1; }                     // not _IOEOF
        f->ptr = f->base; flag &= ~1u;
    }
    f->cnt = 0; flag = (flag & ~0x10u) | 0x02u; f->flag = flag;
    int32_t rc = 0; uint32_t count;
    if ((flag & 0x108) == 0) {                                                            // unbuffered: __write(fh, &ch, 1)
        count = 1; rc = 1;
    } else {
        uint32_t base = f->base, p = f->ptr;
        f->ptr = base + 1; count = p - base; f->cnt = f->bufsiz - 1;
        if ((int32_t)count >= 1) rc = (int32_t)count;                                     // __write(fh, base, count)
        *(uint8_t*)(uintptr_t)base = (uint8_t)ch;
    }
    if ((uint32_t)rc == count) return ch & 0xFF;
    f->flag |= 0x20; return -1;
}
