// 1.13c D2Common+0x3D250: register EDI = text list {+0 pool, +4 word count, +8 first node}. Each node's next link (+8) is read before the
// node is released through the Fog free thunk 0x9228 (ECX = *(list), EDX = node, stack: file string, line 0x3C, 0; ret 0xC), a seam.
// The head link (+8) is not cleared; only the word at +4 is zeroed. The file string is logged by the probe as the image word 0x94060.
#include <cstdint>
#include <cstring>

struct D2Common3D250Env { void (*freeNode)(uint32_t pool, uint32_t node); };

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_3D250_Raw(uint32_t list, const D2Common3D250Env* env)
{
    uint32_t node = Rd(list + 8);
    while (node != 0) {
        const uint32_t next = Rd(node + 8);
        env->freeNode(Rd(list), node);
        node = next;
    }
    const uint16_t z = 0; memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(list + 4)), &z, 2);
}
