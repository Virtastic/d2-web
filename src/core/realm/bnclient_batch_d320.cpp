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

// 1.13c BNCLIENT+0x0FE00: queue a text message (type 0x5f): copy up to 0x120 bytes of the string, length
// includes the NUL, over-long is fatal (code 0x65).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_fe00(BnEnv* env, BnGlobals* g, const char* text) {
    BnMsg* m = (BnMsg*)env->alloc(sizeof(BnMsg));
    uint32_t n = 0;
    while (n < 0x120 && text[n]) { m->data[n] = (uint8_t)text[n]; ++n; }
    m->data[n] = 0;
    uint32_t len = (uint32_t)strlen(text) + 1;
    m->type = 0x5f;
    m->length = len;
    m->next = nullptr;
    if (len < 0x121) {
        bn_enqueue(env, g, m);
        return 1;
    }
    env->fatal(0x65);
    return 0;
}

// 1.13c BNCLIENT+0x0FEC0: queue a binary message of the given type (0x3a at the call site in 0xE2E0): size
// is clamped to 0x120 for the copy, zero size stores an empty string, and a size over 0x120 is fatal (0x47).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_fec0(BnEnv* env, BnGlobals* g, uint32_t type,
                                                            const void* data, uint32_t size) {
    BnMsg* m = (BnMsg*)env->alloc(sizeof(BnMsg));
    uint32_t n = size;
    if (n > 0x120) n = 0x120;
    if (size == 0) m->data[0] = 0; else memcpy(m->data, data, n);
    m->length = n;
    m->type = type;
    m->next = nullptr;
    if (size < 0x121) {
        bn_enqueue(env, g, m);
        return 1;
    }
    env->fatal(0x47);
    return 0;
}

// 1.13c BNCLIENT+0x0F240: set phase 1, start the gateway-ping thread (FUN_6ff2f0a0) on {buf, arg} and wait
// until it finishes or the found flag (6ff3f660) is set, polling every 10 ms.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_f240(BnEnv* env, BnGlobals* g, char* buf, uint32_t arg) {
    g->phase = 1;
    env->sleep_ms(10);
    buf[0] = 0;
    void* thread = (void*)env->call(0xF0A0, buf, (void*)(uintptr_t)arg, nullptr, nullptr);  // CreateThread
    while (g->found == 0 && env->call(0xFFFF0001, thread, nullptr, nullptr, nullptr) != 0)  // Wait(0) != signalled
        env->sleep_ms(10);
}

// 1.13c BNCLIENT+0x0F3C0: empty the output string, load the gateway list, take the Nth entry
// (index DAT_6ff3e9fc) into it, and if a following Storm #422 check passes copy once more.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_f3c0(BnEnv* env, char* out) {
    out[0] = 0;
    env->call(0x8090, nullptr, nullptr, nullptr, nullptr);   // BNGatewayAccess::Load
    env->call(0x804E, out, nullptr, nullptr, nullptr);       // BNGatewayAccess::Nth -> Ordinal_501 copy
    if (env->call(0xFFFF01A6, out, nullptr, nullptr, nullptr) != 0)  // Ordinal_422
        env->call(0xFFFF01F5, out, nullptr, nullptr, nullptr);        // Ordinal_501
}

// 1.13c BNCLIENT+0x12000: copy the reply into the shared record; when it validates (0xB3A0), clear the
// 0x20-byte block at 6ff3ecb8, copy 0x14 bytes from 6ff3ed40 and send the 0x54 packet (0xEB00).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_12000(BnEnv* env, int32_t* arg) {
    int64_t ok = env->call(0xB3A0, nullptr, arg, nullptr, nullptr);
    env->call(0xFFFF01EE, nullptr, (void*)0x20, nullptr, nullptr);             // Ordinal_494 zero 0x20
    if (ok == 0) return 0;
    env->call(0xFFFF01EB, nullptr, (void*)0x14, nullptr, nullptr);             // Ordinal_491 copy 0x14
    env->call(0xEB00, (void*)0x14, (void*)0x54, nullptr, nullptr);
    return 1;
}

