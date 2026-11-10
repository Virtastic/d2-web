// 1.13c Storm+0x26AC0: probe the MPQ hash table (16-byte entries) for a name. Registers: ECX = table object, EAX = name,
// BL = platform; one stack word, of which the low 16 bits are the locale (ret 4). Returns the entry index, the best wildcard
// entry, or 0xFFFFFFFF.
#include <cstdint>
#include <cstring>

extern "C" {
// Storm+0x25ED0 with the crypt table (the pointer in the module global at RVA 0x53120) passed in.
static uint32_t storm26ac0_hash(const uint32_t* crypt, int32_t type, const char* s) {
    uint32_t h = 0x7fed7fed; int32_t seed = -0x11111112;
    for (; s && *s; s++) {
        int8_t c = (int8_t)((*s >= 'a' && *s <= 'z') ? *s - 32 : *s);
        h = (h + (uint32_t)seed) ^ crypt[type * 0x100 + c];
        seed = (int32_t)((uint32_t)seed * 0x21 + (uint32_t)(int32_t)c + 3 + h);
    }
    return h;
}
static uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

uint32_t Storm_Sub26AC0Full(const uint32_t* crypt, const uint8_t* self, const char* name, uint8_t platform, uint32_t localeWord) {
    const uint32_t h0 = storm26ac0_hash(crypt, 0, name), h1 = storm26ac0_hash(crypt, 1, name), h2 = storm26ac0_hash(crypt, 2, name);
    const uint8_t* base = (const uint8_t*)(uintptr_t)rd32(self + 0x138);
    const uint32_t mask = rd32((const uint8_t*)(uintptr_t)rd32(self + 0x130) + 0x18) - 1;
    const int16_t locale = (int16_t)localeWord;
    const uint32_t start = h0 & mask;
    uint32_t i = start, best = 0xffffffffu;
    for (;;) {
        const uint8_t* e = base + i * 0x10;
        if (rd32(e + 12) == 0xffffffffu) return best;
        if (rd32(e) == h1 && rd32(e + 4) == h2 && rd32(e + 12) != 0xfffffffeu) {
            int16_t l; std::memcpy(&l, e + 8, 2);
            const uint8_t pl = e[10];
            if (l == locale && pl == platform) return i;
            if ((l == 0 || l == locale) && (pl == 0 || pl == platform)) best = i;
        }
        i = (i + 1) & mask;
        if (i == start) return best;
    }
}

// FN probe wrapper: r[0] ECX, r[1] EAX, r[2] EBX, r[3] stack word, r[14] the crypt blob (global 0x53120); result in r[4].
extern "C" void storm_port_26ac0(uint32_t* r) {
    r[4] = Storm_Sub26AC0Full((const uint32_t*)(uintptr_t)r[14], (const uint8_t*)(uintptr_t)r[0], (const char*)(uintptr_t)r[1],
                              (uint8_t)r[2], r[3]);
}
}
