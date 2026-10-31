// Implementation batch 452c0: skill-level and stat-scaling helpers in the 0x49F30-0x4E230 range,
// written from the cached 1.13c decompiles (build-wasm/ghidra-d2game/decomp). Callees are declared by
// retail ordinal (oNNNNN) or RVA (rNNNN); registers the decompile shows as in_* / unaff_* are explicit
// parameters. Pointer-sized values use void* / intptr_t so the 0x1C field read is wasm64-correct.
// Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>

using i32 = int32_t;
using u32 = uint32_t;

// private signatures for the shared env imports: other TUs declare them with 64-bit words
namespace impl452c0_env {
__attribute__((weak, import_module("env"), import_name("o10265"))) u32 o10265(i32);                 // Ordinal_10265: string id lookup for the fatal path
__attribute__((weak, import_module("env"), import_name("o10024"))) void o10024(const char*, u32);   // Ordinal_10024: module fatal, tail is _exit(-1)
__attribute__((weak, import_module("env"), import_name("o10457"))) void o10457(i32);                // Ordinal_10457
__attribute__((weak, import_module("env"), import_name("o10237"))) void* o10237(i32);               // Ordinal_10237: returns a pointer whose +0x1C is a divisor
__attribute__((weak, import_module("env"), import_name("o11104"))) i32 o11104();                    // Ordinal_11104: zero means the gating state holds
__attribute__((weak, import_module("env"), import_name("o10587"))) i32 o10587();                    // Ordinal_10587: level-like value compared against 99
} using namespace impl452c0_env;
extern const char DAT_6fd19ff9[]; // retail module-name literal used by the fatal path

// 1.13c D2Game+0x49F30 (ecx = unit, edx = idx, eax = out: 4 dwords)
void D2GAME_49F30(i32* out, i32 unit, u32 idx)
{
    if (idx > 0xf && unit != 0) {
        out[0] = -1;
        out[1] = -1;
        out[2] = -1;
        out[3] = -1;
        o10457(unit);
        return;
    }
    o10024(DAT_6fd19ff9, o10265(0x3be));
    std::_Exit(-1);
}

// 1.13c D2Game+0x4DF60 (al = class byte, esi = value to divide)
i32 D2GAME_4DF60(i32 al, i32 value)
{
    void* p = o10237(al);
    if (p != nullptr && *(i32*)((char*)p + 0x1c) != 0)
        return value / *(i32*)((char*)p + 0x1c);
    return value;
}

// 1.13c D2Game+0x4E230 (edi = skill id, ebx = value, esi = optional flag pointer)
i32 D2GAME_4E230(i32 skillId, i32 value, const i32* flag)
{
    switch (skillId) {
    case 0x24:
    case 0x25:
    case 0x27:
    case 0x29:
    case 0x2b:
    case 0x2d:
        if (value < 1 && (flag == nullptr || *flag != 0) && o11104() == 0 && o10587() > 99)
            return value / 5;
        break;
    default:
        break;
    }
    return value;
}
