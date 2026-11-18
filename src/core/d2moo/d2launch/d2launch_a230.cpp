// 1.13c D2Launch+0xA230  sub_A230: per-key buffer cache (CharSel.cpp). EAX = key string, ECX = source bytes, stack = byte count (ret 4).
// The key's Storm 502 hash (thunk 0x97AE) selects a node of the list at 0x25EF4 ({hash, data, expiry, next}, 16 bytes, retail 32-bit
// pointers). A hit frees the old data (Storm 403, 0x9766) and stores a fresh copy (Storm 401, 0x9760); a miss allocates a node and a copy
// and pushes the node at the head. Either way expiry := GetTickCount() + 300000. g = the module range [0x25EF0, 0x25F00).
#include <cstdint>
#include <cstring>

struct D2lA230Seams {
    uint32_t (*o97ae)(const char* key, uint32_t z0, uint32_t z1);                     // Storm 502, stdcall
    uint8_t* (*o9760)(uint32_t size, uint32_t fileRva, uint32_t line, uint32_t z);     // Storm 401 alloc, stdcall
    void (*o9766)(uint8_t* p, uint32_t fileRva, uint32_t line, uint32_t z);            // Storm 403 free, stdcall
    uint32_t (*tick)();                                                                // kernel32 GetTickCount (IAT 0x1C364)
};

static constexpr uint32_t kBase = 0x25EF0, kFile = 0x1EB84;

extern "C" void d2l_a230(const D2lA230Seams* s, uint8_t* g, const char* key, const uint8_t* src, uint32_t size) {
    uint32_t& head = *(uint32_t*)(g + (0x25EF4 - kBase));
    const uint32_t h = s->o97ae(key, 0, 0);
    uint32_t* n = (uint32_t*)(uintptr_t)head;
    while (n && n[0] != h) n = (uint32_t*)(uintptr_t)n[3];
    if (n) {
        s->o9766((uint8_t*)(uintptr_t)n[1], kFile, 0xF1, 0);
        uint8_t* d = s->o9760(size, kFile, 0xF2, 0);
        n[1] = (uint32_t)(uintptr_t)d;
        memcpy(d, src, size);
        n[2] = s->tick() + 300000u;
        return;
    }
    n = (uint32_t*)s->o9760(0x10, kFile, 0xFA, 0);
    n[0] = h;
    uint8_t* d = s->o9760(size, kFile, 0xFC, 0);
    n[1] = (uint32_t)(uintptr_t)d;
    memcpy(d, src, size);
    const uint32_t t = s->tick();
    n[3] = head;
    head = (uint32_t)(uintptr_t)n;
    n[2] = t + 300000u;
}
