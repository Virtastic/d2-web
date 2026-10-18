// src/core/realm/bnclient_batch_b54.cpp -- twelve UNMAPPED 1.13c BNClient.dll rows reimplemented from
// the cached decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane: implementation +
// marker only, no spec, no vector). Base 0x6FF20000; every global is named at its retail RVA.
//
// Rows: sub_1412 0x1412, sub_141B 0x141B, sub_23DD 0x23DD, sub_2494 0x2494, sub_30AE 0x30AE,
// sub_315A 0x315A, __initptd 0x3242, sub_33F8 0x33F8, sub_3404 0x3404, ___ascii_stricmp 0x38D0,
// sub_3D47 0x3D47, _strcmp 0x3D70. All are MSVC 2003 (VC7.1) Release CRT leaves.
//
// The nine-byte sub_XXXX leaves are the lock wrappers: each enters or leaves one slot of the CRT lock
// table DAT_6FF3D090. The table and its __lock/__unlock engines already live in
// bnclient_batch_2558.cpp (0x3163 __lock, bn_unlock_lock = the shared slot release); these wrappers
// pass the constant slot number the decompile shows.
// Prior art: src/core/realm/bnclient_batch_2558.cpp (__lock and the slot release), the D2Game lock
// wrappers src/core/d2game/d2game_crt_b23.cpp 0x3D36/0x3D42, __initptd 0x3B80 there and
// src/core/gfx/d2ddraw_batch_10da.cpp 0x1BAB, and src/core/d2client/crt_strings.cpp for the strings.
#include <cstdint>
#include <emscripten.h>

extern "C" void bn_lock(int32_t lock_num, const void* env);   // 0x3163 __lock
extern "C" void bn_unlock_lock(int32_t lock_num);             // __unlock on DAT_6FF3D090

// 1.13c BNClient+0x01412 (9 B, caller 0x1342 __onexit): the __lock(8) wrapper -- enter the CRT lock
// slot 8 through 0x3163, which creates the slot on first use.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_lock_1412(void) {
    bn_lock(8, nullptr);
}

// 1.13c BNClient+0x0141B (9 B, caller 0x1374): __unlock(8) -- leave the CRT lock slot 8.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_141b(void) {
    bn_unlock_lock(8);
}

// 1.13c BNClient+0x023DD (9 B, caller 0x2275): __unlock(4) -- leave the CRT lock slot 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_23dd(void) {
    bn_unlock_lock(4);
}

// 1.13c BNClient+0x02494 (9 B, caller 0x2427 __msize): __unlock(4) -- leave the CRT lock slot 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_2494(void) {
    bn_unlock_lock(4);
}

// 1.13c BNClient+0x030AE (21 B, 15 callers): __unlock(lock_num) -- the shared release of the CRT lock
// table slot DAT_6FF3D090[lock_num*2]. The table, its initialisation and the LeaveCriticalSection are
// bn_unlock_lock in bnclient_batch_2558.cpp; a slot that was never created holds 0 and is skipped.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_30ae(int32_t lock_num) {
    bn_unlock_lock(lock_num);
}

// 1.13c BNClient+0x0315A (9 B, caller 0x30C3 __mtinitlocknum): __unlock(0xA) -- leave slot 10.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_315a(void) {
    bn_unlock_lock(0xA);
}

// 1.13c BNClient+0x03242 (19 B, callers 0x1F1F __beginthreadex and 0x1FDD __CRT_INIT@12): __initptd.
// Seed a _ptiddata block: _initaddr at +0x54 = DAT_6FF3D4D8 (stored as its RVA 0x1D4D8, the port's
// convention for retail VAs) and _holdrand at +0x14 = 1. The locale argument is not touched.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_initptd(uint8_t* ptd, void* locale) {
    (void)locale;
    *(uint32_t*)(ptd + 0x54) = 0x1D4D8u;   // _initaddr, retail 0x6FF3D4D8
    *(uint32_t*)(ptd + 0x14) = 1;          // _holdrand
}

// 1.13c BNClient+0x033F8 (9 B, caller 0x32C6 __freefls@4): __unlock(0xD) -- leave slot 13.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_33f8(void) {
    bn_unlock_lock(0xD);
}

// 1.13c BNClient+0x03404 (9 B, caller 0x32C6 __freefls@4): __unlock(0xC) -- leave slot 12.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_3404(void) {
    bn_unlock_lock(0xC);
}

// A-Z to lower case; every other byte is unchanged.
static uint8_t bn_fold_ascii(uint8_t c) { return (uint8_t)(c - 'A') < 26 ? (uint8_t)(c + 0x20) : c; }

// 1.13c BNClient+0x038D0 (78 B, caller 0x18C6 __stricmp): ___ascii_stricmp. Compare byte-wise, folding
// only A-Z to lower case; return a signed char -1/0/1 (retail tests the terminator on the second
// string). Same rule as D2Client+0x76F0 in src/core/d2client/crt_strings.cpp.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_ascii_stricmp_38d0(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        uint8_t x = *a, y = *b;
        if (x != y) {
            x = bn_fold_ascii(x);
            y = bn_fold_ascii(y);
            if (x != y) return x < y ? -1 : 1;
        }
        if (y == 0) return 0;
    }
}

// 1.13c BNClient+0x03D47 (9 B, caller 0x3CF4): __unlock(4) -- leave the CRT lock slot 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_3d47(void) {
    bn_unlock_lock(4);
}

// 1.13c BNClient+0x03D70 (135 B, caller 0x1799A TypeMatch): _strcmp. Compare unsigned bytes and return
// -1/0/1 at the first difference or the shared terminator; the decompile's 4-byte word scan is only an
// unrolled form of this loop. Same rule as D2Client+0x4A60 in src/core/d2client/crt_strings.cpp.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_strcmp_3d70(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        if (*a != *b) return *a < *b ? -1 : 1;
        if (*a == 0) return 0;
    }
}
