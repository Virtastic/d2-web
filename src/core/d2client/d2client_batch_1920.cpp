// src/core/d2client/d2client_batch_1920.cpp -- UNMAPPED 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is
// retail's. Unrecovered callees are host seams (the src/core/d2client/cursor_init_166c0.cpp pattern):
// the function takes a D2ClientBatch1920Env AFTER its retail arguments, which keep their order.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// FUN_6fab1920 reads its RC4 state from EAX: S[0x100] at +0x00, i at +0x100, j at +0x101.
struct D2ClientRc4State { uint8_t s[0x100]; uint8_t i; uint8_t j; };
static_assert(sizeof(D2ClientRc4State) == 0x102, "RC4 state is 0x102 bytes");
static_assert(offsetof(D2ClientRc4State, i) == 0x100, "i at +0x100");
static_assert(offsetof(D2ClientRc4State, j) == 0x101, "j at +0x101");

// The retail callees these rows reach through the import table and the local unload routine.
struct D2ClientBatch1920Env {
    int32_t (*animOrSeqMode)(const int32_t* obj);                               // D2Common #10175
    void (*scanAnim)(const int32_t* obj, int32_t* kind, int32_t* value,
                     int32_t* frame, int32_t line, const char* file);           // D2Common #10350
    void (*smemFree)(void* p, const char* file, int32_t line, int32_t flags);   // Storm #403
    void (*unloadModules)(void* client);                                        // 0x9E630
};

static const char kEngineH[]  = "..\\Source\\D2Client/Engine.h";                       // 0x0D4EA4
static const char kWardenCpp[] = "..\\Source\\D2Client\\WARDEN\\WardenClient.cpp";     // 0x0D4938

// 1.13c D2Client+0x01920: RC4 PRGA stream XOR -- i = (i+1) & 0xFF, j = (j + S[i]) & 0xFF, swap
// S[i]/S[j], data[k] ^= S[(S[i] + S[j]) & 0xFF]. State in EAX; ret 8 (data and len on the stack).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1920(uint8_t* data, uint32_t len, D2ClientRc4State* state) {
    for (uint32_t k = 0; k < len; k++) {
        state->i = (uint8_t)(state->i + 1);
        state->j = (uint8_t)(state->j + state->s[state->i]);
        uint8_t t = state->s[state->i];
        state->s[state->i] = state->s[state->j];
        state->s[state->j] = t;
        data[k] ^= state->s[(uint8_t)(state->s[state->i] + state->s[state->j])];
    }
}

// 1.13c D2Client+0x01A00: SEH-guarded __stdcall wrapper -- ESI = client, then 0x9E630(client): unload
// each module in the client's list (GetModuleHandleA/FreeLibrary, count at +0x08, pointer array at
// base+0x1C) and VirtualFree the block.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1a00(void* client, const D2ClientBatch1920Env* env) {
    env->unloadModules(client);
}

// 1.13c D2Client+0x01A40: free the block in EAX -- SMemFree(EAX, WardenClient.cpp, line 0x117, 0).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1a40(void* block, const D2ClientBatch1920Env* env) {
    env->smemFree(block, kWardenCpp, 0x117, 0);
}

// 1.13c D2Client+0x01A80: ESI = obj. NULL -> 6. obj+0xC8 bit 3 clear -> obj[0]. Set -> D2Common
// #10175(obj), then #10350(obj, &kind, &value, &frame, 0xF2, Engine.h) -- the compiler spills
// #10175's return and two scratch slots, the callee fills kind/value -- and the object in EAX.
extern "C"
EMSCRIPTEN_KEEPALIVE uintptr_t d2_client_1a80(const int32_t* obj, const D2ClientBatch1920Env* env) {
    if (obj == nullptr) return 6;
    if ((obj[0xC8 / 4] & 8) == 0) return (uintptr_t)(uint32_t)obj[0];
    int32_t kind = 0, value = 0;
    int32_t frame = env->animOrSeqMode(obj);
    env->scanAnim(obj, &kind, &value, &frame, 0xF2, kEngineH);
    return (uintptr_t)obj;
}

// 1.13c D2Client+0x01AF0: same guard. obj live and bit 3 set -> the two D2Common calls (line 0x104),
// then the object in EAX; otherwise just #10175(obj)'s result (0 for NULL, D2Common's own check).
extern "C"
EMSCRIPTEN_KEEPALIVE uintptr_t d2_client_1af0(const int32_t* obj, const D2ClientBatch1920Env* env) {
    if (obj != nullptr && (obj[0xC8 / 4] & 8) != 0) {
        int32_t kind = 0, value = 0;
        int32_t frame = env->animOrSeqMode(obj);
        env->scanAnim(obj, &kind, &value, &frame, 0x104, kEngineH);
        return (uintptr_t)obj;
    }
    return (uintptr_t)(uint32_t)env->animOrSeqMode(obj);
}

// 1.13c D2Client+0x01B50: NULL -> -1; bit 3 clear -> obj[1]; set -> the two D2Common calls (line
// 0x116) and the object in EAX.
extern "C"
EMSCRIPTEN_KEEPALIVE uintptr_t d2_client_1b50(const int32_t* obj, const D2ClientBatch1920Env* env) {
    if (obj == nullptr) return (uintptr_t)(uint32_t)-1;
    if ((obj[0xC8 / 4] & 8) == 0) return (uintptr_t)(uint32_t)obj[1];
    int32_t kind = 0, value = 0;
    int32_t frame = env->animOrSeqMode(obj);
    env->scanAnim(obj, &kind, &value, &frame, 0x116, kEngineH);
    return (uintptr_t)obj;
}
