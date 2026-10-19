// src/core/d2client/d2client_batch_tiny_1018m.cpp -- five UNMAPPED 1.13c D2Client.dll rows
// reimplemented from build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, batch 1018m).
// Base 0x6FAB0000; every offset below is retail's. Unrecovered callees are host seams (the
// src/core/d2client/d2client_batch_2c10.cpp pattern), placed AFTER the retail arguments, which keep
// their order. No spec, no vector yet.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

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

// 1.13c D2Client+0xAD800 (30 B): fastcall ECX = record. Look up the unit id at record+1, store the
// zero-extended byte at record+5 into the node's dword at +0x30, then tail-call the list refresh.
// A null lookup returns without either write.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_ad800(const void* self, const D2ClientBatchTiny1018mEnv* env) {
    const uint8_t* p = (const uint8_t*)self;
    int32_t id;
    std::memcpy(&id, p + 1, 4);
    uint8_t* node = (uint8_t*)env->unit_lookup(self, id);
    if (node == nullptr) return;
    const uint32_t v = p[5];
    std::memcpy(node + 0x30, &v, 4);
    env->unit_refresh();
}

// 1.13c D2Client+0xAD8E0 (8 B): a tail thunk, not the body Ghidra printed for it (it followed the jmp):
//   mov ecx,[ecx+1]   ; the unit id at record+1
//   jmp 0x6FB4E280    ; 0x9E280, the unit teardown, with ECX = that id
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_ad8e0(const void* self, const D2ClientBatchTiny1018mEnv* env) {
    const uint8_t* p = (const uint8_t*)self;
    int32_t id;
    std::memcpy(&id, p + 1, 4);
    env->unit_free(id);
}

// 1.13c D2Client+0xADB30 (29 B): thiscall ECX = record. Copy the 6-byte prefix {dword at +0, word at
// +4} into an 8-byte stack frame and hand its address to 0x47970 (in EAX).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_adb30(const void* self, const D2ClientBatchTiny1018mEnv* env) {
    const uint8_t* p = (const uint8_t*)self;
    uint8_t msg[8] = {0};
    std::memcpy(msg, p, 4);
    std::memcpy(msg + 4, p + 4, 2);
    env->msg_47970(msg);
}

// 1.13c D2Client+0xADB70 (29 B): same frame and call shape as 0xADB30, with 0x82F50 as the target.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_adb70(const void* self, const D2ClientBatchTiny1018mEnv* env) {
    const uint8_t* p = (const uint8_t*)self;
    uint8_t msg[8] = {0};
    std::memcpy(msg, p, 4);
    std::memcpy(msg + 4, p + 4, 2);
    env->msg_82F50(msg);
}

// 1.13c D2Client+0xAF8F0 (31 B): same {dword, word} frame, then 0x1E160(self, &frame) -- ECX passes
// through unchanged, the frame pointer is the one stack argument.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_af8f0(const void* self, const D2ClientBatchTiny1018mEnv* env) {
    const uint8_t* p = (const uint8_t*)self;
    uint8_t msg[8] = {0};
    std::memcpy(msg, p, 4);
    std::memcpy(msg + 4, p + 4, 2);
    env->input_1e160(self, msg);
}
