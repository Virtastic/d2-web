// 1.13c D2Net+0x7540 (ordinal 10034): the connection-state init. stdcall(mode, threadParam), ret 8.
// Every module global it touches and every callee it makes are explicit in the environment: the two queue heads are
// cleared, the critical section is initialised, a 0x7bc-byte state block is allocated (Fog #10042 through the 0x5E34
// thunk, size 0x7bc, line 0xc2, flag 0) and zeroed, the mode is stored. Modes 1 and 2 (local) then push the two-byte
// record {0xAF, 0x00} through sub_63E0(0, 0, rec, 2) and return 1; any other mode starts the receive thread
// (start routine D2Net+0x7030, parameter threadParam, flags 0, thread id into the id global) and returns its handle,
// which is also stored in the thread global.
#include <cstdint>
#include <cstring>

struct D2Net7540Env {
    uint32_t* threadId;      // DAT_6fbfb240
    uint8_t** block;         // DAT_6fbfb244
    uint32_t* queueB;        // DAT_6fbfb3f4
    uint32_t* thread;        // DAT_6fbfb3f8
    uint32_t* queueA;        // DAT_6fbfb3fc
    uint32_t* mode;          // DAT_6fbfb418
    void* ctx;
    void (*initCS)(void* ctx);                                                     // InitializeCriticalSection(&DAT_6fbfb400)
    uint8_t* (*alloc)(void* ctx, uint32_t size, uint32_t line, uint32_t flag);       // 0x5E34 -> Fog #10042
    uint32_t (*send)(void* ctx, uint32_t kind, uint32_t arg, uint8_t* buf, uint32_t len);   // sub_63E0
    uint32_t (*createThread)(void* ctx, uint32_t startRva, uint32_t param, uint32_t flags, uint32_t* threadId);   // CreateThread(0, 0, ...)
};

uint32_t d2net_7540(const D2Net7540Env* e, uint32_t mode, uint32_t threadParam) {
    *e->queueA = 0;
    *e->queueB = 0;
    e->initCS(e->ctx);
    uint8_t* blk = e->alloc(e->ctx, 0x7bc, 0xc2, 0);
    *e->block = blk;
    std::memset(blk, 0, 0x1ef * 4);
    *e->mode = mode;
    if (mode == 1 || mode == 2) {
        uint8_t rec[2] = { 0xaf, 0x00 };   // retail writes it over its own first argument slot
        e->send(e->ctx, 0, 0, rec, 2);
        return 1;
    }
    const uint32_t h = e->createThread(e->ctx, 0x7030, threadParam, 0, e->threadId);
    *e->thread = h;
    return h;
}
