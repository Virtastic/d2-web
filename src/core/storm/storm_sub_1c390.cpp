// 1.13c Storm+0x1C390: pool block allocate (SMemAlloc core). Register args in retail: EAX = heap record
// (+0x5c line, +0x60 name), EBX = &held lock index, stack = (flags, size). Sizes >= 2^31 and failed
// allocations drop the held lock and are fatal (0x1F590 then ExitProcess(1)).
#include <cstdint>
#include <Windows.h>

struct Storm1C390Env {
    uint32_t* (*f_1c0c0)(uint32_t size, uint8_t flags);
    void      (*f_12c50)(void* p, uint32_t n, uint8_t fill);
    void      (*f_1f590)(uint32_t code, const void* msg, int32_t line, uint8_t* buf, int32_t a, int32_t b);
    void      (*exit_process)(uint32_t code);
    CRITICAL_SECTION* locks;   // 0x6FC43558, 0x18 bytes each
    uint32_t  fill_on;         // 0x6FC45164
};

static void drop(const Storm1C390Env* e, int32_t* lk) {
    if (*lk != -1) { LeaveCriticalSection(&e->locks[*lk]); *lk = -1; }
}

extern "C"
uint32_t* Storm_Sub1C390(uint32_t flags, uint32_t size, uint8_t* heap, int32_t* lk, const Storm1C390Env* e) {
    if (size > 0x7fffffffu) {
        drop(e, lk);
        e->f_1f590(8, heap + 0x60, *(int32_t*)(heap + 0x5c), nullptr, 0, 1);
        e->exit_process(1);
    }
    if (heap != nullptr) {
        uint8_t f = 0;
        if (flags & 0x4000000) f = 0x40;
        if (flags & 0x8000000) f |= 0x80;
        uint32_t* p = e->f_1c0c0(size, f);
        if (p != nullptr) {
            if (flags & 8) { for (uint32_t i = 0; i < size; i++) ((uint8_t*)p)[i] = 0; }
            else if (e->fill_on != 0) e->f_12c50(p, size, 0xee);
            return p;
        }
    }
    drop(e, lk);
    if (heap != nullptr && heap[0x60] != 0) e->f_1f590(8, heap + 0x60, *(int32_t*)(heap + 0x5c), nullptr, 0, 1);
    else e->f_1f590(8, "SMemHeapAlloc()", -1, nullptr, 0, 1);
    e->exit_process(1);
    return nullptr;
}
