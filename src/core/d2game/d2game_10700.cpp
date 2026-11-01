// Item teardown, first phase (1.13c D2Game+0x10700; 1.10f sub_6FC502B0), split out of d2game_batch_9bf40.cpp.
// EDI is the game, ESI the item. A socketable item (D2Common #10757) has its socketed contents removed
// (0xE1110, stack game, item) and its inventory (item+0x60) freed (#10820); then the unit's timer chain is
// released (0x8E880: EAX = game, stack item). Plain ret.
#include <cstdint>
#include <cstring>

struct D2G10700Env {
    int32_t (*itemIsSocketable)(void* item);                 // D2Common #10757
    void    (*removeAllItemsFromInventory)(void* game, void* item);   // 0xE1110
    void    (*freeInventory)(void* inventory);               // D2Common #10820
    void    (*freeTimers)(void* game, void* item);           // 0x8E880
};

// 1.13c D2Game+0x10700
extern "C" void d2g_10700(void* game, void* item, const D2G10700Env* env)
{
    if (env->itemIsSocketable(item)) {
        env->removeAllItemsFromInventory(game, item);
        void* inv; std::memcpy(&inv, (const uint8_t*)item + 0x60, sizeof inv);
        env->freeInventory(inv);
    }
    env->freeTimers(game, item);
}
