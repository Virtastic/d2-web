// 1.13c Fog+0x18580: size of today's log file for a log name.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <mutex>

extern "C" void* fog_sub_2593(const char* pName, const char* pMode);
extern "C" long fog__ftell(FILE* f);
extern "C" int fog__fseek(FILE* f, long off, int origin);
extern "C" int fog_fclose(FILE* f);
extern "C" int fog_snprintf(char* dest, size_t count, const char* fmt, ...);

// Retail DAT_6ffa6ea4 (log critical section), DAT_6ff7f368 (log dir prefix), and the 20-slot
// name/FILE* table (0x6ff816f8 stride 16, 0x6ff81900) closed by 0x180D0 and filled by 0x18C40.
// ponytail: table + prefix are local until 0x18C40/0x180D0 are mapped; hook them then.
static std::recursive_mutex g_fogLogLock;
static const char g_fogLogDir[] = "";
static char g_fogLogName[20][16];
static FILE* g_fogLogFile[20];

// 1.13c Fog+0x180D0: close the open log slot whose name matches, clear its FILE*. The table and the closer are parameters so the
// capture probe can run it over the pinned retail table (names = 20 x 16 bytes at Fog+0x316F8, files = 20 FILE* at Fog+0x31900).
extern "C" void fog_log_close_named(const char (*names)[16], FILE** files, const char* name, int (*closer)(FILE*)) {
    for (int i = 0; i < 20; i++)
        if (files[i] && !strcmp(names[i], name)) { closer(files[i]); files[i] = nullptr; return; }
}
extern "C" void fog_sub_180d0(const char* name) {  // 1.13c Fog+0x180D0
    fog_log_close_named(g_fogLogName, g_fogLogFile, name, fog_fclose);
}

extern "C" long fog_sub_18580(const char* name) {
    std::lock_guard<std::recursive_mutex> lk(g_fogLogLock);
    fog_sub_180d0(name);
    time_t t = time(nullptr);
    struct tm tm; localtime_r(&t, &tm);
    char path[260];
    fog_snprintf(path, sizeof path, "%s%s%02d%02d%02d.txt", g_fogLogDir, name, (tm.tm_year + 1900) % 100, tm.tm_mon + 1, tm.tm_mday);
    long n = 0;
    FILE* f = (FILE*)fog_sub_2593(path, "r");
    if (f) { fog__fseek(f, 0, SEEK_END); n = fog__ftell(f); fog_fclose(f); }
    return n;
}
