// D2Client automap cell-tree walks. Node = retail layout {i32 kind; i16 a,b; i32 c; Node* child(+0xC); Node* next(+0x10)}; links are native pointers here.
#include <cstdint>
#include <emscripten.h>
struct D2cAmNode { int32_t kind; int16_t a, b; int32_t c; D2cAmNode* child; D2cAmNode* next; };
extern "C" {
// 1.13c D2Client+0x5F360  (ECX = node, cursor = DAT 1CC1B4 passed by reference)
EMSCRIPTEN_KEEPALIVE void d2c_5f360(D2cAmNode* n, int16_t** cursor) {
    for (; n; n = n->next) {
        d2c_5f360(n->child, cursor);
        if (n->kind == 0) { (*cursor)[0] = n->a; (*cursor)[1] = n->b; (*cursor)[2] = (int16_t)n->c; *cursor += 3; }
    }
}
// 1.13c D2Client+0x5F3B0  (ECX = node, EDX = byte counter)
EMSCRIPTEN_KEEPALIVE void d2c_5f3b0(D2cAmNode* n, int32_t* bytes) {
    for (; n; n = n->next) {
        d2c_5f3b0(n->child, bytes);
        if (n->kind == 0) *bytes += 6;
    }
}
}
