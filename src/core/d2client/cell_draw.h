#pragma once
#include <cstdint>
#include <cstddef>

// Retail x86 context stores handles, never native pointers. The callback and its
// user pointer remain native-width on wasm64 and native hosts.
struct D2ClientCellContext {
    uint32_t frame;
    uint32_t reserved04[12];
    uint32_t cel;
    uint32_t reserved38[2];
    uint32_t zero40;
    uint32_t reserved44;
};
static_assert(sizeof(D2ClientCellContext) == 0x48);
static_assert(offsetof(D2ClientCellContext, cel) == 0x34);
static_assert(offsetof(D2ClientCellContext, zero40) == 0x40);
using D2ClientCellDraw = uint32_t (*)(void*, const D2ClientCellContext*, int32_t, int32_t, int32_t, int32_t, int32_t);
uint32_t d2client_sub_15FC0(const int32_t*, D2ClientCellDraw, void*);
uint32_t d2client_sub_36170(uint32_t, D2ClientCellDraw, void*);
uint32_t d2client_sub_BE4C0(const int32_t*, uint32_t, D2ClientCellDraw, void*);
