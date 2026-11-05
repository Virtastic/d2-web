// 1.13c D2Common+0x41CA0: register EAX = level (+0x1D0 id). Id 0x6E runs the single callee 0x41070(level) and returns. Any other id runs, in order,
// 0x176F0, 0x41340, 0x41230, 0x41B50 (each with the level as its one stack word), 0x416B0 (EDI = level), 0x41120 (EBX = level, only when the id is
// 0x6F), 0x41AD0 (EAX = level), 0x41730 (EDI = level) and 0x41A20 (EAX = level). The id is read again after 0x416B0; the callees are seams.
#include <cstdint>
#include <cstring>

struct D2Common41CA0Env {
    void (*c41070)(uint32_t level);
    void (*c176f0)(uint32_t level);
    void (*c41340)(uint32_t level);
    void (*c41230)(uint32_t level);
    void (*c41b50)(uint32_t level);
    void (*c416b0)(uint32_t level);   // EDI
    void (*c41120)(uint32_t level);   // EBX
    void (*c41ad0)(uint32_t level);   // EAX
    void (*c41730)(uint32_t level);   // EDI
    void (*c41a20)(uint32_t level);   // EAX
};

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_41CA0_Raw(uint32_t level, const D2Common41CA0Env* env)
{
    if (Rd(level + 0x1D0) == 0x6E) { env->c41070(level); return; }
    env->c176f0(level);
    env->c41340(level);
    env->c41230(level);
    env->c41b50(level);
    env->c416b0(level);
    if (Rd(level + 0x1D0) == 0x6F) env->c41120(level);
    env->c41ad0(level);
    env->c41730(level);
    env->c41a20(level);
}
