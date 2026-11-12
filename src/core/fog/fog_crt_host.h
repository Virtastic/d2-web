// Host seams of the statically linked MSVCRT low-level I/O in Fog (_read_lk at 0x06CE2, _write_lk at 0x0701C): the ioinfo slot of a
// handle, the kernel32 file calls and the errno setters, in the order retail makes them. The engine binds them to its own file layer;
// the FN probes bind them to the recorded imports.
#pragma once
#include <cstdint>

namespace fogcrt {
// ioinfo slot (0x24 bytes in retail): OS handle (+0), flags (+4), lookahead char (+5).
struct IoInfo { void* handle; uint8_t flags; char peek; };
enum { FEOFLAG = 0x02, FCRLF = 0x04, FPIPE = 0x08, FAPPEND = 0x20, FDEV = 0x40, FTEXT = 0x80 };

struct Host {
    IoInfo* (*slot)(uint32_t fh);
    int (*readFile)(void* h, void* buf, uint32_t n, uint32_t* got);          // nonzero = ok
    int (*writeFile)(void* h, const void* buf, uint32_t n, uint32_t* put);   // nonzero = ok
    uint32_t (*lastError)();
    int* (*errnoPtr)();               // 0x6A5A
    uint32_t* (*doserrnoPtr)();       // 0x6A63
    void (*dosmaperr)(uint32_t err);  // 0x6A6C
    void (*lseek)(uint32_t fh, int32_t lo, int32_t hi, int32_t origin);   // __lseeki64_lk (0x0A0DB)
};
extern Host host;   // defined with _read_lk (fog_sub_6ce2.cpp)
inline void bind(const Host& h) { host = h; }
}

int fog_readLk(uint32_t fh, char* buf, uint32_t cnt);
int fog_writeLk(uint32_t fh, const char* buf, uint32_t cnt);
