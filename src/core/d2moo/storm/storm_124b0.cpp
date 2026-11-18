// 1.13c Storm+0x124B0 (no arguments): drain the SNET provider's receive queue. The provider table (retail 0x6FC45834) holds a receive slot (+0x30,
// stdcall: &sender, &name, &data) and a dispatch slot (+0x10, stdcall: sender, name, data). While a receive succeeds with all three pointers
// non-null, a message whose sender and name strings are both empty is a system message: a user-event node is taken from the SNET list (0x5170),
// stamped type 4 / id -1, and given a copy of the data string (strlen + 1 bytes, SMemAlloc "SNET.CPP" line 0x8F3) with its length; every message
// is then handed to dispatch and the three locals are zeroed before the next receive.
#include <cstdint>
#include <cstring>

extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);

// recv/dispatch/newNode are the provider slots and the 0x5170 node allocator (the node is 0x18+ bytes: words 2 = type, 3 = id, 4 = copy, 5 = length).
extern "C" void Storm_124B0_Run(int (*recv)(const char**, const char**, const char**), void (*dispatch)(const char*, const char*, const char*),
                                uint32_t* (*newNode)()) {
    const char *a = nullptr, *b = nullptr, *d = nullptr;
    int ok = recv(&a, &b, &d);
    while (ok && a && b && d) {
        if (*a == 0 && *b == 0) {
            const uint32_t n = (uint32_t)strlen(d) + 1;
            uint32_t* node = newNode();
            node[2] = 4;
            node[3] = 0xffffffffu;
            void* c = Storm_Sub1C580(n, "SNET.CPP", 0x8f3, 0);
            node[4] = (uint32_t)(uintptr_t)c;
            memcpy(c, d, n);
            node[5] = n;
        }
        dispatch(a, b, d);
        a = b = d = nullptr;
        ok = recv(&a, &b, &d);
    }
}
