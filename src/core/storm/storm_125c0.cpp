// 1.13c Storm+0x125C0 (stdcall, ret 8): store a 0x80-byte record in slot idx of the growable player table (retail 0x6FC45A54: capacity, 0x...58: count,
// 0x...5C: slot array, 0x...60: growth step). idx > 0xFF is 0x57. A slot at or past the count extends the table: past the capacity, the new capacity is
// need rounded up to the step (0x2C20 picks the step when none is stored: need >= 8 stores 8, below 8 the highest power of two <= need, 1 for 0/1), and
// 0x5440 reallocates the array (SMemReAlloc, flag 0x10; on failure SMemAlloc, copy min(newcap, count) slots, SMemFree the old); the count becomes need and
// only the first byte of each new slot is cleared. The record is then string-copied (0x3B080, max 0x80: through the first NUL, byte 0x7F forced to 0) over slot idx.
#include <cstdint>
#include <cstring>

extern "C" void* Storm_SMemReAlloc(void* p, uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);

namespace {
struct Tbl { uint32_t cap, count; uint8_t* ptr; uint32_t step; };
Tbl g_t;
int32_t g_err;   // 0x6FC45388
const char kTag[] = "AU_PLAYERNAME";

// 1.13c Storm+0x2C20
uint32_t Sub2C20(uint32_t n, Tbl* t) {
    if (n < 8) {
        uint32_t u = (n - 1) & n;
        while (u != 0) { n = u; u = (u - 1) & u; }
        return n == 0 ? 1 : n;
    }
    t->step = 8;
    return 8;
}

// 1.13c Storm+0x5440 (EAX = new capacity, EBX = the table)
void Sub5440(uint32_t newcap, Tbl* t) {
    uint8_t* old = t->ptr;
    t->cap = newcap;
    uint8_t* p = (uint8_t*)Storm_SMemReAlloc(old, newcap << 7, kTag, -2, 0x10);
    t->ptr = p;
    if (p == nullptr) {
        p = (uint8_t*)Storm_Sub1C580(newcap << 7, kTag, -2, 0);
        t->ptr = p;
        if (old != nullptr) {
            uint32_t n = t->count <= newcap ? t->count : newcap;
            if (n != 0) memcpy(p, old, (size_t)n << 7);
            Storm_Sub1B960(old, kTag, -2);
        }
    }
}
}  // namespace

void Storm_Sub125C0(uint32_t idx, const uint32_t* rec) {
    if (idx > 0xff) { g_err = 0x57; return; }
    const uint32_t need = idx + 1, old = g_t.count;
    if (need > old) {
        if (need > g_t.cap) {
            uint32_t step = g_t.step;
            if (step == 0) step = Sub2C20(need, &g_t);
            const uint32_t rem = need % step;
            Sub5440(rem ? need - rem + step : need, &g_t);
        }
        g_t.count = need;
        for (uint32_t i = old; i < need; i++) g_t.ptr[(size_t)i << 7] = 0;
    }
    // 0x3B080 is the bounded strcpy (SStrCopy, max 0x80), not a memcpy: it copies through the first NUL and forces byte 0x7F to 0.
    uint8_t* dst = g_t.ptr + ((size_t)idx << 7);
    const uint8_t* src = (const uint8_t*)rec;
    uint32_t i = 0;
    for (; i < 0x80; i++) { dst[i] = src[i]; if (src[i] == 0) break; }
    if (i == 0x80) dst[0x7F] = 0;
}

// FN seam: tw = {cap, count, slot array address, step} and the error word, in and out.
extern "C" void Storm_125C0_FnRun(uint32_t* tw, uint32_t idx, const uint32_t* rec, uint32_t* err) {
    g_t.cap = tw[0]; g_t.count = tw[1]; g_t.ptr = (uint8_t*)(uintptr_t)tw[2]; g_t.step = tw[3]; g_err = (int32_t)*err;
    Storm_Sub125C0(idx, rec);
    tw[0] = g_t.cap; tw[1] = g_t.count; tw[2] = (uint32_t)(uintptr_t)g_t.ptr; tw[3] = g_t.step; *err = (uint32_t)g_err;
}
