// MSVC CRT lock table and onexit list linked into D2Net.dll, as a single-threaded wasm model:
// a lock is a depth count (no real critical section), the allocator is the host's.
// 178D, 17D6, 1911, 1991, 19B9, 19EB had no cached decompile: written from the VS2003 CRT shape.
#include <cstdint>
#include <initializer_list>
#include <cstdlib>

typedef int (*d2_onexit_fn)();
static int g_crtLockDepth[48];
static bool g_crtLockMade[48];
static d2_onexit_fn *g_onexitBegin, *g_onexitEnd;

static void crt_exit_host(int) { std::abort(); }  // host seam: __exit(0xff) ends the process

// 1.13c D2Net+0x175A
void d2_realm_net_amsg_exit(int msg) { (void)msg; crt_exit_host(0xff); }

// 1.13c D2Net+0x182B
void d2_realm_net_unlock(int n) { --g_crtLockDepth[n]; }

// 1.13c D2Net+0x1840
int d2_realm_net_mtinitlocknum(int n) { g_crtLockMade[n] = true; return 1; }

// 1.13c D2Net+0x18D7
void d2_realm_net_unlock_10() { d2_realm_net_unlock(10); }

// 1.13c D2Net+0x18E0
void d2_realm_net_lock_n(int n) {
    if (!g_crtLockMade[n] && !d2_realm_net_mtinitlocknum(n)) d2_realm_net_amsg_exit(0x11);
    ++g_crtLockDepth[n];
}

// 1.13c D2NET+0x0178D
int d2_realm_net_mtinitlocks() {
    for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) g_crtLockMade[i] = true;  // statically-seeded slots
    return 1;
}

// 1.13c D2NET+0x017D6
void d2_realm_net_mtdeletelocks() {
    for (int i = 0; i < 48; ++i) { g_crtLockMade[i] = false; g_crtLockDepth[i] = 0; }
}

// 1.13c D2NET+0x01911
d2_onexit_fn d2_realm_net_onexit_lk(d2_onexit_fn f) {
    size_t n = (size_t)(g_onexitEnd - g_onexitBegin);
    d2_onexit_fn *nb = (d2_onexit_fn *)realloc(g_onexitBegin, (n + 1) * sizeof(d2_onexit_fn));
    if (!nb) return nullptr;
    g_onexitBegin = nb; g_onexitEnd = nb + n + 1;
    nb[n] = f;
    return f;
}

// 1.13c D2NET+0x01991
void d2_realm_net_onexitinit() {
    g_onexitBegin = (d2_onexit_fn *)calloc(32, sizeof(d2_onexit_fn));
    g_onexitEnd = g_onexitBegin;  // empty list; 32 slots reserved
}

// 1.13c D2NET+0x019EB
void d2_realm_net_unlock_8() { d2_realm_net_unlock(8); }

// 1.13c D2NET+0x019B9
d2_onexit_fn d2_realm_net_onexit(d2_onexit_fn f) {
    d2_realm_net_lock_n(8);
    d2_onexit_fn r = d2_realm_net_onexit_lk(f);
    d2_realm_net_unlock_8();
    return r;
}

// 1.13c D2Net+0x19F1
int d2_realm_net_atexit(d2_onexit_fn f) { return (d2_realm_net_onexit(f) != nullptr) - 1; }
