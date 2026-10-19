#pragma once
#include <cstdint>
#include <mutex>

// Native owner of the module state used by these twelve Storm rows. This is NOT a
// retail-layout struct: only serialized records/addresses use uint32_t. Install
// the hooks a call reaches; resolve(0) must return nullptr, encode(nullptr) zero.
namespace storm25220 {
struct State {
    uint32_t error{}, heapReady{}, reportErrors{}, requiredRegistered{};
    uint32_t heaps[256]{};
    std::recursive_mutex locks[256];
    void* commandLists[2]{};
    uint8_t* (*resolve)(uint32_t){};
    uint32_t (*encode)(const void*){};
    uint8_t* (*allocateNode)(void*, uint32_t, uint32_t){}; // 0x4430
    void (*setLastError)(uint32_t){};
    void (*fatal)(uint32_t, const char*, int32_t, void*, uint32_t, uint32_t){}; // 0x2F590
    void (*blockDetails)(uint32_t*, const void*){}; // 0x1A5A0
    uint32_t (*heapByCaller)(){}; // 0x1AD80, including caller register context
};

uint32_t length(State&, const char*);
uint32_t copy(State&, char*, const char*, uint32_t);
uint32_t append(State&, char*, const char*, uint32_t);
void fill(void*, uint32_t, uint8_t);
int32_t compare(State&, const char*, const char*, uint32_t);
uint32_t hash(State&, const uint8_t*, uint32_t, uint32_t);
uint32_t registerArgument(State&, uint32_t, uint32_t, const char*, uint32_t,
                          uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t registerList(State&, const uint32_t*, uint32_t);
uint32_t heapByCaller(State&);
uint32_t findNextHeap(State&, uint32_t, uint32_t*, uint32_t*);
uint32_t findNextBlock(State&, uint32_t, uint8_t*, uint32_t*, uint32_t*);
void tokenize(State&, const char**, char*, uint32_t, const char*, uint32_t*);
} // namespace storm25220
