// src/core/d2win/d2win_sub_18a9c.cpp -- D2Win.dll CRT _fptrap init: fills the 6 float-conversion hook slots.
#include <cstdint>
#include <emscripten.h>

// slots = &PTR___fptrap_6f8ff6d0 (6 consecutive pointers); fn = the six addresses in order
// (__cfltcvt, LAB_8ba5, __fassign, FUN_8b69, LAB_8bf0, __cfltcvt), host-supplied.
// 1.13c D2Win+0x18A9C
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_18a9c(uintptr_t* slots, const uintptr_t* fn) {
    for (int i = 0; i < 6; i++) slots[i] = fn[i];
}
