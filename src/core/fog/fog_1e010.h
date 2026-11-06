// src/core/fog/fog_1e010.h -- the seams of the async request destroy (src/core/fog/fog_1e010.cpp).
// Retail calls each seam through its own convention; the seam takes the register and stack values as plain words.
#pragma once
#include <cstdint>

struct FogReqSeams {
    // 1DC40, fastcall ECX = req; nonzero = the request is done (no cancel, wait or completion).
    uint32_t (*poll)(uint32_t req);
    // Ordinal 283 (Storm async cancel), stdcall 1 word: the buffer.
    void (*cancel)(uint32_t buf);
    // WaitForSingleObject(handle, INFINITE).
    void (*waitEvent)(uint32_t handle);
    // 1D740, EAX = req; ret 0.
    void (*complete)(uint32_t req);
    // 1B0C0, EAX = ctx, ECX = &p (p is the stack word the callee reads), stack words = file, line; ret 8.
    void (*poolFree)(uint32_t ctx, uint32_t p, uint32_t file, uint32_t line);
    // the retail exit paths (_exit(-1) after FogError); never returns.
    void (*fatal)(uint32_t code);
};

// fastcall(req) with ECX = req; void. listHead = the deferred list head global (DAT 9af64, a link cell),
// poolEnabled = the module flag at DAT 7f350 (the pool is passed to poolFree only when it is nonzero).
void fog_req_destroy_1e010(uint8_t* req, uint32_t* listHead, uint32_t poolEnabled, const FogReqSeams* s);
