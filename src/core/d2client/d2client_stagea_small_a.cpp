// Prior art used: UI/gameplay rows and D2Client brief;
// D2MOO has no D2Client implementation. Rules below come from the cached
// 1.13c bodies, with register arguments checked in the retail instructions.
// Native pointers are explicit: pointer-bearing projections retain the scalar
// byte offsets below, while bucket arrays and exception links use native stride.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace {
template<class T> T load(const void* p, size_t offset) {
    T value;
    std::memcpy(&value, static_cast<const uint8_t*>(p) + offset, sizeof value);
    return value;
}
template<class T> void store(void* p, size_t offset, T value) {
    std::memcpy(static_cast<uint8_t*>(p) + offset, &value, sizeof value);
}
}

// 1.13c D2Client+0x537A0
// ECX is the signed key; live head is DAT_6fbc9608. Each node's next
// pointer lives at scalar offset 0x1004 (unaligned on wasm64).
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_client_537a0(int32_t key, void* head_119608) {
    for (void* node = head_119608; node; node = load<void*>(node, 0x1004))
        if (load<int32_t>(node, 0) == key) return node;
    return nullptr;
}

// 1.13c D2Client+0x62C90
// Platform import receives the actual native module handle, before both globals
// are cleared. The return value of FreeLibrary is deliberately ignored.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_62c90(void** module_11c1ac, uint32_t* state_11c0a4,
                                int (*free_library)(void*)) {
    if (*module_11c1ac) free_library(*module_11c1ac);
    *module_11c1ac = nullptr;
    *state_11c0a4 = 0;
}

// 1.13c D2Client+0x72970
// ECX is unit; helper 0xC7B00 receives that same unit in EAX. Its complete
// effect is included here: reuse/allocate 0x4c bytes, clear all 19 dwords,
// publish at path+0x30, then set bit 0x10. Even a null unit allocates/clears;
// retail leaves that allocation unattached, so no extra free is introduced.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_72970(void* unit) {
    void* path = unit ? load<void*>(unit, 0x54) : nullptr;
    auto* flags = path ? load<uint32_t*>(path, 0x30) : nullptr;
    if (!flags) {
        flags = static_cast<uint32_t*>(std::malloc(0x4c));
        if (!flags) std::abort();
    }
    std::memset(flags, 0, 0x4c);
    if (unit) {
        path = load<void*>(unit, 0x54);
        if (path) store(path, 0x30, flags);
    }
    if (unit) {
        path = load<void*>(unit, 0x54);
        if (path) {
            flags = load<uint32_t*>(path, 0x30);
            if (flags) *flags |= 0x10u;
        }
    }
}

// 1.13c D2Client+0x8BE10
// Both predicates read live DAT_6fbcbc34. Values are unsigned for the
// switch only: all values outside the listed finite set return zero.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_8be10(uint32_t mode_11bc34) {
    switch (mode_11bc34) {
    case 0: case 1: case 2: case 11: case 12: case 14: case 15: case 19: return 1;
    default: return 0;
    }
}

// 1.13c D2Client+0x8BE60
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_8be60(uint32_t mode_11bc34) {
    switch (mode_11bc34) {
    case 0: case 1: case 2: case 11: case 12: case 14: case 19: return 1;
    default: return 0;
    }
}

// Native projection of __getptd()->_curexception's object/link pairs;
// objects and links are pointers, never uint32_t pointer surrogates.
struct D2ClientExceptionLinkSmallA {
    void* object;
    const D2ClientExceptionLinkSmallA* next;
};

// 1.13c D2Client+0xCB0FF
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_cb0ff(const void* object,
                               const D2ClientExceptionLinkSmallA* current_exception) {
    for (auto* link = current_exception; link; link = link->next)
        if (link->object == object) return 0;
    return 1;
}

// 1.13c D2Client+0xCC741
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_cc741(uint32_t a, uint32_t b, uint32_t* result) {
    const uint32_t sum = a + b;
    *result = sum;
    return sum < a || sum < b;
}

// 1.13c D2Client+0x0A71B
// Live __pioinfo table at DAT_6fbd39a0; its blocks keep the retail 0x24
// byte file-record stride. Only the platform knows the opaque lock layout.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a71b(int32_t file_handle, uint8_t* const* pioinfo_1239a0,
                               void (*leave_critical_section)(void*)) {
    uint8_t* block = pioinfo_1239a0[file_handle >> 5];
    leave_critical_section(block + 0xc + (uint32_t(file_handle) & 31u) * 0x24);
}

// 1.13c D2Client+0xA6040
// ECX key, EDX type, stack value. The 0xA4E20 lookup body is included:
// EAX=key&127; row = DAT_6fbba608 + type*0x200 retail bytes. In the
// native projection each type owns 128 native-width bucket pointers.
// Node scalar type/id offsets are 0/0xc; next is at byte offset 0xe4.
// D2Common ordinal 11129 receives (stack value, found node), including null.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a6040(uint32_t key, uint32_t type, uint32_t value,
                                void* const* buckets_10a608,
                                void (*ordinal_11129)(uint32_t, void*)) {
    void* node = buckets_10a608[size_t(type) * 128 + (key & 127u)];
    while (node && load<uint32_t>(node, 0xc) != key)
        node = load<void*>(node, 0xe4);
    if (node && load<uint32_t>(node, 0) != type) std::abort();
    ordinal_11129(value, node);
}

// Negative results (no implementation markers): AD680 copies 40 bytes then
// forwards its stack copy in EAX to essential packet handler 7DE50; B2980
// performs ordinal 10737/10970 then essential B17C0; 81720 calls ordinal
// 10063 on globals 11BF60 and 11BF64, then 814F0 with ECX=ESI if nonzero.
// Those essential local bodies are not linked here; retaining UNMAPPED is
// preferable to representing an absent effect as an implemented wrapper.
