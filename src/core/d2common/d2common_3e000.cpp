// 1.13c D2Common+0x3E000: register EAX = level (+0x14 = outdoor block ou). Runs 0x3D5A0(level) then 0x3DD40(level), then for i from 0 while i is below the
// signed count at ou+0x260 (reread every pass): 0x17EB0(level), and when it returns non-zero 0x36FE0 (EAX = the head word at ou+0x68+4*i, EDI = ou+0x2C,
// EBX = 0x80) followed by 0x3D920(level, i) (ESI = level). Every callee is a seam.
#include <cstdint>
#include <cstring>

struct D2Common3E000Env {
    void (*collect)(uint32_t level);                       // 0x3D5A0
    void (*route)(uint32_t level);                         // 0x3DD40
    uint32_t (*search)(uint32_t level);                    // 0x17EB0
    void (*trim)(uint32_t head, uint32_t edi, uint32_t ebx);   // 0x36FE0
    void (*mark)(uint32_t level, uint32_t index);          // 0x3D920
};

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_3E000_Raw(uint32_t level, const D2Common3E000Env* env)
{
    const uint32_t ou = Rd(level + 0x14);
    env->collect(level);
    env->route(level);
    for (int32_t i = 0; i < static_cast<int32_t>(Rd(ou + 0x260)); i++) {
        if (env->search(level) != 0) {
            env->trim(Rd(ou + 0x68 + 4u * static_cast<uint32_t>(i)), ou + 0x2C, 0x80);
            env->mark(level, static_cast<uint32_t>(i));
        }
    }
}
