// src/core/fog/fog_18430.cpp -- 1.13c Fog.dll sub_18430 (log file name builder), from build-wasm/ghidra-fog/decomp/0x18430.c.
// Starts from an empty 260-byte buffer; when the program-directory helper (Fog+0x143D0, EAX = 0x104, ESI = buffer) reports a name it
// appends "\" (Storm ordinal 503, bounded append), then Fog+0x17C40 (fastcall, ECX = buffer) creates the directories. Last, the global
// 260-byte log-name buffer (DAT_6ff7f368) is formatted from "%s%s" with that buffer and the argument (Storm ordinal 578).
#include <cstdint>

struct Fog18430Api {
    uint32_t (*prog_dir)(char* buf, uint32_t cap);                          // Fog+0x143D0
    void (*pack)(char* dst, const char* src, uint32_t cap);                 // Storm ordinal 503
    void (*make_dirs)(char* path);                                          // Fog+0x17C40
    void (*format)(char* dst, uint32_t cap, const char* fmt, const char* a, const char* b);  // Storm ordinal 578
};

// 1.13c Fog+0x18430 (stdcall, one argument)
extern "C" void fog_log_name_18430(const Fog18430Api* api, char* logName /* DAT_6ff7f368, 0x104 bytes */, const char* name) {
    char dir[0x104];
    dir[0] = '\0';
    if (api->prog_dir(dir, 0x104) != 0) {
        api->pack(dir, "\\", 0x104);
        api->make_dirs(dir);
    }
    api->format(logName, 0x104, "%s%s", dir, name);
}
