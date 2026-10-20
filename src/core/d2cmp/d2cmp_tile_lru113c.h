#pragma once
#include <cstdint>

// Native ownership shared by the LRU operations. Retail's pointer fields become
// native pointers; byte-layout tile records remain explicitly serialized.
struct D2CmpLruNode113c {
    void* data{};                   // retail +0x00
    uint32_t size{};                // +0x04
    uint32_t callbackValue{};       // +0x08
    D2CmpLruNode113c* next{};        // +0x0c
    D2CmpLruNode113c* previous{};    // +0x10
};
struct D2CmpLru113c {
    uint32_t activeCount{};         // retail +0x0c
    D2CmpLruNode113c* first{};       // +0x18
    D2CmpLruNode113c* last{};        // +0x1c
    uint32_t touches{};             // +0x30
};
extern "C" {
uint32_t d2cmp_15210(const void* tile);
uint32_t d2cmp_152a0(const void* tile);
uint16_t d2cmp_15300(const void* tile);
D2CmpLruNode113c* d2cmp_17170(D2CmpLru113c* cache);
void d2cmp_17320(D2CmpLru113c* cache, D2CmpLruNode113c* node);
void d2cmp_173a0(D2CmpLru113c* cache, D2CmpLruNode113c* node);
}
