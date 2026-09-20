// M0a smoke surface: prove the whole loop (emscripten build -> content-addressed deploy ->
// streamfs -> StormLib) with no D2 content anywhere in it. Exercised by
// wasm-build/scenarios/s00-smoke.mjs. Gone once real subsystems replace it.
#include <cstdint>
#include <cstring>
#include <string>
#include <emscripten.h>
#include <StormLib.h>

extern "C" {

// Open the MPQ at `mpq` (an emscripten FS path, typically a streamfs mount), read `name` out
// of it, and return its contents as "<len>:<fnv1a hex>" so the scenario can compare against
// what tools/mkmpq.mjs put in. Returns "ERR:<stage>:<GetLastError>" on any failure.
EMSCRIPTEN_KEEPALIVE
const char* d2_smoke_mpq(const char* mpq, const char* name) {
    static std::string out;
    HANDLE ha = nullptr, hf = nullptr;
    if (!SFileOpenArchive(mpq, 0, MPQ_OPEN_READ_ONLY, &ha)) {
        out = "ERR:open:" + std::to_string(SErrGetLastError()); return out.c_str();
    }
    if (!SFileOpenFileEx(ha, name, 0, &hf)) {
        out = "ERR:file:" + std::to_string(SErrGetLastError()); SFileCloseArchive(ha); return out.c_str();
    }
    DWORD size = SFileGetFileSize(hf, nullptr);
    std::string buf(size, '\0');
    DWORD got = 0;
    bool ok = SFileReadFile(hf, buf.data(), size, &got, nullptr) && got == size;
    SFileCloseFile(hf);
    SFileCloseArchive(ha);
    if (!ok) { out = "ERR:read:" + std::to_string(SErrGetLastError()); return out.c_str(); }
    uint32_t h = 2166136261u;
    for (unsigned char c : buf) { h ^= c; h *= 16777619u; }
    char hex[9]; snprintf(hex, sizeof hex, "%08x", h);
    out = std::to_string(size) + ":" + hex;
    return out.c_str();
}

// Assert retail-style uint32_t wrapping and the native wasm64 pointer width at runtime;
// a toolchain change that breaks either contract must fail s00 before the parity scenarios.
EMSCRIPTEN_KEEPALIVE
uint32_t d2_smoke_i32(void) {
    volatile uint32_t a = 0xFFFFFFFFu; a += 2;      // wraps to 1
    volatile int32_t b = 0x7FFFFFFF;                // signed overflow via unsigned path
    uint32_t c = (uint32_t)b + 1u;                  // 0x80000000
    return a + (c >> 31) + (uint32_t)sizeof(void*); // 1 + 1 + 8 = 10
}

} // extern "C"

int main() { return 0; }
