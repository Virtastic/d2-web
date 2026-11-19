// 1.13c Fog+0x117C0 ordinal 10050: void __fastcall(LPCRITICAL_SECTION cs) -- validate cs through 0x11690,
// then enter the section through the KERNEL32 import. ECX is forwarded unchanged to both callees. The callees are seams.
#include <cstdint>
typedef uint32_t (*FogCsCall)(uint32_t);
// 1.13c Fog+0x117C0
uint32_t fog_sub_117c0(uint32_t cs, FogCsCall validate, FogCsCall enter) {
    validate(cs);
    return enter(cs);
}
