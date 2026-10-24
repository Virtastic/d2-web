// src/core/fog/fog_batch_b64.cpp -- 1.13c Fog.dll alpha-lane rows, from build-wasm/ghidra-fog/decomp.
// Retail structs are 32-bit; guest addresses (uint32_t) index a flat image `mem` (base 0, so the
// retail address 0x6FF9AF78 is mem[0x6FF9AF78]). Win32 critical sections, VirtualFree, free and the
// module-internal callees are seams on FogB64Env. FUN_6ff61690 (lock tracker) is env->lock_note.
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct FogB64Env {
    uint8_t* mem;
    void (*enter)(uint32_t cs);
    void (*leave)(uint32_t cs);
    void (*init_cs)(uint32_t cs);
    void (*delete_cs)(uint32_t cs);
    void (*lock_note)(uint32_t cs);                      // Fog+0x11690
    void (*vfree)(uint32_t block);                       // VirtualFree(p,0,MEM_RELEASE)
    void (*free_mem)(uint32_t p);                        // CRT free
    void (*strcopy)(uint32_t dst, const char* src, uint32_t n);   // Storm #501
    void (*pool_node_unlink)(uint32_t node);             // Fog+0x1BB00
    void (*free_ptr)(uint32_t* pp);                      // Fog+0x1B0C0
    void (*raise_pool_error)(const char* msg);           // Fog+0x1AD20 + RaiseException(0xE0000001)
    void (*fatal)(int lvl, const char* file, int line, const char* fmt, ...);  // Fog+0x10F30
    uint32_t (*capture_ip)(void);                        // Fog+0xFC20
    void (*ordinal116)(void);
    void (*abort_fn)(void);                              // Fog+0x184A0
    void (*exit_fn)(int);                                // _exit
    void (*f71050)(void);
    void (*f70180)(uint32_t self, uint32_t* a, uint32_t b);
    int (*ord19)(uint32_t sock, uint32_t* a, uint32_t b, int c);
    void (*ord111)(void);
    void (*ord3)(int h);
    void (*ord5)(uint32_t sock, void* buf, uint32_t* len);
    void* (*ord11)(uint32_t a, uint32_t b, uint32_t c);  // IP string
    void (*f5e9a0)(void);
    void (*f5d5e0)(void);
    void (*f5d710)(void);
    void (*f5e550)(void);
    void (*f5df00)(uint32_t a, uint32_t b, uint32_t c);
    void (*log)(uint32_t logaddr, const char* fmt);      // Fog+0x190A0 (varargs lost)
};

static uint32_t L(const FogB64Env* e, uint32_t a) { uint32_t v; memcpy(&v, e->mem + a, 4); return v; }
static void S(const FogB64Env* e, uint32_t a, uint32_t v) { memcpy(e->mem + a, &v, 4); }
static void Z(const FogB64Env* e, uint32_t a, uint32_t dwords) { memset(e->mem + a, 0, (size_t)dwords * 4); }
static void lock(const FogB64Env* e, uint32_t cs) { e->lock_note(cs); e->enter(cs); }

static void fatal_exit(const FogB64Env* e, int line) {
    const uint32_t ip = e->capture_ip();
    e->ordinal116();
    e->fatal(6, "", line, "Unrecoverable internal error %08x", ip);
    e->abort_fn();
    e->exit_fn(-1);
}

static const uint32_t POOL = 0x6ff9af94, POOL_FLAG = 0x6ff9af78, POOL_CS = 0x6ff9af7c;
static const uint32_t POOL_USED = 0x6ffa6df4, POOL_FREELIST = 0x6ffa6df8, POOL_NFREE = 0x6ffa6e18;

