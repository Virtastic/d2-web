// 1.13c Fog+0x10660 (__fastcall: ECX = sink handle, EDX = sink or NULL). The crash-report header: five lines
// (program name with the version suffix, computer name, user name, local time, an empty line), each formatted into one
// 0x200-byte buffer and handed to the sink as sink(handle, line). A NULL sink is the +0xFD80 default.
// The host calls (GetLocalTime, GetUserNameA, GetComputerNameA, the version text at +0x32B08) are the FogEnv-style seam below,
// so a vector can pin them. Order of the host calls is retail's: local time, user name, computer name.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

struct Fog10660Env {
    void (*localTime)(uint16_t t[8]);                      // GetLocalTime: year, month, dow, day, hour, minute, second, ms
    int32_t (*userName)(char* buf, uint32_t* count);       // GetUserNameA
    int32_t (*computerName)(char* buf, uint32_t* count);   // GetComputerNameA
    const char* program;                                   // +0x2FCD8 ("Diablo II")
    const char* version;                                   // +0x32B08 (empty: no suffix)
};

static void fog_10660_default_sink(void*, const char* line) {   // 1.13c Fog+0xFD80: the line, then the terminator "\n"
    std::fputs(line, stderr);
    std::fputc('\n', stderr);
}

extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_10660(const Fog10660Env* e, void* h,
                                                   void (*sink)(void*, const char*)) {   // 1.13c Fog+0x10660
    if (sink == nullptr) sink = fog_10660_default_sink;
    uint16_t t[8];
    e->localTime(t);
    char user[64];
    uint32_t n = 0x40;
    if (e->userName(user, &n) == 0) user[0] = '\0';
    char comp[64];
    n = 0x40;
    if (e->computerName(comp, &n) == 0) comp[0] = '\0';
    char program[0x104];
    if (e->version[0] == '\0') {
        std::strncpy(program, e->program, 0x104);
        program[0x103] = '\0';
    } else {
        std::snprintf(program, 0x104, "%s %s", e->program, e->version);
    }
    char line[0x200];
    std::snprintf(line, 0x200, "PROGRAM:       %s", program);
    sink(h, line);
    std::snprintf(line, 0x200, "COMPUTER NAME: %s", comp);
    sink(h, line);
    std::snprintf(line, 0x200, "USER NAME:     %s", user);
    sink(h, line);
    std::snprintf(line, 0x200, "TIME:          %04u-%02u-%02u %02u:%02u:%02u.%03u", (unsigned)t[0], (unsigned)t[1],
                  (unsigned)t[3], (unsigned)t[4], (unsigned)t[5], (unsigned)t[6], (unsigned)t[7]);
    sink(h, line);
    line[0] = '\0';   // +0x6FF768EC is the empty format
    sink(h, line);
}
