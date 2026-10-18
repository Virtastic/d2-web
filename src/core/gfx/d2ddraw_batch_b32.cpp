// src/core/gfx/d2ddraw_batch_b32.cpp -- five UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's.
//
// Four are MSVC 2003 (VC7.1) Release CRT glue the linker pulled into the DLL:
// the three nine-byte __unlock fragments (0x1EE7, 0x1FCB, 0x3E7E, each `call __unlock; ret`) and the
// errno getter (0x3CB2, `call __getptd; add eax,8; ret`). The fifth, 0x43E0, is the CRT's
// word-at-a-time strcpy. The lock table, the __unlock implementation (RVA 0x1A17) and __getptd (RVA
// 0x1BBE) live in the earlier batch files and are reached through their shared accessors.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

extern "C" void d2ddraw_sub_1A17(int locknum);   // RVA 0x1A17, d2ddraw_batch2_rowex.cpp
extern "C" void* d2_fn_d2ddraw_1bbe(void);       // RVA 0x1BBE (__getptd), d2ddraw_batch_10da.cpp

// 1.13c D2DDraw+0x01EE7: tail fragment into RVA 0x1A17 with lock number 4 -- __unlock(4).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_1EE7(void) { d2ddraw_sub_1A17(4); }

// 1.13c D2DDraw+0x01FCB: tail fragment into RVA 0x1A17 with lock number 4 -- __unlock(4). A second
// copy of the same leaf, emitted once per instantiation site.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_1FCB(void) { d2ddraw_sub_1A17(4); }

// 1.13c D2DDraw+0x03E7E: tail fragment into RVA 0x1A17 with lock number 0xC -- __unlock(0xC).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_3E7E(void) { d2ddraw_sub_1A17(0xC); }

// 1.13c D2DDraw+0x03CB2 (_errno): __getptd(), then the address of its _terrno cell at +8. Returns a
// pointer into the per-thread block; writes nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2ddraw_sub_3CB2(void) {
    uint8_t* ptd = (uint8_t*)d2_fn_d2ddraw_1bbe();
    return (int32_t*)(ptd + 0x8);
}

// 1.13c D2DDraw+0x043E0 (_strcpy): the VC7.1 word-at-a-time copy. The tail is aligned byte by byte
// until the source is 4-byte aligned; a byte-wise copy stops on NUL. Then full words are tested with
// the 0x7EFEFEFF/0x81010100 zero-byte trick: if a word holds no zero byte it is stored whole; if it
// does, the terminator's position inside the word selects a 1-, 2-, 3- or 4-byte store. Returns the
// destination base.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t* d2ddraw_sub_43E0(uint32_t* param_1, uint32_t* param_2) {
    uint8_t  bVar1;
    uint32_t uVar2;
    uint32_t uVar3;
    uint32_t* puVar4 = param_1;

    while (((uintptr_t)param_2 & 3) != 0) {
        bVar1 = *(uint8_t*)param_2;
        uVar3 = (uint32_t)bVar1;
        param_2 = (uint32_t*)((uintptr_t)param_2 + 1);
        if (bVar1 == 0) goto LAB_6f8c44d0;
        *(uint8_t*)puVar4 = bVar1;
        puVar4 = (uint32_t*)((uintptr_t)puVar4 + 1);
    }
    do {
        uVar2 = *param_2;
        uVar3 = *param_2;
        param_2 = param_2 + 1;
        if (((uVar2 ^ 0xFFFFFFFFu ^ (uVar2 + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
            if ((uint8_t)uVar3 == 0) {
LAB_6f8c44d0:
                *(uint8_t*)puVar4 = (uint8_t)uVar3;
                return param_1;
            }
            if ((uint8_t)(uVar3 >> 8) == 0) {
                *(uint16_t*)puVar4 = (uint16_t)uVar3;
                return param_1;
            }
            if ((uVar3 & 0x00FF0000u) == 0) {
                *(uint16_t*)puVar4 = (uint16_t)uVar3;
                *(uint8_t*)((uintptr_t)puVar4 + 2) = 0;
                return param_1;
            }
            if ((uVar3 & 0xFF000000u) == 0) {
                *puVar4 = uVar3;
                return param_1;
            }
        }
        *puVar4 = uVar3;
        puVar4 = puVar4 + 1;
    } while (true);
}
