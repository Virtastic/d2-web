#include <cstdint>

// 1.13c D2Gfx+0xBF99 TypeMatch (MSVC 7.1 EH): ESI=catchable type, EDI=handler type, stack arg = handler
// (adjectives at +0). Pointers are 32-bit fields in retail layout. Returns 1 when the handler catches it.
extern "C" int32_t d2gfx_strcmp(const char*, const char*);
extern "C" int32_t d2gfx_type_match(uint32_t adjectives, const uint8_t* esi, const uint8_t* edi) {
    auto rd = [](const uint8_t* p) { return *reinterpret_cast<const uint32_t*>(p); };
    auto ti = [](uint32_t v) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(v)); };
    const uint8_t* t = ti(rd(esi + 4));
    if (t && t[8]) {
        const uint8_t* u = ti(rd(edi + 4));
        if (t != u && d2gfx_strcmp(reinterpret_cast<const char*>(t + 8), reinterpret_cast<const char*>(u + 8)) != 0) return 0;
        if (((*edi & 2) && !(*esi & 8)) || ((adjectives & 1) && !(*esi & 1)) || ((adjectives & 2) && !(*esi & 2))) return 0;
    }
    return 1;
}
