// 1.13c Fog+0x15240 -- build a base-address-sorted list of the process's loaded modules
// (toolhelp snapshot). Win32 is a host seam: the caller supplies the enumerator.
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct FogModNode {            // 0x210 bytes
    char     szModule[0x104];  // +0x000
    char     szExePath[0x104]; // +0x104 (overlaps decompile's 0x104 copy; see spec)
    uint32_t base;             // +0x204
    uint32_t size;             // +0x208
    FogModNode* next;          // +0x20C
};
struct FogModEntry { const char* szModule; const char* szExePath; uint32_t base, size; };

// seam: returns 0 on load/snapshot failure (retail logs and returns null), else calls out per module.
typedef int (*FogModEnum)(uint32_t pid, FogModEntry* out, int idx);

FogModNode* fog_sub_15240(uint32_t pid, FogModEnum en) {
    FogModNode* head = nullptr;
    FogModEntry e;
    for (int i = 0; en(pid, &e, i); ++i) {
        FogModNode* n = (FogModNode*)calloc(1, sizeof(FogModNode));
        if (!n) return head;
        strncpy(n->szModule, e.szModule, 0x103);
        strncpy(n->szExePath, e.szExePath, 0x103);
        n->base = e.base; n->size = e.size;
        FogModNode** p = &head;                       // ascending insert by base
        while (*p && (*p)->base < n->base) p = &(*p)->next;
        n->next = *p; *p = n;
    }
    return head;
}
