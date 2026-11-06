// 1.13c BNClient+0x12A70 seams (src/core/bnclient/bnclient_12a70.cpp): the news cache read 0x14050 (name = the global 0x1E0F0, stamp, &data, &size),
// the growable-array resize 0x11C0 (EAX = want, EBX = header) and step query 0x1180 (EAX = want, header), and the Ordinal_403 free (0x801E):
// which 0 = the table (type name, line -2), 1 = the file data (BNNews.cpp, line 0x12B).
#pragma once
#include <cstdint>
struct BnNews12a70Env {
    uint32_t (*load)(uint32_t* stamp, uint32_t** data, uint32_t* size);
    void (*grow)(uint32_t want, uint32_t* hdr);
    uint32_t (*step)(uint32_t want, uint32_t* hdr);
    void (*free)(void* p, uint32_t which);
};
extern "C" uint32_t bn_news_load_12a70(uint32_t* g, const BnNews12a70Env* e);
