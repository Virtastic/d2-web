#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69970: EAX = path "...\\name.ext", stack = hash table, total (+0x11C044) by pointer. Local
// = lower-cased base name (0x68B30, 0x3D8E); 0x69350(local) finds a node: found = 0. Else node = alloc 0xDC (hooked 0xBF7A), zeroed, +4 |= 1, name copied to
// +0xC, node[5] = 0xD810(path, 0xD408(&node[2])): 0 = return 0, byte [+2] > 0x40 = fatal 0xEC. Else pushed on the chain at table+0xC+0x68B80(local)*4,
// total += node[2] + 0x1C, returns the node.
extern "C" void d2_client_68b30(const char* path, char* out);
extern "C" char* d2_client_3d8e(char* s);
extern "C" uint32_t d2cf_68b80(uint32_t, const Env*);
extern "C" uint32_t d2cf_69350(uint32_t, uint32_t, const Env*);
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69970(uint32_t path, uint32_t table, uint32_t total, const Env* e) {
    char frame[0x118] = {0};
    char* loc = frame + 8;
    d2_client_68b30((const char*)(uintptr_t)path, loc);
    d2_client_3d8e(loc);
    uint32_t l = (uint32_t)(uintptr_t)loc;
    if (d2cf_69350(l, table, e)) return 0;
    uint32_t node = e->call(0xBF7A, 0, 0x1C, 0xB2000000u | kFileRva, 0, 0, 0xDC, 0);
    for (uint32_t i = 0; i < 7; ++i) W(node, i * 4) = 0;
    W(node, 4) |= 1;
    for (uint32_t i = 0;; ++i) { B(node, 0xC + i) = (uint8_t)loc[i]; if (!loc[i]) break; }
    uint32_t v = e->call(0xD408, 0, 0, 0, 0, 0, node + 8, 0);
    uint32_t f = e->call(0xD810, path, 0, 0, 0, 0, v, 0);
    W(node, 0x14) = f;
    if (!f) return 0;
    if (B(f, 2) > 0x40) { e->fatal(0xEC); return 0; }
    uint32_t h = d2cf_68b80(l, e);
    W(node, 0) = R(table, 0xC + h * 4);
    W(table, 0xC + h * 4) = node;
    W(total, 0) = R(total, 0) + R(node, 8) + 0x1C;
    return node;
}
