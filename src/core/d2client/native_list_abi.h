#pragma once
#include <cstdint>

struct D2ClientBatch1e70Env {
    uint32_t (*ord10044)();                                                     // imported ordinal 10044
    void (*insert)(void* rowBase, uint32_t slot, uint32_t value, uint32_t owner); // 0xA6EF0, __fastcall + 4 stack
    void* (*find)(uint32_t key, void* rowBase, uint32_t slot);                   // 0xA4E20; EAX = slot
    void (*removeAll)(void* obj);                                                // 0xA8630; ESI = obj
    void (*dispatch)(int32_t* obj, uint32_t arg, uint32_t kind);                 // 0x212F0
    void* (*fsopen)(const char* name, const char* mode, int shflag);             // 0x3695 __fsopen
};

// Native pointers; serialized x86 node words must be decoded before calling.
extern "C" void d2_client_10b0(uintptr_t* anchor, uintptr_t* base, uintptr_t* node, int32_t mode);
extern "C" void d2_client_a5b00(uint32_t row, uint32_t value, const D2ClientBatch1e70Env* env);
