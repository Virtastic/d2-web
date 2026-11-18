// D2CMP GfxHash node insert. Retail 32-bit node layout (the probe is wasm32): +0 bucket, +4 id, +8..+0x18 key words, +0x1C hash (written by 16E80),
// +0x20 stack arg 0, +0x28 prev, +0x2C next. The allocator and the record hash (16E80) are injected.
#include <cstdint>
#include <cstring>

struct D2Cmp16F70Env {
    uint8_t* (*alloc)(const char* file, int line, int z);   // thunk 0x8D84 (EDX = 0x30), Ordinal_10045(GfxHash.cpp, 0xEF, 0)
    // Fog-side 0x16E80: EAX = key, ECX = arg1, EDX = &zeroed stack slot, stack word = &node+0x1C, EBX = table, ESI = node; returns the bucket
    uint32_t (*hashKey)(const uint8_t* key, uint32_t arg1, uint32_t* slot, uint32_t* outHash, uint32_t* table, uint8_t* node);
};

static uint32_t rd(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static void wr(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }

// 1.13c D2CMP+0x16F70 (EBX = table, EDI = key; stack arg0, arg1; ret 8)
uint8_t* d2cmp_16f70(uint32_t* table, const uint8_t* key, uint32_t arg0, uint32_t arg1, const D2Cmp16F70Env* e) {
    uint8_t* n = e->alloc("..\\Source\\D2CMP\\SRC\\GfxHash.cpp", 0xef, 0);
    wr(n + 0x20, arg0);
    uint32_t slot = 0;
    uint32_t b = e->hashKey(key, arg1, &slot, reinterpret_cast<uint32_t*>(n + 0x1c), table, n);
    wr(n + 8, rd(key + 0x18)); wr(n + 0xc, rd(key + 0x1c)); wr(n + 0x10, rd(key + 0x20)); wr(n + 0x14, rd(key + 0x24)); wr(n + 0x18, rd(key + 0x28));
    uint32_t kind = rd(key + 8);
    if (kind == 6 && (key[4] & 2)) wr(n + 4, rd(key + 0x14));
    else if (kind == 4 || kind == 3) wr(n + 4, rd(key + 0xc));
    wr(n + 0x28, 0);
    uint32_t head = table[0x8000 / 4 + b];
    wr(n + 0x2c, head);
    if (head) wr(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(head)) + 0x28, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(n)));
    wr(n, b);
    table[0x8000 / 4 + b] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(n));
    return n;
}
