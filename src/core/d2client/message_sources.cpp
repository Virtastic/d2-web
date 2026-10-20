// Prior art used: D2MOO Storm.h identifies #501 as SStrCopy; community client pointer
// tables provide no message-source owner. Retail 1.13c DF00/2F70/2400 cached bodies
// plus DCA0/DD20/DED0/CDDB0/CDDF0 disassembly determine this shared registry.
#include "message_sources.h"
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

uintptr_t d2_client_message_sources[3];
uint32_t d2_client_message_sources_initialized;
std::recursive_mutex d2_client_message_sources_mutex;

namespace {
void* allocate_source(uint32_t bytes, const char*, uint32_t, uint32_t) {
    return std::malloc(bytes);
}
// Retail's CRT startup entry 0xCDDB0 constructs the shared list and registers its
// destructor 0xCDDF0. Native construction runs before any source registration.
struct SourceListLifetime {
    SourceListLifetime() { d2_fn_d2client_2f00(d2_client_message_sources); }
    ~SourceListLifetime() { d2_fn_d2client_2ec0(d2_client_message_sources); }
} sourceListLifetime;
}

// 1.13c D2Client+0xDED0: initialize the shared critical section once and register cleanup.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ded0() {
    if (d2_client_message_sources_initialized == 0) {
        std::atexit(d2_client_dd20);
        d2_client_message_sources_initialized = 1;
    }
}

// 1.13c D2Client+0xDF00: ESI is 1 in the caller's disassembly, so this inserts
// before the sentinel. The callback is a native function pointer, never a retail ID.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_df00(D2ClientMessageSource source) {
    d2_client_ded0();
    std::lock_guard<std::recursive_mutex> guard(d2_client_message_sources_mutex);
    auto* node = static_cast<D2ClientMessageNode*>(d2_fn_d2client_2f70(
        reinterpret_cast<uintptr_t>(d2_client_message_sources), 0, 0, 1, allocate_source));
    // Retail dereferences allocation unconditionally; preserve that failure contract.
    node->source = source;
}

// 1.13c D2Client+0xDD20: unlink/free from the tail until the encoded sentinel,
// then release the critical-section lifetime. The native mutex's storage persists.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_dd20() {
    {
        std::lock_guard<std::recursive_mutex> guard(d2_client_message_sources_mutex);
        while (static_cast<intptr_t>(d2_client_message_sources[2]) > 0) {
            auto* node = reinterpret_cast<uintptr_t*>(d2_client_message_sources[2]);
            d2_fn_d2client_2ff0(node);
            std::free(node);
        }
    }
    d2_client_message_sources_initialized = 0;
}

int32_t d2_client_try_message_sources(uint32_t message, char* output, uint32_t capacity) {
    if (!d2_client_message_sources_initialized) return 0;
    std::lock_guard<std::recursive_mutex> guard(d2_client_message_sources_mutex);
    uintptr_t current = d2_client_message_sources[2];
    while (static_cast<intptr_t>(current) > 0) {
        auto* node = reinterpret_cast<D2ClientMessageNode*>(current);
        const int32_t handled = node->source(message, output, capacity);
        if (handled) return handled;
        current = node->previous + d2_client_message_sources[0];
    }
    return 0;
}

// Retail D2Client+0xDCA0 (not present in the current function list import): run the shared
// registry, or Storm #501 SStrCopy(output, "D2Hell Fault.cpp", capacity) if none handles
// this 32-bit language/message ID. Register arguments EBX/EDI are output/capacity.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_dca0(uint32_t message, char* output, uint32_t capacity) {
    if (d2_client_try_message_sources(message, output, capacity)) return;
    if (capacity == 0) return;
    constexpr char fallback[] = "D2Hell Fault.cpp";
    const size_t bytes = capacity <= sizeof(fallback) ? capacity - 1u : sizeof(fallback) - 1u;
    std::memcpy(output, fallback, bytes);
    output[bytes] = 0;
}
