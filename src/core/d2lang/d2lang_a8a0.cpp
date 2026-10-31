// D2Lang key -> string pointer (ordinal 10011): the out slot of the key lookup, the lookup's index dropped.
#include <cstdint>

// 1.13c D2Lang+0xA8A0 (fastcall ECX = key): out = 0; 0xA7B0(key, &out); return out.
extern "C" uintptr_t d2lang_a8a0(const char* key, void (*a7b0)(const char* key, const void** out)) {
    const void* out = nullptr;
    a7b0(key, &out);
    return (uintptr_t)out;
}
