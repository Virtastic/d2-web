// D2Multi _output (0x2334) environment: the port's callbacks; shared by the port and the FN probe.
#pragma once
#include <cstdint>

struct d2multi_out_env {
    void* ctx; int* count;                              // stream and running count (local_228)
    void (*write_char)(d2multi_out_env*, int ch);
    void (*write_multi_char)(d2multi_out_env*, int ch, int n);
    void (*write_string)(d2multi_out_env*, const char* p, int n);
    void* (*addr)(uint32_t);                            // retail 32-bit address -> host pointer
    int (*wctomb)(char*, uint16_t);
    void (*cfltcvt)(const void* dbl8, char* buf, int fmt, int prec, int caps);   // 0x9860
    void (*cropzeros)(char*); void (*forcdecpt)(char*); // 0x9864, 0x986c
    const uint8_t* mbctype;                             // PTR_DAT_6f9e9488
    const char* nullStr; const uint16_t* nullWStr;      // "(null)" / L"(null)"
    uint32_t (*udivrem)(uint64_t* val, uint32_t base);  // __aulldvrm; returns remainder, val updated
};

int d2multi_output(d2multi_out_env* e, const uint8_t* fmt, const uint32_t* args);
