// Fog QServer / D2QSSocket alpha-lane rows, from build-wasm/ghidra-fog/decomp/0x<RVA>.c.
// Register-passed `this` (EAX/EDI/ESI/EBX) is the first parameter. Seams (still UNMAPPED callees) are
// function pointers in FogQsSeams; the +0x61690 CS validate is a debug check and is omitted.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <Windows.h>

namespace {
inline void SetEvent(HANDLE) {}  // single-threaded wasm: no waiter to wake
struct FogQsSeams {
    uint32_t* (*alloc)(int*);                       // +0xB560
    void (*assertFail)(int, const char*, int, const void*);  // +0x10F30
    uint32_t (*tick)();                             // +0xC9E0
    void (*afterInsert)();                          // +0xD110
    void (*onClient)(uint32_t);                     // Ordinal_11
    int (*isLocal)(uint32_t);                       // +0xCEB0
    void (*log)(const void*, const char*);          // +0x690A0
    void (*drop)(uint32_t, const char*, int);       // +0xE910
};
}  // namespace

// Raw-offset view helpers keep the retail layout exact on both 32- and 64-bit builds.
template <class T> static inline T& at(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<char*>(b) + o); }

// 1.13c Fog+0xE460 -- find the client whose +0x10 equals `value`, scanning every bucket under its lock.
// Server: +0x08 bucket count, +0x14 bucket heads, +0x1C critical sections (stride 0x18).
extern "C" void* fog_qs_find_by_value(void* srv, int value) {  // 1.13c Fog+0xE460
    uint32_t n = at<uint32_t>(srv, 8);
    if (!n) return nullptr;
    for (uint32_t i = 0; i < n; ++i) {
        CRITICAL_SECTION* cs = reinterpret_cast<CRITICAL_SECTION*>(at<char*>(srv, 0x1c) + i * 0x18);
        EnterCriticalSection(cs);
        for (char* p = at<char**>(srv, 0x14)[i]; p; p = at<char*>(p, 4))
            if (at<int>(p, 0x10) == value) { LeaveCriticalSection(cs); return p; }
        LeaveCriticalSection(cs);
    }
    return nullptr;
}

// 1.13c Fog+0xE5F0 -- allocate/zero a client (0x664 bytes), link into id and key hash chains.
extern "C" void* fog_qs_add_client(void* srv, const FogQsSeams* s, uint32_t param1, uint32_t key) {  // 1.13c Fog+0xE5F0
    uint32_t* c = s->alloc(nullptr);
    if (!c) s->assertFail(2, "..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x241, "");
    memset(c, 0, 0x199 * 4);
    CRITICAL_SECTION* gl = reinterpret_cast<CRITICAL_SECTION*>(static_cast<char*>(srv) + 0x854);
    EnterCriticalSection(gl);
    uint32_t id = ++at<uint32_t>(srv, 4);
    LeaveCriticalSection(gl);
    c[4] = param1; c[0] = id; c[6] = key; c[9] = 0; c[10] = 0; c[0x1d] = 0; c[0x16] = 1; c[0x1c] = 2;
    uint32_t t = s->tick();
    c[0x26] = c[0x27] = c[0x28] = c[0x29] = t;
    InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(c + 0xb));
    uint32_t mask = at<uint32_t>(srv, 0xc);
    uint32_t b = mask & id;
    CRITICAL_SECTION* bcs = reinterpret_cast<CRITICAL_SECTION*>(at<char*>(srv, 0x1c) + b * 0x18);
    EnterCriticalSection(bcs);
    at<void*>(c, 4) = at<void**>(srv, 0x14)[b];
    at<void**>(srv, 0x14)[b] = c;
    LeaveCriticalSection(bcs);
    CRITICAL_SECTION* kcs = reinterpret_cast<CRITICAL_SECTION*>(static_cast<char*>(srv) + 0x28);
    EnterCriticalSection(kcs);
    uint32_t k = mask & key;
    at<void*>(c, 0x1c) = at<void**>(srv, 0x20)[k];
    at<void**>(srv, 0x20)[k] = c;
    LeaveCriticalSection(kcs);
    EnterCriticalSection(gl);
    s->afterInsert();
    ++*reinterpret_cast<int*>(c + 2);  // single-threaded wasm: plain 32-bit increment (compat long is 64-bit)
    ++at<int>(srv, 0x880);
    LeaveCriticalSection(gl);
    s->onClient(c[6]);
    return c;
}