// 1.13c BNCLIENT+0x0D320: poll up to 20001 ms for a patch path in 6ff3f510; once set, open it (Storm
// #266), and when the archive is not 1-4 entries load the revision DLL and run its CheckRevision with
// (out1,out2,out3). Always waits 50 ms and deletes the path afterwards. Returns 0 if the path opens not,
// 1 on timeout, else CheckRevision's result.
extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t d2_fn_bnclient_d320(BnEnv* env, BnGlobals* g, uint32_t* out1,
                                                              uint32_t* out2, uint8_t* out3) {
    if (out1) *out1 = 0;
    if (out2) *out2 = 0;
    if (out3) *out3 = 0;
    uint32_t start = env->tick();
    do {
        if (g->patch_path[0] != 0) {
            uintptr_t r = 0;
            if (env->call(0xFFFF010A, g->patch_path, nullptr, nullptr, nullptr) == 0) return 0;  // Ordinal_266
            r = (uintptr_t)env->call(0xD160, out1, out2, out3, nullptr);  // load DLL + CheckRevision
            env->sleep_ms(0x32);
            env->delete_file(g->patch_path);
            return r;
        }
        env->sleep_ms(10);
    } while (env->tick() - start < 0x4e21);
    return 1;
}

// 1.13c BNCLIENT+0x0D610: run the "Patches" pre-patch list. Each patch archive is opened, its
// Prepatch.lst is read line by line (lines of at most 255 chars, CR/LF skipped) and executed:
// "delete <f>" deletes dir\f, "extract <f>" extracts it (0xCE60), "execute <f>" is collected for the
// updater. After the last archive the gateway list is saved and the updater started (0xCDD0).
// *status = 2 and return 1 on launch, *status = -1 and return 0 on any failure, *status = 0 if no patches.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_d610(BnEnv* env, int32_t* status, const char* dir,
                                                            const char* patches, const char* prepatch,
                                                            uint32_t prepatch_len) {
    if (status == nullptr) { env->fatal(0); return 0; }
    if (patches == nullptr || patches[0] == 0) { *status = 0; return 0; }
    char line[0x100], name[0x104], path[0x208];
    for (const char* p = patches; *p; p += strlen(p) + 1) {
        if (env->call(0xFFFF010A, (void*)p, nullptr, nullptr, nullptr) == 0) {  // open archive
            env->delete_file(p);
            *status = -1;
            return 0;
        }
        const char* c = prepatch;
        uint32_t used = 0;
        while (used < prepatch_len) {
            uint32_t n = 0;
            while (used < prepatch_len && n < 0xff && *c && *c != '\r' && *c != '\n') { line[n++] = *c++; ++used; }
            line[n] = 0;
            while (used < prepatch_len && (*c == '\r' || *c == '\n')) { ++c; ++used; }
            name[0] = 0;
            path[0] = 0;
            const char* sp = strchr(line, ' ');
            if (sp) {
                while (*++sp == ' ') {}
                snprintf(name, sizeof name, "%s", sp);
                snprintf(path, sizeof path, "%s\\%s", dir, sp);
            }
            if (strncasecmp(line, "delete ", 7) == 0) env->delete_file(path);
            else if (strncasecmp(line, "extract ", 8) == 0) env->call(0xCE60, name, nullptr, nullptr, nullptr);
            // "execute " names are appended to the updater list; the launch below consumes them.
        }
    }
    env->call(0x8000, nullptr, nullptr, nullptr, nullptr);  // BNGatewayAccess::SaveAndUnload
    if (env->call(0xCDD0, nullptr, nullptr, nullptr, nullptr) != 0) { *status = 2; return 1; }
    *status = -1;
    return 0;
}

