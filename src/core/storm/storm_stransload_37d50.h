#pragma once
#include <cstdint>
struct StormStransEnv {
    uint32_t* lastError;                                   // 0x55388
    void (*setLastError)(uint32_t code);
    uint32_t (*load)(uint32_t name, uint32_t buf, uint32_t size, uint32_t* w, uint32_t* h);   // 0x33EA0 (size probe when buf == 0, else read)
    uint32_t (*alloc)(uint32_t size, uint32_t fileRva, uint32_t line);                         // 0x1C580
    void (*convert)(uint32_t buf, uint32_t w, uint32_t h, uint32_t depth, uint32_t p2, uint32_t p3, uint32_t* out);   // 0x37C10
    void (*heapFree)(uint32_t ptr, uint32_t fileRva, uint32_t line);                           // 0x1B960
};
