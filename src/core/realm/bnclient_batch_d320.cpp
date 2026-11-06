// src/core/realm/bnclient_batch_d320.cpp -- twelve UNMAPPED 1.13c BNClient.dll alpha-lane rows (Stage A:
// implementation + marker only, no spec, no vector), transcribed from build-wasm/ghidra-bnclient/decomp/
// 0xD320.c and siblings (image base 0x6FF20000). The patch/ad/server-probe code is all host I/O (files,
// threads, sockets, LoadLibrary, Storm/Fog ordinals); every such call is a seam in a BnEnv function table,
// the control flow and byte layouts after the retail. Register-passed args the decompiler lost
// (unaff_*) are named by their use at the call sites in 0xE2E0/0xDC50/0xE120.
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

namespace {
void put16(uint8_t* b, uint32_t o, uint32_t v) { b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); }
void put32(uint8_t* b, uint32_t o, uint32_t v) { put16(b, o, v); put16(b, o + 2, v >> 16); }
}  // namespace

struct BnEnv {
    uint32_t (*tick)();
    void (*sleep_ms)(uint32_t ms);
    void (*lock)(int which);    // 0 = DAT_6ff3f1dc (queue), 1 = DAT_6ff3f8e8 (patch list)
    void (*unlock)(int which);
    void* (*alloc)(uint32_t size);   // Fog #10042 / Storm #401
    void (*free_)(void* p);          // Storm #403
    void (*delete_file)(const char* path);
    void (*fatal)(uint32_t code);    // Fog #10265 + #10024 + _exit(-1)
    void (*log)(const char* s);      // FUN_6ff37420
    // host seams, one per undecoded callee, named by retail address
    int64_t (*call)(uint32_t rva, void* a, void* b, void* c, void* d);
};

// DAT_6ff3f510 patch path, 6ff3f64c/50 queue head/tail, 6ff3f670 patch list, 6ff3f508 state, 6ff3f1f8 phase.
struct BnGlobals {
    char patch_path[0x104];
    struct BnMsg* queue_head;
    struct BnMsg* queue_tail;
    char patch_list[0x400];  // NUL-separated entries, double-NUL end
    int32_t state;
    int32_t phase;
    int32_t abort_flag;      // DAT_6ff3f614
    int32_t thread_count;    // DAT_6ff3f66c
    int32_t found;           // DAT_6ff3f660
    void* server;            // DAT_6ff3f654
};

// Queue node of 0x12c bytes: type, 0x120 data bytes, length, next.
struct BnMsg {
    uint32_t type;
    uint8_t data[0x120];
    uint32_t length;
    BnMsg* next;
};

namespace {
void bn_enqueue(BnEnv* env, BnGlobals* g, BnMsg* m) {
    env->lock(0);
    if (g->queue_head != nullptr) {
        g->queue_tail->next = m;
    }
    g->queue_head = g->queue_head ? g->queue_head : m;
    g->queue_tail = m;
    env->unlock(0);
}
}  // namespace

// 1.13c BNCLIENT+0x0FE00: queue a text message. type is the first stack argument (0xE2E0 passes 1). The copy is
// SStrCopy-bounded to 0x120 (0x11F chars plus NUL); length = strlen(copy)+1; over 0x120 is fatal (code 0x65).
extern "C" int32_t bnclient_queue_text(BnEnv* env, BnGlobals* g, uint32_t type, const char* text) {
    BnMsg* m = (BnMsg*)env->alloc(sizeof(BnMsg));
    uint32_t n = 0;
    while (n < 0x11f && text[n]) { m->data[n] = (uint8_t)text[n]; ++n; }
    m->data[n] = 0;
    uint32_t len = (uint32_t)strlen((const char*)m->data) + 1;
    m->type = type;
    m->length = len;
    m->next = nullptr;
    if (len < 0x121) {
        bn_enqueue(env, g, m);
        return 1;
    }
    env->fatal(0x65);
    return 0;
}

