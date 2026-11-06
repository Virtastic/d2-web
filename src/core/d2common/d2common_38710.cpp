// 1.13c D2Common+0x38710: forwarder, retail layout (32-bit guest addresses). The four stack words (a0..a3) reach the callee at RVA 0x382F0
// as EAX = a0, ECX = a2 and stack (a1, a3), ret 8; the callee's result is returned unchanged. The decompile and the older port treat a2 as
// the room; the disassembly dereferences the EAX argument (a0), so a0 is the room-side pointer.
#include <cstdint>
struct D2Common38710Env { uint32_t (*select)(uint32_t eax, uint32_t ecx, uint32_t flags, uint32_t mode); };
extern "C" uint32_t D2Common_38710_Raw(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, const D2Common38710Env* env)
{
    return env->select(a0, a2, a1, a3);
}
