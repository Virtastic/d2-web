// 1.13c Storm+0x1BDD0: allocate and link a heap node (reserve param_6 bytes, commit param_5, header
// of 0x60 bytes + copy of the owner name). Retail passes the name in EAX; here it is the first arg.
// Words are 32-bit as in retail; pointer slots keep the low 32 bits (ponytail: wasm64 ceiling).
#include <cstdint>
#include <cstring>
#include <cstdlib>

// Win32 seams: reserve+commit collapse to one zeroed block; fatal error 8 (0x2F590) has no UI here.
static void* Storm_VirtualReserve(size_t n) { return calloc(1, n); }
static void* Storm_VirtualCommit(void* p, size_t) { return p; }
static void Storm_SErrDisplayError(uint32_t, const char*, int32_t, void*, uint32_t, uint32_t) {}

uint32_t* g_stormHeapLists_1bdd0[0x100];               // 0x6FC43150

uint32_t* Storm_Sub1BDD0(const char* name, int32_t p1, uint32_t p2, int32_t id, uint32_t p4,
                         size_t commit, size_t reserve) {
    uint32_t* n = (uint32_t*)Storm_VirtualReserve(reserve);
    if (!n || !Storm_VirtualCommit(n, commit)) {
        Storm_SErrDisplayError(8, name, p1, nullptr, 0, 1);
        exit(1);
    }
    uint32_t len = name ? strlen(name) : 0;
    uintptr_t end = (uintptr_t)n + ((0x64 + len + 3) & ~3u);  // 100 + len, rounded up to 4
    n[1] = p2;
    n[7] = n[8] = (uint32_t)end;
    n[10] = p4; n[11] = (uint32_t)commit; n[12] = (uint32_t)reserve;
    n[0] = (uint32_t)(uintptr_t)g_stormHeapLists_1bdd0[id];
    n[2] = id; n[4] = 1; n[9] = 0; n[0x17] = p1;
    memset(&n[0xe], 0, 9 * 4);
    if (name) memcpy(&n[0x18], name, len + 1); else *(char*)&n[0x18] = 0;
    n[3] = 0x6f6d0000u | (((uintptr_t)n >> 16) & 0xffff);
    g_stormHeapLists_1bdd0[id] = n;
    return n;
}
