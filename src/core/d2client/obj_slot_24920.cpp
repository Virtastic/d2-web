// Derived from cached 1.13c decompile; D2Client has no D2MOO prior art.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct ObjSlotEnv {
    void*    (*alloc)(uint32_t half);  // D2Client+0xA8C20 (arg = count>>1)
    void     (*release)(void* node);   // D2Client+0xA8630 (node in ESI)
    void     (*init)(void* node);      // D2Client+0xA8BE0
};
// 1.13c D2Client+0x24920
// ecx=obj, ax=count: obj+0x64 holds an owned node; count==0 frees it, else allocate if absent and init it.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_24920(void* obj, uint16_t count, const ObjSlotEnv* env) {
    uint8_t* slot = static_cast<uint8_t*>(obj) + 0x64;
    void* node; std::memcpy(&node, slot, sizeof node);
    if (count == 0) {
        if (node) { env->release(node); node = nullptr; std::memcpy(slot, &node, sizeof node); }
        return;
    }
    if (!node) {
        node = env->alloc(count >> 1);
        std::memcpy(slot, &node, sizeof node);
        if (!node) return;
    }
    env->init(node);
}
