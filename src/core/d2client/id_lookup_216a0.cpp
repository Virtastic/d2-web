// D2Client+0x216A0: keyed walk of the singly-linked list whose head is the module global at RVA 0x1C4D4.
// Node: key at +8, value at +0xC, next at +0x30. Returns the value of the first node with key == arg, else -1.
// The head's storage is a parameter (the pin the probe receives): retail reads it from 0x1C4D4.
#include <cstdint>
#include <cstring>

namespace {
uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
}

// 1.13c D2Client+0x216A0: ecx = key, plain `ret`, result in eax.
extern "C" uint32_t d2_client_216a0(const uint32_t* head, uint32_t key) {
    uint32_t n = head ? *head : 0;
    while (n) {
        if (rd((const uint8_t*)(uintptr_t)n, 8) == key) return rd((const uint8_t*)(uintptr_t)n, 0xC);
        n = rd((const uint8_t*)(uintptr_t)n, 0x30);
    }
    return 0xFFFFFFFFu;
}
