// Batch b49: Fog seam-style ports (callees supplied by the caller).
#include <cstdint>

// 1.13c Fog+0x1C920: true iff GetVersionExA succeeds and dwPlatformId == 2 (VER_PLATFORM_WIN32_NT).
extern "C" bool fog_sub_1c920(bool (*get_version_ex)(uint32_t *platform_id)) {
    uint32_t platform = 0;
    if (!get_version_ex(&platform)) return false;
    return platform == 2;
}

// 1.13c Fog+0xD1A0: store param_2 at +3000; null param_1 runs the fatal path (seam, does not return).
extern "C" void fog_sub_d1a0(uint8_t *p, uint32_t v, void (*fatal)()) {
    if (!p) { fatal(); return; }
    *(uint32_t *)(p + 3000) = v;
}

// 1.13c Fog+0xD1F0: if the int at +0xBD0 is nonzero, log the hacklist size line (seam).
extern "C" void fog_sub_d1f0(const uint8_t *p, void (*log_hacklist_size)()) {
    if (*(const int32_t *)(p + 0xbd0) != 0) log_hacklist_size();
}

// 1.13c Fog+0xEA20: look up entry (Fog+0xE4E0 seam) for non-zero id; store value at dword [8].
extern "C" void fog_sub_ea20(void *table, uint32_t id, uint32_t value, uint32_t *(*find)(void *, uint32_t)) {
    if (id == 0) return;
    uint32_t *e = find(table, id);
    if (e) e[8] = value;
}

// 1.13c Fog+0xEB10: if int at +0xBD0 nonzero: format IP (ordinal 11), log, then Fog+0xDE00 unlist (seams).
extern "C" void fog_sub_eb10(const uint8_t *p, uint32_t ip, void (*log_unlist)(uint32_t), void (*unlist)(uint32_t)) {
    if (*(const int32_t *)(p + 0xbd0) == 0) return;
    log_unlist(ip);
    unlist(ip);
}
