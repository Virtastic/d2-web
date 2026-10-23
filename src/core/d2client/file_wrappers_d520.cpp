#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

// Prior art used: D2MOO has no D2Client implementation. Its Fog.h and Storm.h supply the
// file contracts (Fog 10103-10105, Storm 276). The local 1.13c decompiles and x86
// disassembly establish the register arguments omitted by Ghidra below.
// The owner supplies the actual file service and diagnostic functions; handles
// and buffers remain native pointers, never truncated retail address tokens.
#include "file_services.h"

[[noreturn]] static void fileFailure(const D2ClientFileServices& services, uint32_t line) {
    services.fatal(line);
    std::abort(); // The retail exit call cannot return.
}

// 1.13c D2Client+0xd520
// ESI=file, EDI=length, [entry ESP+4]=destination. All seven arguments
// to Fog ordinal 10104 are visible in the disassembly, including three zeros.
// Only that synchronous, zero-request subset is bound by this adapter.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_d520(
    void* file, uint32_t length, void* destination, const D2ClientFileServices* services) {
    if (!file) fileFailure(*services, 0x87);
    uint32_t bytesRead;
    if (!services->readSynchronous(file, destination, length, &bytesRead)) {
        char filename[0x104];
        services->fileName(file, filename, sizeof(filename));
        fileFailure(*services, 0x8f);
    }
    if (bytesRead != length) fileFailure(*services, 0x92);
}

// 1.13c D2Client+0x0D610
// ESI=file, [entry ESP+4]=output pointer. Success preserves Fog's EAX.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_d610(
    void* file, uint32_t* size, const D2ClientFileServices* services) {
    if (!file) fileFailure(*services, 0x59);
    const uint32_t result = services->size(file, size);
    if (!result) {
        char filename[0x104];
        services->fileName(file, filename, sizeof(filename));
        fileFailure(*services, 0x5f);
    }
    return result;
}

// ECX=file; success tail-jumps to Fog ordinal 10103 and returns its EAX.
// 1.13c D2Client+0x0d670
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_d670(
    void* file, const D2ClientFileServices* services) {
    if (!file) fileFailure(*services, 0x49);
    return services->close(file);
}
