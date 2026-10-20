#pragma once
#include <cstddef>
#include <cstdint>
#include "vsprintf_2814.h"

// Argument source standing in for retail's 32-bit stack argument block (wasm64 has no such
// block). Each call consumes what retail advances over: one slot (u32/ptr) or two (u64).
struct D2CommonOutputArgs3985 {
    void* ctx;
    uint32_t (*u32)(void*);
    uint64_t (*u64)(void*);   // I64 integers and doubles
    const void* (*ptr)(void*); // %s %S %n %Z pointers
};
// %Z operand (ANSI_STRING-like): length in bytes, capacity, buffer.
struct D2CommonCountedString3985 { int16_t length; int16_t maximum; const void* buffer; };
struct D2CommonOutputCalls3985 {
    int (*flushByte)(int, D2CommonStringStream2814*);        // +0x37DE
    int (*wctomb)(char*, char16_t);                           // +0x7442
    int (*isLeadByte)(int);                                   // ctype table, PTR_DAT_6fde9910
    void* (*alloc)(size_t);                                   // +0x5F7F
    void (*release)(void*);                                   // +0x4C0D
    void (*cfltcvt)(const uint64_t*, char*, int, int, int);   // PTR___fptrap_6fde9a68
    void (*forcdecpt)(char*);                                 // PTR___fptrap_6fde9a74
    void (*cropzeros)(char*);                                 // PTR___fptrap_6fde9a6c
};
extern "C" int D2Common_3985_Output(D2CommonStringStream2814*, const char*,
                                    const D2CommonOutputArgs3985*, const D2CommonOutputCalls3985*);
