// src/core/d2client/d2client_batch_tiny_1018m.h -- the Env of d2client_batch_tiny_1018m.cpp (its FN probes include it).
#pragma once
#include <cstdint>
struct D2ClientBatchTiny1018mEnv {
    void* (*unit_lookup)(const void* self, int32_t unitId); // 0x9D630: client-unit-list lookup (ECX = self)
    void  (*unit_refresh)(void);                            // 0x772E0: rebuild/refresh the client-unit list
    void  (*unit_free)(int32_t unitId);                     // 0x9E280: 0xAD8E0's tail target
    void  (*msg_47970)(const void* msg);                    // 0x47970: {u32, u16} handler, pointer in EAX
    void  (*msg_82F50)(const void* msg);                    // 0x82F50: same record shape
    // 0x1E160 is recovered in src/core/d2client/ui_input_1e160.cpp (d2_client_1e160); its globals,
    // base and callee block belong to that function's wiring, so this thunk names only its retail shape:
    // ECX = self (unused by the dispatcher), one stack word = pMsg.
    void  (*input_1e160)(const void* self, const void* msg);
};
extern "C" void d2_client_adb30(const void* self, const D2ClientBatchTiny1018mEnv* env);
extern "C" void d2_client_adb70(const void* self, const D2ClientBatchTiny1018mEnv* env);
