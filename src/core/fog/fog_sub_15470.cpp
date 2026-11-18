// 1.13c Fog+0x15470 -- sEnumModules(pid): try the toolhelp module snapshot (Fog+0x15240, pid in
// EAX, pushed); if that yields no list fall back to the PSAPI walk (Fog+0x14F50, pid on the stack). Then log
// one "    Base:%08lXh  Size:%7lXh  Name:%-15.15s  Path:%s" line per node (next at +0x20c) through
// Fog+0x190A0 and return the head. The wasm build has no process module list: fog_sub_15470 passes
// enumerators that yield null, so the loop never runs and the head is null. The capture probe passes
// the recorded enumerators and logger (fog_15470_run).
#include <cstdint>

#include <emscripten.h>

struct Fog15470Env {
    uint8_t* (*enumToolhelp)(uint32_t pid);                                 // Fog+0x15240
    uint8_t* (*enumPsapi)(uint32_t proc);                                   // Fog+0x14F50
    void (*logLine)(uint32_t base, uint32_t size, const char* name, const uint8_t* node);  // Fog+0x190A0, the Base/Size/Name/Path line
};

extern "C" uint8_t* fog_15470_run(uint32_t pid, uint32_t proc, const Fog15470Env* env) {
    uint8_t* head = env->enumToolhelp(pid);
    if (head == nullptr) head = env->enumPsapi(proc);
    for (const uint8_t* n = head; n != nullptr;) {
        uint32_t base, size;
        __builtin_memcpy(&base, n + 0x204, 4);
        __builtin_memcpy(&size, n + 0x208, 4);
        env->logLine(base, size, reinterpret_cast<const char*>(n + 0x104), n);
        uintptr_t next;
        uint32_t next32;
        __builtin_memcpy(&next32, n + 0x20c, 4);
        next = next32;
        n = reinterpret_cast<const uint8_t*>(next);
    }
    return head;
}

namespace {
uint8_t* enumNone(uint32_t) { return nullptr; }
void logNone(uint32_t, uint32_t, const char*, const uint8_t*) {}
}

extern "C" EMSCRIPTEN_KEEPALIVE void* fog_sub_15470(uint32_t pid) {  // 1.13c Fog+0x15470
    const Fog15470Env env{ enumNone, enumNone, logNone };
    return fog_15470_run(pid, pid, &env);
}
