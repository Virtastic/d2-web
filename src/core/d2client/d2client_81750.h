// Globals and callees of the D2Client 0x81750 weather/overlay init (port: d2client_81750.cpp).
#pragma once
#include <cstdint>
struct D2c81750G {
    int32_t* once;    // +0x123444: 0x80BE0 has run
    uint8_t* blk;     // +0x113880 .. +0x11394C (0xCC bytes): the weather tables, indexed by RVA - 0x113880
};
struct D2c81750Env {
    void (*f814f0)(uint32_t ecx);                                   // 0x814F0, ECX = 1
    void (*f80be0)();                                               // 0x80BE0
    uint32_t (*ord10459)(int32_t act);                              // D2Common 10459 (thunk 0xC1EA), the low byte is the answer
    uint32_t (*ord10190)(uint32_t r, uint32_t g, uint32_t b);       // 10190 (thunk 0xD390): palette index of an RGB, low byte
    void (*wsprintfA)(char* buf, const char* fmt, const char* arg); // user32 wsprintfA (IAT 0xCF114)
    uint32_t (*fdb380)(const char* path, uint8_t* local, uint32_t zero);  // 0x2B380: EAX = path, stack (&local, 0), ret 8
    uint32_t (*ord10086)(uint32_t h);                               // 10086 (thunk 0xC0C4)
};
// The format strings of 0x81750 (retail .rdata 0xD4AD8.. and 0xD357C), in call order: Rain1..4, bubble1..4, then the %s argument.
extern const char* const d2c81750Fmt[8];
extern const char d2c81750Dir[];