// 1.13c Fog+0xF250 -- QSBanByAddr: look the client up by key; if found and not local, log and drop it.
extern "C" uint32_t fog_qs_ban_by_addr(void* srv, const FogQsSeams* s, uint32_t* req) {  // 1.13c Fog+0xF250
    uint32_t key = *req;
    CRITICAL_SECTION* kcs = reinterpret_cast<CRITICAL_SECTION*>(static_cast<char*>(srv) + 0x28);
    EnterCriticalSection(kcs);
    char* p = at<char**>(srv, 0x20)[at<uint32_t>(srv, 0xc) & key];
    while (p && at<uint32_t>(p, 0x18) != key) p = at<char*>(p, 0x1c);
    LeaveCriticalSection(kcs);
    if (p && s->isLocal(at<uint32_t>(p, 0x18)) == 0) {
        s->onClient(*req);
        s->log(nullptr, "[HACKLIST]  Client %d at %s banned via QSBanByAddr()");
        s->drop(at<uint32_t>(p, 0), "..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x580);
    }
    return 1;
}

// 1.13c Fog+0x1E550 -- D2QSSocket teardown: if the connection has a handle at +0 and +8, close it under its CS (+0xC).
extern "C" void fog_qssocket_release(void* sock, void (*closeHandle)(uint32_t)) {  // 1.13c Fog+0x1E550
    if (!sock) return;
    char* c = at<char*>(sock, 0x110);
    if (!c || !at<int>(c, 0) || !at<uint32_t>(c, 8)) return;
    CRITICAL_SECTION* cs = reinterpret_cast<CRITICAL_SECTION*>(c + 0xc);
    EnterCriticalSection(cs);
    closeHandle(at<uint32_t>(at<char*>(sock, 0x110), 8));
    at<uint32_t>(at<char*>(sock, 0x110), 8) = 0;
    at<uint32_t>(at<char*>(sock, 0x110), 0) = 0;
    LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(at<char*>(sock, 0x110) + 0xc));
}

// 1.13c Fog+0x1E620 -- split a received stream into frames: u16 length at buf[+0x11c-offset], dispatch each
// to the handler at sock+0x168 (or queue it into sock+0x170 and SetEvent); the tail is copied back.
// Fatal paths (length > 0x2063, bad code ptr) call the abort seam (+0xFC20/Ordinal_116/+0x10F30/+0x684A0/_exit).
extern "C" void fog_qssocket_frames(void* sock, uint8_t* dst, int total, void (*queue)(void*, void*),
                                    void (*fatal)(int)) {  // 1.13c Fog+0x1E620
    uint8_t* src = dst;
    for (;;) {
        int off = at<int>(sock, 0x11c);
        uint32_t len = *reinterpret_cast<uint16_t*>(src + off);
        int remain = static_cast<int>(dst - src) + total;
        if (remain < at<int>(sock, 0x118) || remain < static_cast<int>(len)) {
            memmove(dst, src, remain);  // retail copies forward dword-then-byte; regions may overlap forward-only
            return;
        }
        if (len > 0x2063) fatal(0xfb);
        void* pkt = src + at<int>(sock, 0x118);
        auto handler = at<int (*)(void*)>(sock, 0x168);
        if (!handler) {
            queue(static_cast<char*>(sock) + 0x170, pkt);
            SetEvent(at<HANDLE>(sock, 0x114));
        } else {
            if (handler(at<void*>(sock, 0x16c)) == 0) {
                queue(static_cast<char*>(sock) + 0x170, off < 1 ? pkt : src);
                SetEvent(at<HANDLE>(sock, 0x114));
            } else {
                src += len;
                continue;
            }
        }
        src += len;
    }
}
