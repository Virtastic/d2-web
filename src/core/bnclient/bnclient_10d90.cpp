// 1.13c BNClient+0x10D90: wait for slot N. Returns 1 as soon as slots[idx] is non-zero; if the abort word is set first
// the error 0x12 is recorded (only when none is set, under the critical section) and 0 returned; after 45 s without either, 0x11 likewise. Polls every 10 ms.
#include <cstdint>

struct Bn10d90Cb {
    uint32_t (*tick)();                  // GetTickCount
    void (*sleep)(uint32_t ms);          // Sleep
    void (*enter)();                     // EnterCriticalSection(DAT_1F1DC)
    void (*leave)();                     // LeaveCriticalSection(DAT_1F1DC)
};

extern "C" uint32_t bnclient_wait_ready_10d90(const uint32_t* slots, uint8_t idx, const uint32_t* abort_word, uint32_t* err, const Bn10d90Cb* cb) {
    const uint32_t t0 = cb->tick();
    uint32_t v = slots[idx];
    for (;;) {
        if (v != 0) return 1;
        if (*abort_word != 0) break;
        if (cb->tick() - t0 > 45000) {
            cb->enter(); if (*err == 0) *err = 0x11; cb->leave();
            return 0;
        }
        cb->sleep(10);
        v = slots[idx];
    }
    cb->enter(); if (*err == 0) *err = 0x12; cb->leave();
    return 0;
}
