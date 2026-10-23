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

// Node (0x210): path[0x104] name[0x100+] base@0x204 size@0x208 next@0x20C. Pointers are host-width here.
struct FogModNode { char path[0x104]; char name[0x100]; uint32_t base, size; FogModNode* next; };

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
        FogModNode* head = nullptr;
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
            FogModNode *h = nd, *prev = nullptr;
            for (;;) {
                FogModNode* nx = nd->next;
                head = h;
                if (!nx || nd->base <= nx->base) break;
                if (nd != h) prev->next = nx; else head = nx;
                nd->next = nx->next;
                nx->next = nd;
                h = head;
                prev = nx;
            }
        }
        s.unload();
        return head;
    }
failMsg:
    s.log(err);
fail:
    s.unload();
    s.freeList(nullptr);
    return nullptr;
}
