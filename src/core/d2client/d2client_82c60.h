// 1.13c D2Client+0x82C60: item packet -- an item leaves the player's inventory for the cursor.
// Retail words (unit pointers, the inventory handle) are 32-bit addresses; callees are the thunk RVAs named per member.
#pragma once
#include <cstdint>
struct D2c82c60G {
    uint32_t* player;   // +0x11BBFC  player unit (retail word)
    uint32_t* hash;     // +0x10AE08  item unit hash, 128 buckets, chain at unit+0xE4, id at +0xC, type at +0
    uint8_t*  cur;      // +0x11C484  byte[4], cleared for slot word L+4 < 4
    uint32_t* b834;     // +0x11B834
    uint32_t* b854;     // +0x11B854
    uint32_t* b858;     // +0x11B858
    uint32_t* b864;     // +0x11B864
};
struct D2c82c60Env {
    void     (*ca90)(uint32_t src, uint32_t n, uint32_t zero, uint8_t* out);       // bit-stream unpack into the 24-byte local
    uint32_t (*c6d0)(uint32_t inv, uint32_t item);                                 // returns the item or 0
    void     (*caa2)(uint32_t inv, uint32_t slot);
    void     (*f26410)();
    void     (*ca0c)(uint32_t inv);
    uint32_t (*c1a8)(uint32_t item, uint32_t flag, uint32_t line, uint32_t file);
    void     (*c544)(uint32_t pl, uint32_t item);
    uint32_t (*ca3c)(uint32_t v, uint32_t mask);
    void     (*f82980)(uint32_t eax, uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*c7d8)(uint32_t inv, uint32_t item);
    void     (*c202)(uint32_t inv);
    void     (*f162d0)();
    void     (*f81aa0)();
    void     (*a6ef0)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1);       // unhash: ECX bucket base, EDX id&0x7f; id, type
    void     (*fatal)(uint32_t code);                                              // 10265/10024 + _exit(-1)
};
extern "C" void d2_client_82c60(D2c82c60G* g, const D2c82c60Env* e, const uint8_t* pkt);
