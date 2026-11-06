// src/core/fog/fog_1e620.h -- the seams of the D2QSSocket frame splitter (src/core/fog/fog_1e620.cpp).
// Retail calls each one through its own convention; the seam takes the register and stack values as plain words.
#pragma once
#include <cstdint>

struct FogFrameSeams {
    // the callee at sock+0x168: ECX = frame base (src when sock+0x11C > 0, else the payload pkt), EDX = frame length
    // (src/pkt: the full length, or length - min for the pkt case), stack word 0 = sock+0x16C; ret 4. Nonzero = consumed.
    uint32_t (*handler)(uint32_t ecx, uint32_t edx, uint32_t ctx);
    // Fog+0x17A50: EAX = count, stack = (sock+0x170, frame base); ret 8.
    void (*queue)(uint32_t count, uint32_t q, uint32_t p);
    // IsBadCodePtr(handler): nonzero = fatal 0x101.
    int (*isBadCodePtr)(uint32_t p);
    // SetEvent(sock+0x114).
    void (*setEvent)(uint32_t h);
    // the retail fatal sink (0x10F30 / exit paths); never returns.
    void (*fatal)(uint32_t code);
};

// stdcall(dst, total) with EBX = sock (register input). Returns the bytes carried over to the buffer start (EAX).
int fog_qssocket_frames_1e620(uint8_t* sock, uint8_t* dst, int total, const FogFrameSeams* s);
