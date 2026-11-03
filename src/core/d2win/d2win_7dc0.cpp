// 1.13c D2Win+0x7DC0 -- sub_7DC0.
// Thiscall shape: ECX = buf (260+ bytes), EAX = mode flag (0 selects mode 2, else mode 3), EBX = register input,
// stack word 0 = file name, ret 4. The #266 out-pointer is the name's own stack slot (retail reads it back after the call).
// Every import and thunk is a seam the caller supplies. The fatal path does not exist here.
#include <cstdint>

extern "C" uint32_t d2win_7DC0(uint32_t buf, uint32_t flag, uint32_t ebx, uint32_t name,
                               void (*ord501)(uint32_t dst, uint32_t src, uint32_t max),
                               uint32_t (*probe266)(uint32_t buf, uint32_t ebx, uint32_t mode, uint32_t* out),
                               uint32_t (*moduleName)(uint32_t buf, uint32_t size),
                               void (*ord503)(uint32_t dst, uint32_t src, uint32_t max),
                               uint32_t (*fallback7B30)(uint32_t buf, uint32_t name, uint32_t ebx, uint32_t* out)) {
    uint32_t mode = (flag == 0) ? 2u : 3u;
    uint32_t slot = name;
    ord501(buf, name, 0x7fffffffu);
    if (probe266(buf, ebx, mode, &slot) != 0) return slot;
    if (moduleName(buf, 0x104u) == 0) *(uint8_t*)(uintptr_t)buf = 0;
    // strrchr(buf, '\\'): truncate after the last backslash.
    uint8_t* p = (uint8_t*)(uintptr_t)buf;
    uint8_t* last = nullptr;
    for (uint8_t* q = p; *q != 0; ++q) if (*q == 0x5c) last = q;
    if (last != nullptr) last[1] = 0;
    ord503(buf, name, 0x7fffffffu);
    if (probe266(buf, ebx, mode, &slot) != 0) return slot;
    uint32_t hit = fallback7B30(buf, name, ebx, &slot);
    return slot & ((hit != 0) ? 0xffffffffu : 0u);
}
