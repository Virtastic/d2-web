#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
struct RetailSehRecord {
    uint32_t previous;
    uint32_t handler;
    uint32_t scope;
    uint32_t frame;
};
static_assert(offsetof(RetailSehRecord, handler) == 4);
static_assert(offsetof(RetailSehRecord, scope) == 8);
static_assert(offsetof(RetailSehRecord, frame) == 12);
static_assert(sizeof(RetailSehRecord) == 16);

uint32_t read_word(const void* address) {
    uint32_t value;
    std::memcpy(&value, address, sizeof(value));
    return value;
}
}

// 1.13c D2Game+0x0789A
// FS:[0] is supplied by the host: wasm has no x86 SEH chain. Embedded addresses
// remain retail-width words; the handler identity is the retail absolute address.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_abnormal_termination(const void* exception_list) {
    const auto* record = static_cast<const uint8_t*>(exception_list);
    if (read_word(record + offsetof(RetailSehRecord, handler)) != 0x6fc27810u)
        return 0;
    const uint32_t frame = read_word(record + offsetof(RetailSehRecord, frame));
    return read_word(record + offsetof(RetailSehRecord, scope)) ==
           read_word(reinterpret_cast<const void*>(static_cast<uintptr_t>(frame) + 12));
}