// (superseded by fog_d128a.cpp, was Fog 1AE90)
// Create a pool system. out (guest addr, may be 0) receives the pool address; name copied (32 max).
extern "C" uint32_t fog_sub_1ae90(const FogB64Env* e, uint32_t out, const char* name, int32_t size) {
    if (out == 0) {
        Z(e, POOL_FLAG, 0x2fa9);
        e->init_cs(POOL_CS);
        S(e, POOL_FLAG, 1);
    }
    e->enter(POOL_CS);
    int32_t idx;
    if (L(e, POOL_NFREE) == 0) {
        if (7 < (int32_t)L(e, POOL_USED)) {
            e->leave(POOL_CS);
            e->raise_pool_error("Insufficient pre-allocated pool systems");
            return 0;
        }
        idx = (int32_t)L(e, POOL_USED);
        S(e, POOL_USED, idx + 1);
    } else {
        S(e, POOL_NFREE, L(e, POOL_NFREE) - 1);
        idx = (int32_t)L(e, POOL_FREELIST + L(e, POOL_NFREE) * 4);
    }
    const uint32_t P = POOL + (uint32_t)idx * 0x17cc;
    e->leave(POOL_CS);
    Z(e, P, 0x5f3);
    if (out != 0) S(e, out, P);
    e->init_cs(P + 4);
    e->lock_note(P + 4);
    e->enter(P + 4);
    if (name != nullptr) {
        e->strcopy(0x6ff9c740 + (uint32_t)idx * 0x17cc, name, 0x20);
        e->mem[0x6ff9c75f + (uint32_t)idx * 0x17cc] = 0;
    }
    int32_t n;
    if (size < 0x11) n = 0;
    else {
        uint32_t u = (uint32_t)size - 1;
        n = 32 - __builtin_clz(u);      // bit-scan-reverse + 1
        n -= 4;
    }
    n += 1;
    if (0x28 < n) {
        fatal_exit(e, 0x86c);
        return 0;
    }
    S(e, P + 0x1c, (uint32_t)n);
    S(e, P + 0x7a8, 0);
    for (int32_t i = 0; i < n; i++) {
        const uint32_t c = P + 32 + (uint32_t)i * 48;
        e->init_cs(c);
        const int32_t sz = i == 0 ? 0x10 : (int32_t)(1u << ((i + 4) & 0x1f));
        S(e, c + 24, (uint32_t)sz);
        int32_t cnt = 0x100;
        if (!(sz < 0xf4241 && sz * 0x100 < 0x40000) || sz * 0x100 < 0x10000) {
            const uint32_t num = (sz < 0xf4241 && sz * 0x100 < 0x40000) ? 0x10000u : 0x80000u;
            cnt = (int32_t)(num / (uint32_t)sz);
        }
        S(e, c + 28, (uint32_t)cnt);
        S(e, c + 32, (uint32_t)(sz * cnt));
    }
    Z(e, P + 0x7ac, 0x3ff);
    e->lock_note(P + 4);
    e->leave(P + 4);
    return 0;
}

// 1.13c Fog+0x1B830
// Destroy a pool system (0 = the default one): free every block, bucket chain, delete the locks,
// zero the struct, return its index to the free list.
extern "C" void fog_sub_1b830(const FogB64Env* e, uint32_t P) {
    if (L(e, POOL_FLAG) == 0) return;
    if (P == 0) P = POOL;
    if (L(e, P + 0x1c) == 0) return;
    lock(e, P + 4);
    if (L(e, P + 0x1c) == 0) {
        e->lock_note(P + 4);
        e->leave(P + 4);
    } else {
        for (int32_t i = 0; i < (int32_t)L(e, P + 0x1c); i++) {
            const uint32_t c = P + 32 + (uint32_t)i * 48;
            lock(e, c);
            uint32_t n = L(e, c + 40);
            while (n != 0) {
                const uint32_t next = L(e, n + 16);
                e->vfree(L(e, n));
                e->free_mem(n);
                n = next;
            }
            e->delete_cs(c);
        }
        e->free_mem(L(e, P + 0x7a8));
        uint32_t slot = P + 0x1eb * 4;
        for (int k = 0x3ff; k != 0; k--, slot += 4) {
            uint32_t n = L(e, slot);
            while (n != 0) { const uint32_t nx = L(e, n); e->free_mem(n); n = nx; }
        }
        e->delete_cs(P + 4);
    }
    Z(e, P, 0x5f3);
    lock(e, POOL_CS);
    S(e, POOL_FREELIST + L(e, POOL_NFREE) * 4, (P - POOL) / 0x17cc);
    S(e, POOL_NFREE, L(e, POOL_NFREE) + 1);
    e->lock_note(POOL_CS);
    e->leave(POOL_CS);
    if (P == POOL) {
        lock(e, POOL_CS);
        S(e, POOL_FLAG, 0);
        e->delete_cs(POOL_CS);
    }
}

// 1.13c Fog+0x1BCB0
// Release every empty (used == 0) block of every size class of a pool system.
extern "C" void fog_sub_1bcb0(const FogB64Env* e, uint32_t P) {
    if (P == 0) P = POOL;
    if (L(e, P + 0x1c) == 0) return;
    lock(e, P + 4);
    for (int32_t i = 0; i < (int32_t)L(e, P + 0x1c); i++) {
        const uint32_t c = P + 32 + (uint32_t)i * 48;
        lock(e, c);
        uint32_t next = L(e, c + 40);
        while (next != 0) {
            const uint32_t n = next;
            next = L(e, n + 16);
            if (L(e, n + 8) == 0) {
                if (L(e, n + 12) != 0) S(e, L(e, n + 12) + 0x10, next);
                if (L(e, n + 16) != 0) S(e, L(e, n + 16) + 0xc, L(e, n + 12));
                if (L(e, c + 40) == n) S(e, c + 40, L(e, n + 16));
                if (L(e, c + 44) == n) S(e, c + 44, L(e, n + 12));
                S(e, c + 36, L(e, c + 36) - 1);
                e->pool_node_unlink(n);
                e->vfree(L(e, n));
                e->free_mem(n);
            }
        }
        e->lock_note(c);
        e->leave(c);
    }
    e->lock_note(P + 4);
    e->leave(P + 4);
}

