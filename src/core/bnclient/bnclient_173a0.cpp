// BNClient DllMain-shaped critical-section attach (retail 1.13c); the OS critical-section calls are injected.
#include <cstdint>

struct Bn173a0Env {
    void (*init_cs)();   // InitializeCriticalSection(&DAT_6FF3F1DC)
    void (*delete_cs)(); // DeleteCriticalSection(&DAT_6FF3F1DC)
};
struct Bn173a0State {
    uint32_t head, tail, err_code; // DAT_6FF3F64C, DAT_6FF3F650, DAT_6FF3F1F4
    uint32_t saved;                // DAT_6FF3F61C
};

// 1.13c BNClient+0x173A0
extern "C" uint32_t bnclient_173a0(const Bn173a0Env* e, Bn173a0State* s, uint32_t arg, int32_t reason) {
    if (reason == 0) {
        e->delete_cs();
    } else if (reason == 1) {
        s->head = 0; s->tail = 0; s->err_code = 0;
        e->init_cs();
    }
    s->saved = arg;
    return 1;
}