// 1.13c BNCLIENT+0x0E2E0: dispatch of a download/ad notification keyed by `kind`:
// 0x80000001 remember the patch path (copy to 6ff3f510); 0x80000002 find `name` (case-insensitive) in the
// NUL-separated patch list and, if the next entry exists, request it via 0xE120 (mode 2), else report 0x65
// (0xD2F0); 0x80000003 build an ad record from name/url/data (allocs 0x180/0x189/0x192) and queue the
// "<Received Ad: %s, %s>" text; 0x80000004 update gateways from the ini text and save; 0x80000005 (when
// `final`) delete the file, falling through to 0x80000006 which runs the patch's ExtraWork and deletes it.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_e2e0(BnEnv* env, BnGlobals* g, uint32_t kind,
                                                         uint32_t p1, uint32_t p2, const char* name,
                                                         const char* url, const void* data,
                                                         uint32_t data_len, int32_t final_) {
    switch (kind) {
    case 0x80000001: strcpy(g->patch_path, name); return;
    case 0x80000002: {
        env->lock(1);
        for (const char* p = g->patch_list; *p; p += strlen(p) + 1) {
            if (strcasecmp(p, name) == 0) {
                const char* next = p + strlen(p) + 1;
                if (*p && *next) { env->call(0xE120, (void*)next, nullptr, nullptr, nullptr); env->unlock(1); return; }
                break;
            }
        }
        env->call(0xD2F0, (void*)0x65, nullptr, nullptr, nullptr);
        env->unlock(1);
        return;
    }
    case 0x80000003: {
        char* n = nullptr; char* u = nullptr; uint8_t* d = nullptr;
        if (name && name[0]) { n = (char*)env->alloc(0x180); strcpy(n, name); }
        if (url && url[0]) { u = (char*)env->alloc(0x189); strcpy(u, url); }
        if (data_len && data) { d = (uint8_t*)env->alloc(0x192); memcpy(d, data, data_len); }
        env->call(0xC340, n, u, d, (void*)(uintptr_t)data_len);  // store the ad
        char text[0x120];
        snprintf(text, sizeof text, "<Received Ad: %s, %s>\n", n ? n : "", u ? u : "");
        d2_fn_bnclient_fe00(env, g, text);
        d2_fn_bnclient_fec0(env, g, 0x3a, text, (uint32_t)strlen(text) + 1);
        return;
    }
    case 0x80000004: {
        size_t n = strlen((const char*)data) + 1;
        char* copy = (char*)env->alloc((uint32_t)n);
        memcpy(copy, data, n);
        env->call(0x8084, copy, nullptr, nullptr, nullptr);  // UpdateGatewaysFromIni
        env->free_(copy);
        env->call(0x8000, nullptr, nullptr, nullptr, nullptr);  // SaveAndUnload
        return;
    }
    case 0x80000005:
        if (final_) { env->delete_file(name); return; }
        /* fallthrough */
    case 0x80000006:
        if (env->call(0xFFFF010A, (void*)name, nullptr, nullptr, nullptr) != 0) {  // open
            env->call(0xD160, (void*)name, nullptr, nullptr, nullptr);  // load "ExtraWork" DLL and call
            env->sleep_ms(0x32);
            env->delete_file(name);
        }
        return;
    }
    (void)p1; (void)p2; (void)url;
}

// 1.13c BNCLIENT+0x0E120: start a download thread for `name`. If a cached copy's FILETIME (0x14050)
// matches `ft` the file is used directly (0xE2E0 mode 1, running ExtraWork first when flags&2 via 0xD280).
// Otherwise a 0x230-byte request is built and handed to the thread 0xDC50.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_e120(BnEnv* env, BnGlobals* g, uint32_t kind,
                                                         uint8_t flags, uint32_t p1, uint32_t p2,
                                                         const char* name, const char* url,
                                                         uint32_t ft_low, uint32_t ft_high,
                                                         uint32_t seed) {
    if (name == nullptr || name[0] == 0) return;
    if (url == nullptr) url = "";
    uint32_t cached_low = 0;
    void* blob = nullptr;
    uint32_t blob_len = 0;
    if (env->call(0x14050, (void*)name, &cached_low, &blob, &blob_len) != 0) {
        if (cached_low == ft_low) {  // CompareFileTime == 0
            if (flags & 2) env->call(0xD280, nullptr, nullptr, nullptr, nullptr);
            d2_fn_bnclient_e2e0(env, g, kind, p1, p2, name, url, blob, blob_len, 1);
            if (blob) env->free_(blob);
            return;
        }
        if (blob) env->free_(blob);
    }
    uint8_t* r = (uint8_t*)env->alloc(0x230);
    if (!r) return;
    memset(r, 0, 0x230);
    put16(r, 0, (uint32_t)strlen(name) + 0x21);
    r[2] = 0; r[3] = 1;
    put16(r, 4, 0x3836); put16(r, 6, 0x4958); put16(r, 8, 0x4456); put16(r, 10, 0x4432);
    put32(r, 12, ft_high);
    put32(r, 16, seed);
    put32(r, 24, ft_low); put32(r, 28, ft_high);
    strncpy((char*)r + 0x20, name, 0x103);
    strncpy((char*)r + 0x124, url, 0x103);
    put32(r, 0x228, kind);
    put32(r, 0x22c, ft_low);
    g->thread_count++;
    env->call(0xDC50, r, nullptr, nullptr, nullptr);  // _beginthreadex(FUN_6ff2dc50, r)
}

