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
    void (*reportError)(void* errCb, uint32_t code, uint32_t ebx){};    // 0x24CD0 (EDI = code, EBX = the caller's string address, stack = errCb)
    uint32_t (*lastError)(){};
    // Win32 file seams (CreateFileA 0x80000000/share 1/OPEN_EXISTING, GetFileSize, ReadFile, CloseHandle at the Storm IAT); null = the host's stdio
    intptr_t (*fileOpen)(const char* path){};
    uint32_t (*fileSize)(intptr_t h){};
    void (*fileRead)(intptr_t h, void* buf, uint32_t n, uint32_t* got){};
    void (*fileClose)(intptr_t h){};
    // 0x25710 callee seams (null = the real b144a function below, or a direct call of the callback)
    bool (*load)(Ctx&, const char* path, int32_t* st, uint32_t* cur, void* cb, void* errCb){};   // 0x25290
    bool (*option)(Ctx&, const char* str, int32_t* st, void* errCb){};                           // 0x255A0
    bool (*chain)(Ctx&, const char* str, int32_t* st, int32_t* used){};                          // 0x25520
    bool (*arg)(Ctx&, uint8_t* rec, const char* str, int32_t* used){};                           // 0x253C0
    uint32_t (*callTok)(void* cb, const char* tok){};                                            // the token callback (stdcall, one word)
    const char* switches{"-/"};                                                                  // Storm+0x47118
    int (*parse)(Ctx&, char** cursor, int32_t* st, uint32_t* a, void* b, void* errCb){}; // 0x257F0
    uint32_t (*callCb)(uint32_t cb, const uint32_t* args, uint32_t n){}; // net callback
    uint32_t (*callOpt)(uint32_t cb, const uint32_t* frame, const char* str){}; // rec+0x34 option callback (stdcall: &frame[7], str)
    void* (*newEvent)(){};                                             // 0x5170 on list 0x45A18
    void (*setPalette)(uint32_t, uint32_t, const void*, uint32_t){};   // 0x19520
    void (*regLoad)(const char*, const char*, uint32_t, uint32_t*){};  // 0x2C1D0
    uint8_t* searchHead{};  // EAX of 0x24DF0 callers: not recoverable from the decompile, caller supplies it
    const char* separators{"=:"}; // Storm+0x47128
    uint32_t debugMemory{}, protectMemory{};
};
void trimBlock(Ctx&, uint8_t* heap, uint8_t* blk, int32_t* off, int32_t* total);
bool loadFile(Ctx&, const char* path, int32_t* a, uint32_t* b, void* c, void* errCb);
bool applyArg(Ctx&, uint8_t* rec, const char* str, int32_t* consumed);
bool applyChain(Ctx&, const char* str, int32_t* st, int32_t* maxConsumed);
bool parseOption(Ctx&, const char* str, int32_t* st, void* errCb);
uint32_t handleToken(Ctx&, const char* tok, uint32_t* cur, int32_t* st, uint32_t quoted, void* cb, void* errCb); // 0x25710
bool tokenize(Ctx&, char** cursor, int32_t* st, uint32_t* cur, void* cb, void* errCb);                     // 0x257F0
void setBool(uint8_t* rec, const char* str, int32_t* consumed);
void setNumber(Ctx&, uint8_t* rec, const char* str, int32_t* consumed);
void setString(Ctx&, uint8_t* rec, const char* str, int32_t* consumed);
uint8_t* findLongest(Ctx&, uint8_t* head, int32_t minLen, const char* str);
bool netQuery(Ctx&, uint32_t cb, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t* out2, void** buf);
void postEvent(Ctx&, uint32_t a, uint32_t b, const void* data, uint32_t len);
void initMemory(Ctx&);
}
