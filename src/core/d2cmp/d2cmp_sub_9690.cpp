// 1.13c D2CMP+0x09690 -- transcribed from build-wasm/ghidra-d2cmp/decomp/0x9690.c. No vector (needs the DAT_6fe88328 list).
#include <cstdint>
#include <cstring>

// 1.13c D2CMP+0x09690 (100 bytes): walk the singly linked list from head (DAT_6fe88328), next at +0x10c;
// each node starts with a NUL-terminated UTF-16 name. Return the first node whose name equals `name`, else null.
extern "C" uint8_t* d2cmp_sub_9690(uint8_t* head, const uint8_t* name) {
    for (uint8_t* n = head; n; std::memcpy(&n, n + 0x10c, 4) ) {
        const uint8_t* a = n; const uint8_t* b = name;
        for (;; a += 2, b += 2) {
            if (a[0] != b[0] || a[1] != b[1]) break;
            if (!a[0] && !a[1]) return n;
        }
    }
    return nullptr;
}
