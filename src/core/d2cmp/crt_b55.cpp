// D2CMP statically linked MSVC CRT thread-data helpers (b55-cmp-a). Win32 via seams.
#include <cstdint>

struct CrtB55Env {
    void (*unlock)(int32_t lockNo);          // FUN_6fe12dd8
    uint32_t (*tlsAlloc)(void);              // TlsAlloc
    void (*mtdeletelocks)(void);             // 0x2D83
    void (*freefls)(void* ptd);              // 0x3E8E
    int32_t (*flsFree)(uint32_t idx);        // DAT_6fe347bc
    void* (*flsGet)(uint32_t idx);           // DAT_6fe347b4
    int32_t (*flsSet)(uint32_t idx, void* v);// DAT_6fe347b8
    uint32_t flsIndex;                       // DAT_6fe32188 (-1 = unset)
};


// 1.13c D2CMP+0x3118
void crt_b55_RTC_Initialize(void) {
    // loop range is empty (start == end), nothing to call
}

// 1.13c D2CMP+0x3C6B
void crt_b55_sub_3C6B(CrtB55Env* e) { e->unlock(4); }

// 1.13c D2CMP+0x3D22
void crt_b55_sub_3D22(CrtB55Env* e) { e->unlock(4); }

// 1.13c D2CMP+0x3FC0
void crt_b55_sub_3FC0(CrtB55Env* e) { e->unlock(0xd); }

// 1.13c D2CMP+0x3FCC
void crt_b55_sub_3FCC(CrtB55Env* e) { e->unlock(0xc); }

// 1.13c D2CMP+0x3DE4
uint32_t crt_b55_sub_3DE4(CrtB55Env* e) { return e->tlsAlloc(); }

// 1.13c D2CMP+0x3DED
void crt_b55_mtterm(CrtB55Env* e) {
    if (e->flsIndex != 0xFFFFFFFFu) {
        e->flsFree(e->flsIndex);
        e->flsIndex = 0xFFFFFFFFu;
    }
    e->mtdeletelocks();
}

// 1.13c D2CMP+0x3FD5
void crt_b55_freeptd(CrtB55Env* e, void* ptd) {
    if (e->flsIndex != 0xFFFFFFFFu) {
        if (!ptd) ptd = e->flsGet(e->flsIndex);
        e->freefls(ptd);
        e->flsSet(e->flsIndex, nullptr);
    }
}
