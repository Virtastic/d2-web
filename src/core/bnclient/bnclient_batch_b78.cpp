// BNClient.dll b78: one function from the cached Ghidra decompile of retail 1.13c Bnclient.dll.
#include <cstdint>

struct BnB78Env {
    void (*init_cs)();   // InitializeCriticalSection(&DAT_6FF3F1DC)
    void (*delete_cs)(); // DeleteCriticalSection(&DAT_6FF3F1DC)
};
struct BnB78State {
    uint32_t head, tail, err_code; // DAT_6FF3F64C, DAT_6FF3F650, DAT_6FF3F1F4
    uint32_t saved;                // DAT_6FF3F61C
};

// 1.13c BNClient+0x173A0
extern "C" uint32_t bnb78_cs_attach(const BnB78Env* e, BnB78State* s, uint32_t arg, int32_t reason) {
    if (reason == 0) {
        e->delete_cs();
    } else if (reason == 1) {
        s->head = 0; s->tail = 0; s->err_code = 0;
        e->init_cs();
    }
    s->saved = arg;
    return 1;
}
