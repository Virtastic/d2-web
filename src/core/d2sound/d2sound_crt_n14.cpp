// D2sound CRT batch n14. External state goes through n14_Env seams.
#include <cstdint>
#include <cstring>

struct n14_Env {
    void  (*lock)(int);                      // __lock (D2sound+0x1510)
    void  (*unlock)(int);                    // matching unlock thunk
    void* (*malloc_)(uint32_t);              // _malloc
    int   (*initCsSpin)(void*, uint32_t);    // ___crtInitCritSecAndSpinCount
    int   (*xcptFilter)(uint32_t, void*);    // __XcptFilter
    void  (*freefls)(void*);                 // __freefls@4
    void* (*flsGetValue)(uint32_t);          // DAT_6f9c61b4
    void  (*flsSetValue)(uint32_t, void*);   // DAT_6f9c61b8
    uint32_t flsIndex;                       // DAT_6f9c5158
    int   osPlatform;                        // DAT_6f9c6004
    uint32_t osMajor;                        // DAT_6f9c6010
    uint32_t* lockTab;                       // 0x24 {ptr, needed} pairs @6f9c5038
    uint8_t*  lockPool;                      // DAT_6f9c6060, 0x18 per CS
    uint32_t* onexitBegin;                   // DAT_6f9c698c
    uint32_t* onexitEnd;                     // DAT_6f9c6990
    void* (*onexitLk)(void*);                // __onexit_lk
    void* (*updatetlocinfoLk)();             // ___updatetlocinfo_lk
};
static n14_Env* g_n14;

// 1.13c D2sound+0xD0A4
double n14_set_exp(double x, int16_t e) {
    uint64_t b; memcpy(&b, &x, 8);
    uint16_t hi = (uint16_t)((((e + 0x3fe) * 0x10) | ((b >> 48) & 0x800f)) & 0xffff);
    b = (b & 0xffffffffffffULL) | ((uint64_t)hi << 48);
    memcpy(&x, &b, 8);
    return x;
}

// 1.13c D2sound+0x1676
uint32_t n14_onexitinit() {
    uint32_t* p = (uint32_t*)g_n14->malloc_(0x80);
    g_n14->onexitEnd = p;
    if (!p) return 0x18;
    *p = 0;
    g_n14->onexitBegin = p;
    return 0;
}

// 1.13c D2sound+0x169E
void* n14_onexit(void* func) {
    g_n14->lock(8);
    void* r = g_n14->onexitLk(func);
    g_n14->unlock(8);
    return r;
}

// 1.13c D2sound+0x1AB6
void n14_freeptd(void* ptd) {
    if (g_n14->flsIndex != 0xffffffffu) {
        if (!ptd) ptd = g_n14->flsGetValue(g_n14->flsIndex);
        g_n14->freefls(ptd);
        g_n14->flsSetValue(g_n14->flsIndex, 0);
    }
}

// 1.13c D2sound+0x235D
uint32_t n14_heap_select() {
    return (g_n14->osPlatform == 2 && 4 < g_n14->osMajor) ? 1 : 3;
}

// 1.13c D2sound+0x25F1
int n14_sub_25F1(int code, void* ep) {
    return code == -0x1f928c9d ? g_n14->xcptFilter(0xe06d7363, ep) : 0;
}

// 1.13c D2sound+0x2B72
void n14_sub_2B72() { g_n14->unlock(4); }

// 1.13c D2sound+0x2C29
void n14_sub_2C29() { g_n14->unlock(4); }

// 1.13c D2sound+0x301E
void n14_sub_301E() { g_n14->unlock(0xc); }

// 1.13c D2sound+0x34F5
void n14_sub_34F5() { g_n14->unlock(0xd); }

// 1.13c D2sound+0x1472
int n14_mtinitlocks() {
    n14_Env* e = g_n14;
    uint8_t* pool = e->lockPool;
    for (int i = 0; i < 0x24; i++) {
        if (e->lockTab[i * 2 + 1] == 1) {
            void* cs = pool; pool += 0x18;
            e->lockTab[i * 2] = (uint32_t)(uintptr_t)cs;
            if (!e->initCsSpin(cs, 4000)) { e->lockTab[i * 2] = 0; return 0; }
        }
    }
    return 1;
}

// 1.13c D2sound+0x2FEC
void* n14_updatetlocinfo() {
    g_n14->lock(0xc);
    void* r = g_n14->updatetlocinfoLk();
    n14_sub_301E();
    return r;
}
