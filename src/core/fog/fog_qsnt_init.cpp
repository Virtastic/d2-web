// src/core/fog/fog_qsnt_init.cpp -- 1.13c Fog.dll QServerNT socket init, from build-wasm/ghidra-fog/decomp.
// Winsock/Win32/thread calls are seams on FogQsEnv; `self` (EAX in retail) is a guest address in `mem`.
#include <cstdint>
#include <cstring>

struct FogQsEnv {
    uint8_t* mem;
    int (*iocp_create)(void);                                    // CreateIoCompletionPort(-1,0,0,0) != 0
    void (*fatal_exit)(int line);                                // FC20, ord116, FogError(6,..), 184A0, _exit(-1)
    int (*ws_socket)(void);                                      // ord23
    int (*ws_setsockopt)(int s, int lvl, int opt, const void* v, int len);  // ord21
    int (*ws_ioctl)(int s, uint32_t cmd, uint32_t* arg);         // ord12
    uint16_t (*ws_htons)(uint16_t p);                            // ord9
    int (*ws_bind)(int s, const void* sa, int len);              // ord2
    int (*ws_listen)(int s, int backlog);                        // ord13
    int (*ws_lasterr)(void);                                     // ord111
    void (*ws_close)(int s);                                     // ord3
    void (*log)(const char* fmt, int arg);                       // Fog+0x19100
    uint32_t (*alloc8)(const char* file, int line);              // Fog+0x1CD10
    uint32_t (*thread)(uint32_t proc, uint32_t arg, uint32_t* tid);  // CreateThread, returns handle
};

static uint32_t L(const FogQsEnv* e, uint32_t a) { uint32_t v; memcpy(&v, e->mem + a, 4); return v; }
static void S(const FogQsEnv* e, uint32_t a, uint32_t v) { memcpy(e->mem + a, &v, 4); }

// 1.13c Fog+0x21050
extern "C" int fog_sub_21050(const FogQsEnv* e, uint32_t self, uint16_t port) {
    if (!e->iocp_create()) e->fatal_exit(0x4c);
    const int s = e->ws_socket();
    if (s == -1) { e->log("[SERVER]  QSNTInit failed on socket() - err=%d", e->ws_lasterr()); return 0; }
    int one = 1;
    e->ws_setsockopt(s, 6, 1, &one, 4);          // IPPROTO_TCP, TCP_NODELAY
    e->ws_setsockopt(s, 0xffff, 8, &one, 4);     // SOL_SOCKET, SO_KEEPALIVE
    e->ws_setsockopt(s, 0xffff, 4, &one, 4);     // SO_REUSEADDR
    uint32_t nb = 1;
    e->ws_ioctl(s, 0x8004667e, &nb);             // FIONBIO
    uint8_t sa[16] = {2, 0};                     // AF_INET, INADDR_ANY
    const uint16_t p = e->ws_htons(port);
    memcpy(sa + 2, &p, 2);
    const char* msg;
    if (e->ws_bind(s, sa, 16) == -1) {
        e->log("[SERVER]  QSNTInit failed on bind() - err=%d", e->ws_lasterr());
        e->ws_close(s);
        return 0;
    }
    if (e->ws_listen(s, 0xfa) == -1) {
        e->log("[SERVER]  QSNTInit failed on listen() - err=%d", e->ws_lasterr());
        e->ws_close(s);
        return 0;
    }
    (void)msg;
    const uint32_t i = L(e, self + 0xb8c);
    S(e, self + 0xb8c, i + 1);
    S(e, self + 0xc08 + i * 4, 0x8004667e);      // retail stores the leftover uVar7
    memcpy(e->mem + self + 0xba0 + i * 2, &p, 2);
    S(e, self + 0xb90 + i * 4, (uint32_t)s);
    S(e, self + 0xba8 + i * 4, 1);
    uint32_t a = e->alloc8("..\\Source\\Fog\\Src\\QServer\\QServerNT.cpp", 0x9b);
    S(e, a, self); S(e, a + 4, i);
    uint32_t tid = 0;
    S(e, self + 0xbd8 + i * 4, e->thread(0x6ff70c00, a, &tid));
    S(e, self + 0xbe8 + i * 4, tid);
    if (L(e, self + 0xc18) != 0) {
        a = e->alloc8("..\\Source\\Fog\\Src\\QServer\\QServerNT.cpp", 0xae);
        S(e, a, self); S(e, a + 4, i);
        S(e, self + 0xc1c + i * 4, e->thread(0x6ff70b40, a, &tid));
        S(e, self + 0xc2c + i * 4, tid);
    }
    return s;
}
