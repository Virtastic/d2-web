// Seam struct of the Storm big-number wrappers in storm_batch_2f0e0.cpp, for the FN capture probes.
#pragma once
#include <cstdint>
#include "storm_big_types.h"
struct StormBatch2f0e0Env {
    int32_t (*f_2ebb0)(Storm2fVector*, Storm2fScratch*); // EAX scratch
    void (*f_2d830)(Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fScratch*);
    void (*f_2c840)(Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fScratch*);
    void* (*allocate)(uint32_t,const char*,uint32_t,uint32_t);
    Storm2fBig* (*construct)(void*);
    void (*f_2e930)(Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fVector*,Storm2fScratch*); // EAX fourth, EBX scratch
    void (*f_2c210)(Storm2fVector*,uint32_t,uint32_t); // EAX third
    void (*f_2ba20)(Storm2fVector*,uint32_t,uint32_t); // EAX first
    void (*f_2b020)(Storm2fBytes*,Storm2fVector*);
    void (*free_memory)(void*,const char*,int32_t);
};

int32_t Storm_2F0E0(Storm2fBig* p, StormBatch2f0e0Env& e);
void Storm_2F0C0(Storm2fBig* p, Storm2fBig* a, Storm2fBig* b, Storm2fBig* c, StormBatch2f0e0Env& e);
