// 1.13c Storm+0x361C0: tear down the whole SEVT command table. For every bucket of the
// 0x55370 table, each record's group chain (record +0xC, linked through group +0xC) is released through 0x35FC0, the record
// is unlinked (+0x10) and freed ("delete", -1); then the bucket array itself is freed, the table/size/count words are zeroed
// and the dirty flag 0x52F58 is raised. Returns 1. Retail words are 32-bit; the lock is 0x6FC458C8.
#include <cstdint>
#include "storm_batch_19040.h"

struct StormBatch18f90Env {
    uint32_t (*get_window_long)(void*, int32_t);
    int32_t  (*get_window_rect)(void*, int32_t*);
    int32_t  (*get_system_metrics)(int32_t);
    void     (*mem_free)(void*, const char*, int32_t);
    void*    (*mem_alloc)(uint32_t, const char*, int32_t, uint32_t);
    uint32_t (*hash_string)(const uint8_t*, uint32_t, uint32_t);
};
extern "C" void Storm_Sub35FC0(uint32_t* grp, const StormBatch18f90Env* env);
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);

static void free361c0(void* p, const char* f, int32_t l) { Storm_Sub1B960(p, f, l); }

extern "C" uint32_t Storm_Sub361C0(void) {
    static const StormBatch18f90Env fe{ nullptr, nullptr, nullptr, free361c0, nullptr, nullptr };
    EnterCriticalSection(&g_evtLock);
    uint32_t* tbl = (uint32_t*)(uintptr_t)g_evtTable;
    for (uint32_t i = 0; i < g_evtSize; i++) {
        uint32_t* rec = (uint32_t*)(uintptr_t)tbl[i];
        while (rec) {
            for (uint32_t g = rec[3]; g != 0; g = rec[3]) {
                rec[3] = ((uint32_t*)(uintptr_t)g)[3];
                Storm_Sub35FC0((uint32_t*)(uintptr_t)g, &fe);
                tbl = (uint32_t*)(uintptr_t)g_evtTable;
            }
            tbl[i] = rec[4];
            Storm_Sub1B960(rec, "delete", -1);
            tbl = (uint32_t*)(uintptr_t)g_evtTable;
            rec = (uint32_t*)(uintptr_t)tbl[i];
        }
    }
    if (tbl) Storm_Sub1B960(tbl, "delete", -1);
    g_evtTable = 0;
    g_evtSize = 0;
    g_evtCount = 0;
    g_evtDirty = 1;
    LeaveCriticalSection(&g_evtLock);
    return 1;
}