// 1.13c Fog+0xF6E0
// self is the guest/host int the retail code dereferences; *self == 0 is the fatal path.
extern "C" void fog_sub_f6e0(const FogB64Env* e, const int32_t* self) {
    if (*self != 0) { e->f71050(); return; }
    fatal_exit(e, 0x129);
}

// 1.13c Fog+0xF2F0
// For every hash bucket (self[2] of them) walk the chain; entries with +0xC == 0 get the buffer
// sent: via Ordinal_19 when self[0] == 0 (Ordinal_111 on -1), else Fog+0x70180.
extern "C" uint32_t fog_sub_f2f0(const FogB64Env* e, uint32_t self, uint32_t* buf, uint32_t len) {
    const uint32_t nb = L(e, self + 8);
    if (nb == 0) return 1;
    for (uint32_t b = 0; b < nb; b++) {
        const uint32_t cs = L(e, self + 28) + b * 24;
        lock(e, cs);
        for (uint32_t n = L(e, L(e, self + 20) + b * 4); n != 0; n = L(e, n + 4)) {
            if (L(e, n + 0xc) != 0) continue;
            if (L(e, self) == 0) {
                if (e->ord19(L(e, n + 0x10), buf, len, 0) == -1) e->ord111();
            } else e->f70180(self, buf, len);
        }
        e->leave(L(e, self + 28) + b * 24);
    }
    return 1;
}

// 1.13c Fog+0xE7E0
// Free every per-bucket chain of queued nodes (next link at +0x5BC), emptying both bucket tables.
extern "C" uint32_t fog_sub_e7e0(const FogB64Env* e, uint32_t self) {
    for (int32_t i = 0; i < (int32_t)L(e, self + 0x87c); i++) {
        const uint32_t cs = self + 0x854;
        lock(e, cs);
        uint32_t node = L(e, L(e, self + 0x870) + i * 4);
        S(e, L(e, self + 0x870) + i * 4, 0);
        S(e, L(e, self + 0x874) + i * 4, 0);
        e->leave(cs);
        while (node != 0) {
            const uint32_t next = L(e, node + 0x5bc);
            e->free_ptr(&node);
            node = next;
        }
    }
    return 1;
}

// 1.13c Fog+0xF3C0
// Destroy a server/session object: flush (when buf given), free queues, locks and tables, free self.
extern "C" uint32_t fog_sub_f3c0(const FogB64Env* e, uint32_t self, uint32_t* buf, uint32_t len) {
    if (self == 0) return 1;
    if (buf != nullptr) fog_sub_f2f0(e, self, buf, len);
    if (L(e, self + 0x21b * 4) != 1 && L(e, self + 0x21b * 4) != 2) e->f5e9a0();
    e->f5d5e0();
    e->f5d710();
    fog_sub_e7e0(e, self);
    for (int k = 0; k < 4; k++) {
        const uint32_t a = self + (0x2e4 + k) * 4;
        if ((int32_t)L(e, a) != -1) { e->ord3((int)L(e, a)); S(e, a, 0xffffffffu); }
    }
    e->ordinal116();
    for (uint32_t off : {0x215u, 0x225u, 0x22du}) {
        const uint32_t cs = self + off * 4;
        lock(e, cs);
        e->delete_cs(cs);
    }
    uint32_t p;
    for (uint32_t n = L(e, self + 0x21e * 4); n != 0; n = L(e, self + 0x21e * 4)) {
        p = n;
        const uint32_t next = L(e, p + 0x16f * 4);
        e->free_ptr(&p);
        S(e, self + 0x21e * 4, next);
    }
    for (uint32_t n = L(e, self + 0x223 * 4); n != 0; n = L(e, self + 0x223 * 4)) {
        p = n;
        const uint32_t next = L(e, p + 12);
        e->free_ptr(&p);
        S(e, self + 0x223 * 4, next);
    }
    for (uint32_t off : {0x21cu, 0x21du, 0x10u, 0x22bu, 8u}) {
        p = L(e, self + off * 4);
        e->free_ptr(&p);
    }
    lock(e, self + 40);
    e->delete_cs(self + 40);
    p = L(e, self + 20);
    e->free_ptr(&p);
    const int32_t nb = (int32_t)L(e, 0x6ff7fee4);
    for (int32_t i = 0; i < nb; i++) {
        const uint32_t cs = L(e, self + 28) + (uint32_t)i * 0x18;
        lock(e, cs);
        e->delete_cs(cs);
    }
    p = L(e, self + 28);
    e->free_ptr(&p);
    p = self;
    e->free_ptr(&p);
    return 1;
}

