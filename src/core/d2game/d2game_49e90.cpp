// 1.13c D2Game+0x49E90 CLIENTS_FreeClientsFromGame (Clients.cpp:0x603): walk pGame+0x88 client list,
// following client+0x4A8 (pNext) read before the free, releasing each node via D2Common #10046.
#include <cstdint>

struct D2Game49E90Env {
    void (*freePool)(void* pool, void* p); // D2_FREE_POOL (#10046); file/line = Clients.cpp, 0x603
};

void CLIENTS_FreeClientsFromGame_49E90(void* pGame, void* pool, const D2Game49E90Env* e)
{
    void* c = *(void**)((uint8_t*)pGame + 0x88);
    while (c) {
        void* next = *(void**)((uint8_t*)c + 0x4A8);
        e->freePool(pool, c);
        c = next;
    }
}