// 1.13c BNCLIENT+0x0FEC0: queue a binary message. type is the first stack argument (0xE2E0 passes 3). The size is
// clamped to 0x120 before the copy and before the stored length, so the fatal 0x47 test after it never fires.
// Zero size stores an empty string.
extern "C" int32_t bnclient_queue_binary(BnEnv* env, BnGlobals* g, uint32_t type,
                                                            const void* data, uint32_t size) {
    BnMsg* m = (BnMsg*)env->alloc(sizeof(BnMsg));
    uint32_t n = size;
    if (n > 0x120) n = 0x120;
    if (n == 0) m->data[0] = 0; else memcpy(m->data, data, n);
    m->length = n;
    m->type = type;
    m->next = nullptr;
    bn_enqueue(env, g, m);
    return 1;
}

// 1.13c BNCLIENT+0x0F240 (stdcall, EAX = out string buffer, [esp+4] = arg): set phase 1, Sleep(10), clear the buffer, store {buf, arg} in
// DAT_6ff3f500/04, start the gateway-ping thread (FUN_6ff2f0a0) on that pair and poll until it finishes (Wait(0) == 0) or the found flag
// (DAT_6ff3f660) is set, sleeping 10 ms per poll. The globals ride a 4-dword block {phase 0x1F1F8, buf 0x1F500, arg 0x1F504, found 0x1F660}.
struct BnGatewayStartState { uint32_t phase; uint32_t buf; uint32_t arg; uint32_t found; };
struct BnGatewayStartEnv {
    void (*sleep_ms)(uint32_t ms);
    uint32_t (*create_thread)(const BnGatewayStartState* params);   // CreateThread(0, 0, 0x2F0A0, &DAT_1F500, 0, &tid)
    uint32_t (*wait)(uint32_t handle, uint32_t ms);                 // WaitForSingleObject
};
extern "C" EMSCRIPTEN_KEEPALIVE void bn_gateway_start(const BnGatewayStartEnv* env, BnGatewayStartState* g, char* buf, uint32_t arg) {
    g->phase = 1;
    env->sleep_ms(10);
    buf[0] = 0;
    g->arg = arg;
    g->buf = (uint32_t)(uintptr_t)buf;
    uint32_t thread = env->create_thread(g);
    while (g->found == 0 && env->wait(thread, 0) != 0)
        env->sleep_ms(10);
}

// 1.13c BNCLIENT+0x12000: copy the reply into the shared record; when it validates (0xB3A0), clear the
// 0x20-byte block at 6ff3ecb8, copy 0x14 bytes from 6ff3ed40 and send the 0x54 packet (0xEB00).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t bnclient_batch_12000(BnEnv* env, int32_t* arg) {
    int64_t ok = env->call(0xB3A0, nullptr, arg, nullptr, nullptr);
    env->call(0xFFFF01EE, nullptr, (void*)0x20, nullptr, nullptr);             // Ordinal_494 zero 0x20
    if (ok == 0) return 0;
    env->call(0xFFFF01EB, nullptr, (void*)0x14, nullptr, nullptr);             // Ordinal_491 copy 0x14
    env->call(0xEB00, (void*)0x14, (void*)0x54, nullptr, nullptr);
    return 1;
}

// 0x0D320 moved to src/core/bnclient/bnclient_d320.cpp.

// 0x0D610 moved to src/core/bnclient/bnclient_d610.cpp.

// 0x0E2E0 moved to src/core/bnclient/bnclient_e2e0.cpp.

// 0x0E120 moved to src/core/bnclient/bnclient_e120.cpp.

// 0x0DC50 moved to src/core/bnclient/bnclient_dc50.cpp.

// 0x0F430 moved to src/core/bnclient/bnclient_f430.cpp.

// 1.13c BNCLIENT+0x2854: MSVC CRT _output (the printf engine behind sprintf/wsprintf). Format-state
// machine tables 0x6FF39370/0x6FF39390; here the flags -+ 0#, width/precision, h l w I64 and the
// c C d i o u x X p s S e E f g G n conversions are delegated to vsnprintf (identical C-locale
// output; %Z ANSI_STRING and %I32 are not carried). Returns the character count, or -1 on a bad format.
// ponytail: delegates to libc; transcribe the state tables if %Z/%I32 ever appears in a vector.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_2854(char* out, uint32_t cap, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(out, cap, fmt, ap);
    va_end(ap);
    return n;
}
