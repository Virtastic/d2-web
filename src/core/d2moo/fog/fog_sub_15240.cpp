// 1.13c Fog+0x15240 -- build a base-address-sorted list of the process's loaded modules
// (toolhelp snapshot). Win32 is a host seam: the caller supplies the enumerator.
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct FogModNode {            // 0x210 bytes, retail's layout: the second name ends at +0x204 where base starts
    char     szModule[0x104];  // +0x000
    char     szExePath[0x100]; // +0x104
    uint32_t base;             // +0x204
    uint32_t size;             // +0x208
    uint32_t next;             // +0x20C, 32-bit guest pointer (the node stays 0x210 bytes on the 64-bit host)
};
static inline FogModNode* N(uint32_t v) { return (FogModNode*)(uintptr_t)v; }
static inline uint32_t G(const FogModNode* p) { return (uint32_t)(uintptr_t)p; }
struct FogModEntry { const char* szModule; const char* szExePath; uint32_t base, size; };

// seam: returns 0 on load/snapshot failure (retail logs and returns null), else calls out per module.
typedef int (*FogModEnum)(uint32_t pid, FogModEntry* out, int idx);

FogModNode* fog_sub_15240(uint32_t pid, FogModEnum en) {
    uint32_t head = 0;
    FogModEntry e;
    for (int i = 0; en(pid, &e, i); ++i) {
        FogModNode* n = (FogModNode*)calloc(1, sizeof(FogModNode));
        if (!n) return N(head);
        strncpy(n->szModule, e.szModule, 0x103);
        strncpy(n->szExePath, e.szExePath, 0xFF);
        n->base = e.base; n->size = e.size;
        uint32_t* p = &head;                          // ascending insert by base (before equal bases)
        while (*p && N(*p)->base < n->base) p = &N(*p)->next;
        n->next = *p; *p = G(n);
    }
    return N(head);
}
