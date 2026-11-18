// 1.13c Fog+0x172B0: append a named triple {hash, v1, v2} to the list at +8, count at +4.
// fastcall ECX = name, EDX = v1, stack = v2, list (ret 8). The name hash is the Ordinal_502 result, passed in as `hash`.
#include <cstdint>

extern "C" int32_t fog_172b0_append(uint8_t* list, uint32_t hash, uint32_t v1, uint32_t v2) {
    int32_t n = *(int32_t*)(list + 4);
    uint32_t off = (uint32_t)n * 0xcu;
    if (0x400u < off + 0x14u) return 0xe;
    *(uint32_t*)(list + off + 8) = hash;
    *(uint32_t*)(list + off + 0xc) = v1;
    *(uint32_t*)(list + off + 0x10) = v2;
    *(int32_t*)(list + 4) = n + 1;
    return 0;
}
