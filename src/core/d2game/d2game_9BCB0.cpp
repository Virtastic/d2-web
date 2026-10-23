// src/core/d2game/d2game_9BCB0.cpp -- 1.13c D2Game.dll sub_9BCB0: linked-list membership test.
// head (EAX) -> +4 first node; node+0 key, node+4 next. Null head is fatal (#10265(0xe5)/#10024, _exit(-1)).
#include <cstdint>
#include <cstdlib>
#include <unistd.h>

// 1.13c D2Game+0x9BCB0
uint32_t d2game_9BCB0(void* head, int key) {
    if (!head) _exit(-1);
    for (int** n = *(int***)((uint8_t*)head + 4); n; n = (int**)n[1]) // ponytail: 32-bit node layout; wasm64 build needs offsets if linked
        if ((int)(intptr_t)n[0] == key) return 1;
    return 0;
}
