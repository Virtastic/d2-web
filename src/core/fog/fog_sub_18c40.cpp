// 1.13c Fog+0x18C40 (ordinal 10235): read bytes [from,to) of today's log file "<dir><name>YYMMDD.txt"
// into a freshly allocated zeroed buffer. Callees are unported rows, so they are seams.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct fog_18c40_api {
    void* (*alloc)(void* ecx0);                                   // Fog+0x1B560 (called with 0)
    void (*fatal)(int level, const char* file, uint32_t line, const char* fmt, ...); // Fog+0x10F30
    void (*lock)();                                               // EnterCriticalSection(0x6FFA6EA4)
    void (*unlock)();                                             // LeaveCriticalSection
    void (*prepare)(const char* name);                            // Fog+0x180D0
    void (*localtime)(uint32_t* year, uint32_t* month, uint32_t* day); // GetLocalTime
    const char* logdir;                                           // DAT_6ff7f368
};

void* fog_sub_18c40(const fog_18c40_api* a, const char* name, int from, int to) {
    uint32_t n = (uint32_t)(to - from) + 1;
    void* buf = a->alloc(nullptr);
    if (!buf) a->fatal(2, "..\\Source\\Fog\\Src\\LogManager.cpp", 0xf7, "");
    memset(buf, 0, n);
    a->lock();
    a->prepare(name);
    uint32_t y, m, d;
    a->localtime(&y, &m, &d);
    char path[260];
    snprintf(path, sizeof path, "%s%s%02u%02u%02u.txt", a->logdir, name, y % 100, m, d);
    FILE* f = fopen(path, "r");
    if (f) {
        if (fseek(f, from, SEEK_SET) == 0) fread(buf, 1, (size_t)(to - from), f);
        fclose(f);
    }
    a->unlock();
    return buf;
}
