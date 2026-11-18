// 1.13c D2Client+0x81B40: EBX, ESI, EDI are register inputs. EBX == 0 returns 0. Otherwise D2Common Ordinal 11139
// (thunk 0xC262, stack (edi, esi), callee pops) and, when non-zero, Ordinal 10608 (thunk 0xCAA8, (edi, esi));
// the result is Ordinal 11041 (thunk 0xCA66, (edi, ebx, esi)) != 0. Thunks injected.
#include <cstdint>

struct D2c0x81b40Env {
    uint32_t (*thunk_c262)(uint32_t edi, uint32_t esi);
    uint32_t (*thunk_caa8)(uint32_t edi, uint32_t esi);
    uint32_t (*thunk_ca66)(uint32_t edi, uint32_t ebx, uint32_t esi);
};

extern "C" uint32_t d2c_port_81b40(uint32_t ebx, uint32_t esi, uint32_t edi, const D2c0x81b40Env& e) {
    if (ebx == 0) return 0;
    if (e.thunk_c262(edi, esi) != 0) e.thunk_caa8(edi, esi);
    return e.thunk_ca66(edi, ebx, esi) != 0 ? 1u : 0u;
}
