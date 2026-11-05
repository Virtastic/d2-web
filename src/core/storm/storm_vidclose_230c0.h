#pragma once
// Storm+0x230C0 host seam (retail words: the stream struct and its blocks are dwords at retail 32-bit addresses).
#include <cstdint>
struct StormVidCloseEnv {
    uint32_t* lastError;                              // 0x55388
    uint32_t* (*node)(uint32_t addr);
    uint32_t (*check)(uint32_t p);                    // 0x22FA0 (ECX = p): non-zero when the module is initialised
    void (*fnFree0)(uint32_t a);                      // table entry +0 of the pointer at 0x553A0
    void (*fnFree3)(uint32_t a);                      // table entry +0xC
    void (*heapFree)(uint32_t ptr, uint32_t fileRva, uint32_t line);   // 0x1B960 (ptr, file, line, 0)
    void (*closeHandle)(uint32_t h);
    void (*setLastError)(uint32_t code);
    void (*reset)(uint32_t p, uint32_t listRva);      // 0x45A0 (ESI = p, stack = list at module RVA)
};
