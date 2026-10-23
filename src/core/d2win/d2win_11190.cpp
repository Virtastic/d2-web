// src/core/d2win/d2win_11190.cpp -- 1.13c D2Win+0x11190 (ord 10017, 157 B), from the cached decompile.
#include <cstdint>
#include <cstdlib>

struct D2Win11190Env {
    void (*o512)(void*, uint32_t, uint32_t); void (*o513)(void*, uint32_t, uint32_t);
    void (*o514)(void*, uint32_t, uint32_t); void (*o515)(void*, uint32_t, uint32_t);
    void (*fatal)(uint32_t code);
};

// 1.13c D2Win+0x11190
void D2Win_11190(void* w, int32_t kind, uint32_t msg, uint32_t arg, const D2Win11190Env& e) {
    uint32_t code;
    if (w) {
        switch (kind) {
        case 0: goto kind0;
        case 1: e.o512(w, msg, arg); return;
        case 2: e.o514(w, msg, arg); return;
        case 3: e.o513(w, msg, arg); return;
        default: return;
        }
    }
    code = 0x30;
    goto fatal;
kind0:
    if (msg == 0x111) code = 0x34;
    else if (msg == 0x101) code = 0x35;
    else if (msg == 0x100) code = 0x36;
    else { e.o515(w, msg, arg); return; }
fatal:
    e.fatal(code);
    std::exit(-1);
}
