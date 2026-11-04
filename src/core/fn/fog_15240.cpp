// probe for Fog 0x15240 (sub_15240, loaded module list via toolhelp): stdcall(pid = r[0]); blobs pn r[14] (the tuple's pins: [0] LoadLibraryA, [1] GetProcAddress,
// [2] snapshot, [3] Module32First result, [4] module count, 16 dwords per module from [8]: base, size, name (4), path (4)), lg r[16] (hook log), n0..n5 r[18..28] (the nodes).
// Port: fog_sub_15240 with a module enumerator. The enumerator is the toolhelp side as retail runs it: the load / snapshot failures and their log lines, the snapshot
// call (hook 0x16920: stdcall, 2 words), Module32First / Module32Next (the same stub), and the allocator record (0x1B560, eax = 0x210) before each node.
// The node layout is retail's: the first string copy (the entry's szExePath) lands at +0 and the second (szModule) at +0x104, so the port's two names are fed crossed.
// After the call each node the port built is copied into the blob of its allocation order (found by its base) with its next pointer pointing at the next node's blob.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "hookenv.h"
struct FogModNode {
    char szModule[0x104];
    char szExePath[0x104];
    uint32_t base, size;
    FogModNode* next;
};
struct FogModEntry { const char* szModule; const char* szExePath; uint32_t base, size; };
typedef int (*FogModEnum)(uint32_t pid, FogModEntry* out, int idx);
FogModNode* fog_sub_15240(uint32_t pid, FogModEnum en);
namespace {
HookEnv* H;
const uint32_t* gPn;
char gName[8][17], gPath[8][17];
const uint32_t kLog = 0xB20268ECu;
const uint32_t kFn = 0x16920;
int enumerate(uint32_t pid, FogModEntry* out, int idx) {
    const uint32_t nm = gPn[4];
    if (idx == 0) {
        if (!gPn[0] || !gPn[1]) { H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB2027734u); return 0; }
        const uint32_t snap = H->h(kFn, 0, 0, 0, 0, 0, 8, pid);
        if (snap == 0xFFFFFFFFu) { H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB2027714u); return 0; }
        H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB20276FCu);
        const uint32_t first = H->h(kFn, 0, 0, 0, 0, 0, snap, (uint32_t)(uintptr_t)out);
        if (!first || !nm) return 0;
    } else {
        const uint32_t r = H->h(kFn, 0, 0, 0, 0, 0, gPn[2], (uint32_t)(uintptr_t)out);
        if (!r || (uint32_t)idx >= nm) return 0;
    }
    const uint32_t* m = gPn + 8 + 16 * idx;
    std::memcpy(gName[idx], m + 2, 16); gName[idx][16] = 0;
    std::memcpy(gPath[idx], m + 6, 16); gPath[idx][16] = 0;
    out->szModule = gPath[idx];      // retail's node +0 is the entry's szExePath
    out->szExePath = gName[idx];     // node +0x104 is szModule
    out->base = m[0]; out->size = m[1];
    H->h(0x1B560, 0x210, 0, 0, 0, 0, 0, 0);
    return 1;
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_15240(uint32_t* r) {
    gPn = (const uint32_t*)(uintptr_t)r[14];
    const uint32_t nm = gPn[4];
    const uint32_t snap = gPn[2], first = gPn[3];
    uint32_t sc[40]; uint32_t k = 0;
    sc[k++] = kFn; sc[k++] = snap; sc[k++] = kFn; sc[k++] = first;
    for (uint32_t i = 1; i < nm; i++) { sc[k++] = kFn; sc[k++] = 1; }
    sc[k++] = kFn; sc[k++] = 0; sc[k++] = 0; sc[k++] = 0;
    HookEnv e((uint32_t*)(uintptr_t)r[16], sc);
    H = &e;
    FogModNode* head = fog_sub_15240(r[0], enumerate);
    uint32_t order[8]; uint32_t cnt = 0;
    FogModNode* byIdx[8] = {};
    for (FogModNode* n = head; n && cnt < 8; n = n->next) {
        uint32_t idx = 0xFF;
        for (uint32_t i = 0; i < nm; i++) if (!byIdx[i] && gPn[8 + 16 * i] == n->base) { idx = i; break; }
        if (idx == 0xFF) abort();
        byIdx[idx] = n; order[cnt++] = idx;
    }
    for (uint32_t c = 0; c < cnt; c++) {
        uint8_t* dst = (uint8_t*)(uintptr_t)r[18 + 2 * order[c]];
        std::memcpy(dst, byIdx[order[c]], 0x210);
        const uint32_t nx = c + 1 < cnt ? r[18 + 2 * order[c + 1]] : 0;
        std::memcpy(dst + 0x20C, &nx, 4);
    }
    r[4] = cnt ? r[18 + 2 * order[0]] : 0;
}
