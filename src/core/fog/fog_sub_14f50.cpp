// 1.13c Fog+0x14F50: build a list of a process's loaded modules via psapi.dll, sorted ascending by base address.
#include <cstdint>
#include <cstring>

// Host seams (Win32 psapi + Fog helpers); null psapi => retail's LoadLibrary failure => return null.
struct FogPsapiSeams {
    bool (*load)();                                                             // LoadLibraryA("psapi.dll") + 4 GetProcAddress
    void (*unload)();                                                           // FreeLibrary
    int (*enumModules)(uint32_t proc, uint32_t* mods, uint32_t cb, uint32_t* needed);
    int (*modInfo)(uint32_t proc, uint32_t mod, uint32_t* info3, uint32_t cb);  // base,size,entry
    int (*fileName)(uint32_t proc, uint32_t mod, char* buf, uint32_t n);
    int (*baseName)(uint32_t proc, uint32_t mod, char* buf, uint32_t n);
    void* (*alloc)(uint32_t size);                                              // Fog+0x1CD10 (0x210, Exceptions.cpp, 0x47b)
    void (*log)(const char* fmt, ...);                                          // Fog+0x190A0
    void (*freeList)(void* head);                                               // Fog+0x14C30
};
FogPsapiSeams g_fogPsapi{};

// Node (0x210): path[0x104] name[0x100] base@0x204 size@0x208 next@0x20C. next is the 32-bit guest pointer (retail's layout), so the node stays 0x210 bytes
// on the 64-bit host and the allocator's 0x210 holds it.
struct FogModNode { char path[0x104]; char name[0x100]; uint32_t base, size; uint32_t next; };
static inline FogModNode* N(uint32_t v) { return (FogModNode*)(uintptr_t)v; }
static inline uint32_t G(const FogModNode* p) { return (uint32_t)(uintptr_t)p; }

extern "C" FogModNode* fog_sub_14f50(uint32_t proc) {  // 1.13c Fog+0x14F50
    const FogPsapiSeams& s = g_fogPsapi;
    if (!s.load()) return nullptr;
    uint32_t mods[256], needed = 0, info[3];
    char path[260], name[256];
    const char* err = nullptr;
    if (!s.enumModules(proc, mods, 0x400, &needed)) {
        s.log("Error enumerating modules using PSAPI:  error(%d)");
        goto fail;
    }
    {
        uint32_t n = needed >> 2;
        if (n > 0x100) { err = "PSAPI EnumProcessModules returned invalid number of modules"; goto failMsg; }
        s.log("\nEnumerate modules...");
        uint32_t head = 0;
        for (uint32_t i = 0; i < n; ++i) {
            if (!s.modInfo(proc, mods[i], info, 0xc)) { s.log("ERROR: PSAPI GetModuleInformation (%d)"); goto fail; }
            if (!s.fileName(proc, mods[i], path, 0x104)) { s.log("ERROR: PSAPI GetModuleFileNameExA (%d)"); goto fail; }
            if (!s.baseName(proc, mods[i], name, 0xff)) { s.log("ERROR: PSAPI GetModuleBaseNameA (%d)"); goto fail; }
            FogModNode* nd = (FogModNode*)s.alloc(0x210);
            memset(nd, 0, 0x210);
            strcpy(nd->path, path);
            strcpy(nd->name, name);
            nd->base = info[0];
            nd->size = info[1];
            nd->next = head;
            // One bubble pass: bubble nd toward the tail while its base exceeds its successor's.
            uint32_t h = G(nd), prev = 0;
            for (;;) {
                const uint32_t nx = nd->next;
                head = h;
                if (!nx || nd->base <= N(nx)->base) break;
                if (G(nd) != h) N(prev)->next = nx; else head = nx;
                nd->next = N(nx)->next;
                N(nx)->next = G(nd);
                h = head;
                prev = nx;
            }
            head = h;
        }
        s.unload();
        return N(head);
    }
failMsg:
    s.log(err);
fail:
    s.unload();
    s.freeList(nullptr);
    return nullptr;
}
