#pragma once
#include "storm_batch_25220.h"
#include <cstdio>

// Hooks for the Storm command-line / heap-trim / net-event rows of batch b144a.
// Retail records stay bytes with 32-bit addresses; resolve()/encode() translate them
// (State.resolve(0)==nullptr, State.encode(nullptr)==0), never truncating wasm64 pointers.
namespace storm_b144a {
struct Ctx {
    storm25220::State* st{};
    void* (*alloc)(uint32_t size, const char* file, int32_t line){};  // 0x1C580
    void (*free)(void* p, const char* file, int32_t line){};           // 0x1B960
    void (*reportError)(void* errCb, uint32_t code){};                 // 0x24CD0 (code = EDI, unrecovered caller reg)
    uint32_t (*lastError)(){};
    int (*parse)(Ctx&, char** cursor, int32_t* st, uint32_t* a, void* b, void* errCb){}; // 0x257F0
    uint32_t (*callCb)(uint32_t cb, const uint32_t* args, uint32_t n){}; // 0x34 callback / net callback
    void* (*newEvent)(){};                                             // 0x5170 on list 0x45A18
    void (*setPalette)(uint32_t, uint32_t, const void*, uint32_t){};   // 0x19520
    void (*regLoad)(const char*, const char*, uint32_t, uint32_t*){};  // 0x2C1D0
    uint8_t* searchHead{};  // EAX of 0x24DF0 callers: not recoverable from the decompile, caller supplies it
    const char* separators{"="}; // DAT_6fc37128 contents NOT verified
    uint32_t debugMemory{}, protectMemory{};
};
void trimBlock(Ctx&, uint8_t* heap, uint8_t* blk, int32_t* off, int32_t* total);
bool loadFile(Ctx&, const char* path, int32_t* a, uint32_t* b, void* c, void* errCb);
bool applyArg(Ctx&, uint8_t* rec, const char* str, int32_t* consumed);
bool applyChain(Ctx&, const char* str, int32_t* st, int32_t* maxConsumed);
bool parseOption(Ctx&, const char* str, int32_t* st, void* errCb);
void setBool(uint8_t* rec, const char* str, int32_t* consumed);
void setNumber(Ctx&, uint8_t* rec, const char* str, int32_t* consumed);
void setString(Ctx&, uint8_t* rec, const char* str, int32_t* consumed);
uint8_t* findLongest(Ctx&, uint8_t* head, int32_t minLen, const char* str);
bool netQuery(Ctx&, uint32_t cb, uint32_t arg, uint32_t* out2, void** buf);
void postEvent(Ctx&, uint32_t a, uint32_t b, const void* data, uint32_t len);
void initMemory(Ctx&);
}
