// Seam for D2Game+0x6A9D0 (OBJECTS_OperateFunction08_Door): the door operate rule.
// Every callee is a seam in retail call order, so the engine (ObjMode.cpp) and the function probe run one rule.
#pragma once
#include <cstdint>
struct D2G6A9D0Env {
    void* (*room)(void* obj);                                                             // #10331 thunk 0xA478
    void (*coords)(void* obj, int32_t* xy);                                                // #10141 thunk 0xA544 (out x, y)
    int32_t (*sizeX)(void* obj);                                                           // #10589 thunk 0xA71E
    int32_t (*sizeY)(void* obj);                                                           // #10904 thunk 0xB13E
    uint32_t (*tick)();                                                                    // GetTickCount
    uint32_t (*mode)(void* obj);                                                           // obj+0x10
    uint32_t (*stamp)(void* obj);                                                          // obj+0xD4
    void (*setStamp)(void* obj, uint32_t t);
    int32_t (*keyCheck)(void* player, void* game);                                         // 0xD4140: EAX = player, stack game
    void (*sound)(void* player, int32_t type, void* upd);                                  // 0xDFFD0: ESI = player, stack (type, upd)
    void (*freePath)(void* obj);                                                           // #10408 thunk 0xA550
    uint32_t (*collide)(void* room, int32_t x, int32_t y, int32_t sx, int32_t sy, uint32_t mask); // #10303 thunk 0xAE38; AX tested
    void (*blockPath)(void* obj, void* room, int32_t x, int32_t y);                        // #10032 thunk 0xA69A
    void (*setMode)(void* obj, int32_t mode);                                              // #11090 thunk 0xA4A2
};
int32_t d2g_6a9d0(void* game, void* obj, void* player, const D2G6A9D0Env* e);
