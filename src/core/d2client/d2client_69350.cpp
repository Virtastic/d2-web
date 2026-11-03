#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69350: EDX = path, stack = hash table. Zeroes a 0x104-byte local, 0x68B30 copies the
// base name of "...\name.ext" into it, 0x3D8E lower-cases it, 0x68B80 hashes it (ESI = local). Walks the chain at table+0xC+hash*4 (next = node+0)
// for node+0xC/+0x10 == the first 8 bytes of the local. Returns the node or 0.
extern "C" void d2_client_68b30(const char* path, char* out);
extern "C" char* d2_client_3d8e(char* s);
extern "C" uint32_t d2cf_68b80(uint32_t, const Env*);
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69350(uint32_t name, uint32_t table, const Env* e) {
    char loc[0x108] = {0};
    d2_client_68b30((const char*)(uintptr_t)name, loc);
    d2_client_3d8e(loc);
    uint32_t h = d2cf_68b80((uint32_t)(uintptr_t)loc, e);
    uint32_t a = *(uint32_t*)loc, b = *(uint32_t*)(loc + 4);
    for (uint32_t n = R(table, 0xC + h * 4); n; n = R(n, 0)) if (R(n, 0xC) == a && R(n, 0x10) == b) return n;
    return 0;
}
