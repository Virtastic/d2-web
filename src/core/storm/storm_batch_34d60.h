#pragma once
#include <cstdint>
#include <cstdarg>

struct Storm34d60Log {
    uint32_t id;
    Storm34d60Log* next;
    void* file;
    uint32_t used, pending, counter, field18;
    char* buffer; // native projection of the inline retail +0x1c buffer
};
struct Storm34d60Env {
    Storm34d60Log* logs[4];
    uint32_t debug, last_error;
    void (*error)(uint32_t);
    void (*enter)(uint32_t);
    void (*leave)(uint32_t);
    void (*prepare)(Storm34d60Log*, uint32_t);
    int32_t (*vsprintf_)(char*, const char*, va_list);
    int32_t (*vsnprintf_)(char*, uint32_t, const char*, va_list);
    void (*debug_string)(const char*);
    void (*write)(void*, const void*, uint32_t, uint32_t*);
    void (*flush)(void*);
    void* (*alloc)(uint32_t, const char*, int32_t, uint32_t);
    void (*free_)(void*, const char*, int32_t, uint32_t);
    // ECX=destination, EDX=&output size; source and length are stack arguments.
    void (*decompress[4])(void*, uint32_t*, const void*, uint32_t);
    uint32_t (*hash)(const void*, uint32_t, uint32_t); // ECX=data, stack=len, EAX=seed
    void (*destroy)(void*);
    void (*insert)(void*, const void*, uint32_t); // EAX=object
    void (*append)(void*, uint32_t); // EAX=object
    void (*splice)(void*, const void*, const void*); // EAX=object
    void (*combine)(void*, void*, void*, void*, void*);
};
char* Storm_Sub3B2D0(Storm34d60Env& e,char* s,char value);
char* Storm_Sub3B280(Storm34d60Env& e,char* s,char value);
uint32_t Storm_Sub3B330(Storm34d60Env& e,const uint8_t* s);
uint32_t Storm_Sub3A870(const uint8_t* k, uint32_t len, uint32_t seed);
