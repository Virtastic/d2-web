// 1.13c Storm+0x38F10 (ECX = obj, BL = key, result in EAX): the largest positive +0x14 value over the node list at obj+0x14 (a non-positive head
// is an empty list; next at +4, the walk stops at the first non-positive link) whose byte +8 equals key with +0xA clear and +0xB set, or whose
// byte +0xC equals key with +0xE clear and +0xF set. 0 when none. Words are 32-bit.
#include <cstdint>

extern "C" int32_t Storm13c_38F10(const uint32_t* obj, uint8_t key) {
    int32_t best = 0;
    int32_t n = (int32_t)obj[5];
    if (n < 1) n = 0;
    for (; n > 0; n = *(const int32_t*)(uintptr_t)((uint32_t)n + 4)) {
        const uint8_t* b = (const uint8_t*)(uintptr_t)(uint32_t)n;
        const bool hit = (b[8] == key && b[0xA] == 0 && b[0xB] != 0) || (b[0xC] == key && b[0xE] == 0 && b[0xF] != 0);
        const int32_t v = *(const int32_t*)(b + 0x14);
        if (hit && best < v) best = v;
    }
    return best;
}
