// MSVC 2003 CRT _freefls body shared by D2Launch+0x4785 and D2Lang+0x1F12 (identical code, different
// static-data addresses). Guest 32-bit offsets become named host pointers; env supplies the CRT seams.
#pragma once
#include <cstdint>
#include <initializer_list>
struct CrtMbcInfo { int32_t refcount; };                       // +0xb4 of the object at locinfo[0x13]
struct CrtLocInfo {                                            // ptloci (tiddata+0x64)
    int32_t refcount;                                          // +0x00
    int32_t* r2c; int32_t* r30; int32_t* r34; int32_t* r40;    // +0x2c +0x30 +0x34 +0x40 refcount pointers
    CrtMbcInfo* mbc;                                           // +0x4c
};
struct CrtTidData {
    void *p24, *p2c, *p34, *p3c, *p44, *p48, *p54;
    int32_t* ptmbcinfo;                                        // +0x60
    CrtLocInfo* ptlocinfo;                                     // +0x64
};
struct CrtFreeflsEnv {
    void (*free)(void*);
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    void (*freetlocinfo)(CrtLocInfo*);
    void* defaultP54;               // static default at tiddata+0x54 (never freed)
    int32_t* defaultMbc;            // static mbcinfo whose last holder is not freed
    CrtLocInfo* defaultLoc[2];      // the two static locinfo objects never passed to freetlocinfo
};
static inline void crt_freefls_body(CrtTidData* t, const CrtFreeflsEnv& e) {
    if (!t) return;
    for (void* p : {t->p24, t->p2c, t->p34, t->p3c, t->p44, t->p48}) if (p) e.free(p);
    if (t->p54 != e.defaultP54) e.free(t->p54);
    e.lock(0xd);
    int32_t* m = t->ptmbcinfo;
    if (m && --*m == 0 && m != e.defaultMbc) e.free(m);
    e.unlock(0xd);
    e.lock(0xc);
    CrtLocInfo* l = t->ptlocinfo;
    if (l) {
        --l->refcount;
        if (l->r2c) --*l->r2c;
        if (l->r34) --*l->r34;
        if (l->r30) --*l->r30;
        if (l->r40) --*l->r40;
        --l->mbc->refcount;
        if (l != e.defaultLoc[0] && l != e.defaultLoc[1] && l->refcount == 0) e.freetlocinfo(l);
    }
    e.unlock(0xc);
    e.free(t);
}
