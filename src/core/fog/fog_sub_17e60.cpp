// src/core/fog/fog_sub_17e60.cpp -- 1.13c Fog.dll ordinal 10116 (GetInstallPath), from
// build-wasm/ghidra-fog/decomp/0x17E60.c (alpha lane: NAMED, no spec, no vector; stateful).
#include <cstdint>
#include <cstring>

namespace {
// Host seams (registry, GetFileAttributesA, GetModuleFileNameA); default: nothing available.
int (*g_regRead)(char*, uint32_t) = [](char*, uint32_t) { return 0; };
void (*g_regWrite)(const char*) = [](const char*) {};
bool (*g_exists)(const char*) = [](const char*) { return false; };
void (*g_exePath)(char*, uint32_t) = [](char* b, uint32_t) { b[0] = '\0'; };
}  // namespace

// 1.13c Fog+0x17E60
void fog_sub_17e60(char* buf, uint32_t size) {
    if (g_regRead(buf, size) == 0) buf[0] = '\0';
    if (!g_exists(buf) || buf[0] == '\0') {
        buf[0] = '\0';
        g_exePath(buf, size);
        buf[size - 1] = '\0';
        char* s = std::strrchr(buf, '\\');
        if (s) s[1] = '\0';
        g_regWrite(buf);
    }
    if (size != 0) {
        uint32_t n = (uint32_t)std::strlen(buf);
        if ((n == 0 || buf[n - 1] != '\\') && n + 1U < size) {
            buf[n] = '\\';
            buf[n + 1] = '\0';
        }
    }
}
