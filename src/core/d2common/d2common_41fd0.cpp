// 1.13c D2Common+0x41FD0 (#10063): stdcall(unit, speed), ret 8. An 8-byte path buffer and a 4-byte class code start zero. 0x44290(unit, &path, &code, 0, 1,
// *(unit + 0x60), -1) (seven stack words) fills them, then 0x41DD0 (EBX = &path) returns the AnimData record for that path or 0; a record gets its dword at
// +0xC set to speed. 1.10f stopped after the write; 1.13c returns. Both callees are seams.
#include <cstdint>
#include <cstring>

struct D2Common41FD0Env {
    void (*path)(uint32_t unit, uint8_t* path, uint32_t* code, uint32_t inventory);   // 0x44290
    uint32_t (*lookup)(uint8_t* path);                                                // 0x41DD0
};

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_41FD0_Raw(uint32_t unit, uint32_t speed, const D2Common41FD0Env* env)
{
    uint32_t code = 0;
    uint8_t path[8] = {};
    env->path(unit, path, &code, Rd(unit + 0x60));
    const uint32_t rec = env->lookup(path);
    if (rec != 0) memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(rec + 0xC)), &speed, 4);
}
