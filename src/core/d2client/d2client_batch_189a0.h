#pragma once
#include <cstdint>
// Retail memory contains 32-bit addresses, never native pointers. The adapter
// resolves them; call arguments containing host pointers retain uintptr_t width.
// call identifies an internal callee by RVA or an imported callee by ordinal.
// Its adapter supplies the named callee's retail register/stack convention.
struct D2C189a0Env {
    void* (*resolve)(uint32_t);
    uintptr_t (*call)(uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
    uint32_t* globalClient; // DAT_6fbcbbfc, retail address
    uint32_t* registry;     // DAT_6fbcb800, eight retail addresses
    uint32_t* listHead;     // DAT_6fbcc4d4
    uint32_t* unitBuckets;  // DAT_6fbba808, 128 retail addresses
    uint32_t* shutdownFlag; // DAT_6fbcc3b8
};
