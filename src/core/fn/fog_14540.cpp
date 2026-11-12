// probe for Fog 0x14540 (sub_14540, crash-dump record append), over the HookEnv recorder. stdcall(a, b, c, pStack) with EDI = n:
// r[0] = n (EDI), r[1..3] = a, b, c, r[5] = pStack (blob stk, r[14]); lg = r[16], sc = r[18], exe = r[20] (the 16-byte path the
// GetModuleFileNameA stub stores; first byte 0 = failure). r[4] = 0 (void).
// Port: fog_sub_14540 in src/core/fog/fog_batch_n04.cpp covers the fopen/fwrite/fclose tail; the probe builds the file name as retail does
// (0x143D0 then "\\" then "Crashdump", the directory chain 0x17C40 recorded) and records the open (0x2593: path, mode, path bytes), the
// write (0x2896: size, count, the record) and the close (0x2C0B), in retail's call order. The port's fclose seam sits right after its
// fwrite, so the write record is logged from it.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "hookenv.h"
struct FogN04Seams {
    void (*validateCs)(void* cs); void (*enterCs)(void* cs); void (*leaveCs)(void* cs); void (*deleteCs)(void* cs);
    uint32_t (*nameOf)(uint32_t handle); void (*strCopy)(void* dst, uint32_t src, uint32_t max); void (*free_)(void* p);
    void (*sub5E550)(void); void (*sub64260)(void); void (*closeSocket)(uint32_t s);
    uint32_t (*getThreadContext)(void* hThread, uint32_t* ctx); void (*logPrint)(void* ctx, const char* text);
    void (*dumpBytes)(void* log, void* ctx, const void* p, uint32_t n, uint32_t a, uint32_t b); void (*fatal)(int32_t line);
    uint32_t (*tickCount)(void); void (*sleepMs)(uint32_t ms); uint32_t (*cpuidLeaf1)(uint32_t out[4]); void (*fclose_)(void* f);
};
extern FogN04Seams g_fogN04;
extern "C" int32_t fog_sub_14540(void* (*openAppend)(const char* name, const char* mode), const char* name, const void* record, uint32_t dwords);
extern "C" int fog_app_dir_143d0(char* buf);
namespace {
HookEnv* H;
const uint32_t* gRec; uint32_t gN, gFile;
// the bytes of a C string through hookblk's BYTES mask: whole words up to the terminator, nothing after it
void strBlk(uint32_t rva, const char* s) {
    uint32_t w[16] = {};
    const uint32_t len = (uint32_t)std::strlen(s) + 1;
    std::memcpy(w, s, len);
    H->blk(rva, w, (len + 3) / 4);
}
void* openAppend(const char* name, const char*) {
    gFile = H->h(0x2593, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)name, 0xB2027154u);
    strBlk(0x2593, name);
    static char fbuf[1024];   // a memory stream: the wasm build has no temp directory, so tmpfile() returns 0 and the write never happens
    return gFile ? (void*)fmemopen(fbuf, sizeof fbuf, "wb") : nullptr;
}
void closeF(void* f) {
    H->h(0x2896, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)gRec, gN * 4 + 0x10, 1, gFile);
    H->blk(0x2896, (const uint32_t*)gRec, 4 + gN);
    H->h(0x2C0B, 0, 0, 0, 0, 0, gFile);
    std::fclose((FILE*)f);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14540(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[16], (const uint32_t*)(uintptr_t)r[18]);
    H = &e;
    const uint32_t n = r[0];
    uint32_t rec[64] = { r[1], r[2], r[3], n };   // the record is {a, b, c, n} then the n dwords
    std::memcpy(rec + 4, (const void*)(uintptr_t)r[5], n * 4);   // retail's Ordinal_491(rec + 0x10, pStack, n * 4)
    gRec = rec; gN = n;
    char path[0x104] = {};
    const char* exe = (const char*)(uintptr_t)r[20];
    if (exe[0]) {
        std::memcpy(path, exe, 16);
        if (fog_app_dir_143d0(path)) {
            std::strcat(path, "\\");
            H->h(0x17C40, 0, (uint32_t)(uintptr_t)path);
            strBlk(0x17C40, path);
        }   // a failed 0x143D0 leaves the module path as GetModuleFileNameA wrote it, and retail appends to it
    }
    std::strcat(path, "Crashdump");
    const FogN04Seams saved = g_fogN04;
    g_fogN04.fclose_ = closeF;
    fog_sub_14540(openAppend, path, rec, n);
    g_fogN04 = saved;
    r[4] = 0;
}