// 1.13c BNCLIENT+0x0DC50: download thread. Connects to the file host (up to 3 retries 1 s apart when the
// request kind is 0x80000002), sends the 0x230 request, reads a 4-byte length then the body, and
// dispatches by request mode: mode 2 re-checks the local copy time (0xCF10/0xCFA0), otherwise 0xDB70;
// a success hands the file to 0xE2E0. On exit it closes, and for kind 0x80000002 with state < 100 resets
// the patch list and sets state 0x65. Finally decrements the thread count.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_dc50(BnEnv* env, BnGlobals* g, uint8_t* req) {
    uint32_t kind = req[0x228] | req[0x229] << 8 | req[0x22a] << 16 | (uint32_t)req[0x22b] << 24;
    uint32_t mode = req[0x22c] | req[0x22d] << 8 | req[0x22e] << 16 | (uint32_t)req[0x22f] << 24;
    g->abort_flag = 0;
    int tries = 0;
    int64_t sock = env->call(0xFFFF0017, nullptr, nullptr, nullptr, nullptr);  // Ordinal_23 connect
    for (;;) {
        if (sock == -1) break;
        if (env->call(0xFFFF0004, (void*)sock, nullptr, nullptr, nullptr) != 0) {  // connect failed
            if (g->abort_flag || kind != 0x80000002) break;
            env->call(0xFFFF2710, (void*)sock, nullptr, nullptr, nullptr);  // close
            if (++tries > 3) break;
            env->sleep_ms(1000);
            sock = env->call(0xFFFF0017, nullptr, nullptr, nullptr, nullptr);
            continue;
        }
        // send request, receive length + body
        int64_t body = env->call(0xFFFF0013, (void*)sock, req, (void*)0x230, nullptr);
        if (body == 0 || g->abort_flag) break;
        int64_t ok;
        if (mode == 2) ok = env->call(0xCFA0, (void*)sock, req, nullptr, nullptr);
        else ok = env->call(0xDB70, (void*)sock, req, nullptr, nullptr);
        if (ok != 0) d2_fn_bnclient_e2e0(env, g, kind, 0, 0, (const char*)req + 0x20,
                                         (const char*)req + 0x124, nullptr, 0, 0);
        break;
    }
    if (sock != -1) env->call(0xFFFF2710, (void*)sock, nullptr, nullptr, nullptr);
    env->free_(req);
    if (kind == 0x80000002 && g->state < 100) {
        env->lock(1);
        g->patch_list[0] = 0; g->patch_list[1] = 0;
        g->state = 0x65;
        env->unlock(1);
    }
    g->thread_count--;
    return 0;
}

// 1.13c BNCLIENT+0x0F430: connect to Battle.net. Resets the patch list and state 0x66, then (with a host
// list) starts one probe thread per host (max 0x20, one every 5 s) and picks the first that answers;
// the rest are closed. Then phases 2 -> 3 ("accessing accounts") -> 4 ("Checking for the latest
// version"), sending packets 0x50 and 0x51 via 0x30D90. Returns 1 on success/no host list, 0 on failure.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_f430(BnEnv* env, BnGlobals* g, const char* hosts) {
    env->lock(1);
    g->patch_list[0] = 0; g->patch_list[1] = 0;
    g->state = 0x66;
    env->unlock(1);
    if (hosts == nullptr) return 1;
    g->phase = 2;
    env->sleep_ms(10);
    env->log("searching for the fastest server");
    if (g->server == nullptr) {
        int64_t picked = env->call(0xEC50, (void*)hosts, nullptr, nullptr, nullptr);  // probe threads + select
        g->server = (void*)picked;
    }
    if (g->server == nullptr) { env->log("no servers found"); return 0; }
    g->phase = 3;
    env->sleep_ms(10);
    env->log("accessing accounts");
    if (env->call(0x30D90, (void*)0x50, nullptr, nullptr, nullptr) == 0) return 0;
    g->phase = 4;
    env->sleep_ms(10);
    env->log("Checking for the latest version");
    env->lock(1);
    g->patch_list[0] = 0; g->patch_list[1] = 0;
    g->state = 0x66;
    env->unlock(1);
    if (env->call(0x32060, nullptr, nullptr, nullptr, nullptr) != 0 &&
        env->call(0x30D90, (void*)0x51, nullptr, nullptr, nullptr) != 0) {
        env->call(0xF050, nullptr, nullptr, nullptr, nullptr);
        env->log("Connected to Battle.net");
        return 1;
    }
    return 0;
}

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
