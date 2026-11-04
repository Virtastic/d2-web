// 1.13c Fog+0x14D10 (ordinal 10263): load and delete the "Crashdump" file next to the exe, fold its records
// (4 dwords {a,b,c,n} + n dwords of stack) into a deduped table at `out`:
// byte count, then entries {u16 hits, u32 a, u32 b, u32 c, u8 n, n*4 bytes}.
// The host side (exe dir, directory creation, file attributes, stdio, allocator, delete) comes in through FogCrashEnv.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct FogCrashEnv {
    int (*exeDir)(char* out);                 // Fog+0x143D0 and the copy of its directory (with the trailing backslash); 0 = none
    void (*mkdirs)(char* path);               // Fog+0x17C40
    uint32_t (*attrs)(const char* path);      // GetFileAttributesA
    void* (*fopen)(const char* path);         // fopen(path, "rb")
    int (*fseekEnd)(void* f);                 // fseek(f, 0, SEEK_END)
    long (*ftell)(void* f);
    void (*fseekSet)(void* f);                // fseek(f, 0, SEEK_SET)
    void* (*alloc)(uint32_t n);               // Fog+0x1B560 with Exceptions.cpp line 0x986
    void (*fread)(void* dst, uint32_t n, void* f);
    void (*fclose)(void* f);
    void (*del)(const char* path);            // DeleteFileA
    void (*release)(void* p);                 // retail leaks the buffer; the engine's malloc'd copy is freed here (null = keep)
};

int fog_crashdump_fold(uint8_t* out, uint32_t* lenInOut, const FogCrashEnv* env)
{
    char path[0x104]; path[0] = 0;
    if (env->exeDir(path)) env->mkdirs(path);
    strncat(path, "Crashdump", 0x104 - strlen(path) - 1);
    if (env->attrs(path) == 0xFFFFFFFFu) return 0;
    const uint32_t cap = *lenInOut;
    void* f = env->fopen(path);
    if (!f) return 0;
    if (env->fseekEnd(f)) { env->fclose(f); return 0; }
    const long n = env->ftell(f);
    env->fseekSet(f);
    uint8_t* buf = (uint8_t*)env->alloc((uint32_t)n);
    if (!buf) abort();   // retail: Fog+0x10F30 assert (severity 2, Exceptions.cpp line 0x986)
    env->fread(buf, (uint32_t)n, f);
    env->fclose(f);
    env->del(path);
    *lenInOut = 1;
    uint8_t* end = buf + n;
    uint8_t* p = buf;
    int count = 0;
    // ponytail: retail trusts the file; the bounds below only stop a truncated or hostile record, in-range input folds identically
    while (p + 16 <= end) {
        uint32_t h[4];
        memcpy(h, p, 16);
        uint32_t sz = h[3] * 4;
        if (p + 16 + sz > end || sz > 255 * 4) break;
        const uint8_t* st = p + 16;
        p += 16 + sz;
        uint8_t* e = out + 1;
        int i = 0;
        for (; i < count; i++, e += sz + 15) {
            uint16_t hits; uint32_t a, b, c;
            memcpy(&hits, e, 2); memcpy(&a, e + 2, 4); memcpy(&b, e + 6, 4); memcpy(&c, e + 10, 4);
            if (a == h[0] && b == h[1] && c == h[2] && e[14] == (uint8_t)h[3] && !memcmp(e + 15, st, sz)) {
                hits++; memcpy(e, &hits, 2);
                break;
            }
        }
        if (i == count) {
            if (e + 0x2f >= out + cap) break;
            uint16_t one = 1;
            memcpy(e, &one, 2); memcpy(e + 2, h, 12); e[14] = (uint8_t)h[3]; memcpy(e + 15, st, sz);
            count++;
            *lenInOut += sz + 15;
        }
    }
    out[0] = (uint8_t)count;
    if (env->release) env->release(buf);
    return 1;
}

namespace {
int eExe(char*) { return 0; }
void eMk(char*) {}
uint32_t eAttr(const char* p) { FILE* f = fopen(p, "rb"); if (!f) return 0xFFFFFFFFu; fclose(f); return 0x20; }
void* eOpen(const char* p) { return fopen(p, "rb"); }
int eSeekEnd(void* f) { return fseek((FILE*)f, 0, SEEK_END); }
long eTell(void* f) { return ftell((FILE*)f); }
void eSeekSet(void* f) { fseek((FILE*)f, 0, SEEK_SET); }
void* eAlloc(uint32_t n) { return malloc(n ? n : 1); }
void eRead(void* d, uint32_t n, void* f) { fread(d, 1, n, (FILE*)f); }
void eClose(void* f) { fclose((FILE*)f); }
void eDel(const char* p) { remove(p); }
void eFree(void* p) { free(p); }
}

extern "C" int fog_sub_14d10(uint8_t* out, uint32_t* lenInOut)
{
    static const FogCrashEnv env = { eExe, eMk, eAttr, eOpen, eSeekEnd, eTell, eSeekSet, eAlloc, eRead, eClose, eDel, eFree };
    return fog_crashdump_fold(out, lenInOut, &env);
}
