#pragma once
#include <cstdint>
struct D2C7DE50 {
    void* const* nodes_10a808;   // 128 buckets, type 1
    void* const* nodes_10aa08;   // table passed to 54E20 for type 2
    void** obj_113b10;           // DAT_6fbc3b10
    uint16_t* w_113af4;          // DAT_6fbc3af4
    uint32_t* d_113ac6;          // DAT_6fbc3ac6
    uint32_t d_113ace;           // DAT_6fbc3ace (value)
    void* (*find_54e20)(uint32_t key, void* const* tbl, int type);
    void (*free_10660)(void*);
    void* (*new_10204)(int);
    void (*copy_10485)(void*, const void*);
    void (*fn_10151)(void*);
    void (*del_10422)(int, void*);
    void* (*str_10454)(int, const char*, uint32_t);
    void (*s_7d200)(int16_t, int);
    void (*s_7c8f0)(uint16_t);   // retail: ret 4, the zero-extended u16 @+10
    void (*fail)(void);          // 10265(0x5a)+10024 then _exit(-1)
};
extern "C" void d2_client_7de50(const uint8_t* pkt, D2C7DE50* c);