// 1.13c Fog+0xD820
// Look up key `key` in the hash at self; on a hit, query Ordinal_5 for the entry's socket (+0x10)
// into a 16-byte local. Returns 0 on a miss; the hit return is stale EBP in retail, 1 here.
extern "C" uint32_t fog_sub_d820(const FogB64Env* e, uint32_t self, uint32_t key) {
    const uint32_t b = L(e, self + 0xc) & key;
    const uint32_t cs = L(e, self + 0x1c) + b * 0x18;
    lock(e, cs);
    uint32_t n = L(e, L(e, self + 0x14) + b * 4);
    while (n != 0 && L(e, n) != key) n = L(e, n + 4);
    if (n == 0) { e->leave(cs); return 0; }
    const uint32_t sock = L(e, n + 0x10);
    e->leave(cs);
    uint8_t buf[16];
    uint32_t len = 0x10;
    e->ord5(sock, buf, &len);
    return 1;
}

// 1.13c Fog+0xD7A0
// Same lookup; returns the entry's +0x10 field, or 0xFFFFFFFF on a miss.
extern "C" uint32_t fog_sub_d7a0(const FogB64Env* e, uint32_t self, uint32_t key) {
    const uint32_t b = L(e, self + 0xc) & key;
    const uint32_t cs = L(e, self + 0x1c) + b * 0x18;
    lock(e, cs);
    uint32_t n = L(e, L(e, self + 0x14) + b * 4);
    while (n != 0 && L(e, n) != key) n = L(e, n + 4);
    const uint32_t r = n ? L(e, n + 0x10) : 0xffffffffu;
    e->leave(cs);
    return r;
}

// 1.13c Fog+0xEC10
// Hacklist a client by id (only when self+0xBD0 set): miss logs "closed"; hit logs the IP and kicks.
extern "C" void fog_sub_ec10(const FogB64Env* e, uint32_t self, uint32_t key, uint32_t file, uint32_t line) {
    if (L(e, self + 0xbd0) == 0) return;
    const uint32_t b = L(e, self + 0xc) & key;
    const uint32_t cs = L(e, self + 0x1c) + b * 0x18;
    lock(e, cs);
    uint32_t n = L(e, L(e, self + 0x14) + b * 4);
    while (n != 0 && L(e, n) != key) n = L(e, n + 4);
    e->leave(cs);
    if (n == 0) {
        e->log(0x6ff77040, "[HACKLIST]  Couldn't hacklist client %d (closed) from FILE:%s  LINE:%d");
        return;
    }
    const uint32_t ip = L(e, n + 0x18);
    e->ord11(ip, file, line);
    e->log(0x6ff77040, "[HACKLIST]  IP:%s  QSHackList() from FILE:%s  LINE:%d");
    e->f5e550();
    e->f5df00(ip, file, line);
}

// shared lookup for the by-IP-id tables (self+0x20 buckets, key at +0x18, next at +0x1C)
static uint32_t find_ip(const FogB64Env* e, uint32_t self, uint32_t key) {
    const uint32_t cs = self + 0x28;
    lock(e, cs);
    uint32_t n = L(e, L(e, self + 0x20) + (L(e, self + 0xc) & key) * 4);
    while (n != 0 && L(e, n + 0x18) != key) n = L(e, n + 0x1c);
    e->leave(cs);
    return n;
}

// 1.13c Fog+0xEB60
extern "C" void fog_sub_eb60(const FogB64Env* e, uint32_t self, uint32_t key, uint32_t file, uint32_t line) {
    if (L(e, self + 0xbd0) == 0) return;
    if (find_ip(e, self, key) == 0) {
        e->ord11(key, 0, 0);
        e->log(0x6ff77040, "[HACKLIST]  Client %s is already closed");
    } else e->f5e550();
    e->ord11(key, file, line);
    e->log(0x6ff77040, "[HACKLIST]  IP:%s  QSHackListIP() from FILE:%s  LINE:%d");
    e->f5df00(key, file, line);
}

// 1.13c Fog+0xEA50
extern "C" void fog_sub_ea50(const FogB64Env* e, uint32_t self, uint32_t key, uint32_t file, uint32_t line) {
    if (find_ip(e, self, key) == 0) {
        e->ord11(key, 0, 0);
        e->log(0x6ff77040, "[HACKLIST]  Client %s is already closed");
    } else e->f5e550();
    e->ord11(key, file, line);
    e->log(0x6ff77040, "[HACKLIST]  IP:%s  QSDisconnectIP() from FILE:%s  LINE:%d");
}
