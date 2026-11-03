// 1.13c Storm+0x11720 (stdcall, 5 words, ret 0x14; Storm ordinal 139): hand a run of SNet slots to the provider. (first, count, a3, a4, a5).
// count == 0 or a3 == 0: last error (DAT 0x55388) = 0x57 (also SetLastError), return 0. Inside the SNet critical section (0x55A24): no provider (DAT 0x55834 == 0):
// last error = 0x4B4, return 0. Else allocate count dwords (SNET.CPP 0x14B7, flags 8) and fill entry j with DAT 0x55A5C + (first + j) * 0x80 while first + j < DAT 0x55A58
// (other entries stay as allocated; the loop is skipped when first + count wraps below first); call provider slot +0x54 when set, stdcall,
// (DAT 0x55238, count, entries, a3, a4, a5); free the entries (SNET.CPP 0x14C8) and return 1.
#include <cstdint>
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
struct Storm11720Env {
    uint32_t provider, cbSlot, g238, count5a58, base5a5c;
    uint32_t* lastError;
    void (*setLastError)(uint32_t);
    void (*enterCs)();
    void (*leaveCs)();
    void (*cb)(uint32_t g238, uint32_t count, uint32_t* entries, uint32_t a3, uint32_t a4, uint32_t a5);
};
extern "C" uint32_t Storm13c_11720(uint32_t first, uint32_t count, uint32_t a3, uint32_t a4, uint32_t a5, const Storm11720Env& e) {
    if (count == 0 || a3 == 0) { *e.lastError = 0x57; e.setLastError(0x57); return 0; }
    e.enterCs();
    if (e.provider == 0) { *e.lastError = 0x4B4; e.setLastError(0x4B4); e.leaveCs(); return 0; }
    uint32_t* arr = (uint32_t*)Storm_Sub1C580(count * 4u, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x14B7, 8);
    const uint32_t end = first + count;
    if (first < end) {
        uint32_t* p = arr;
        for (uint32_t i = first; i < end; i++, p++)
            if (i < e.count5a58) *p = e.base5a5c + i * 0x80u;
    }
    if (e.cbSlot) e.cb(e.g238, count, arr, a3, a4, a5);
    Storm_Sub1B960(arr, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x14C8);
    e.leaveCs();
    return 1;
}
