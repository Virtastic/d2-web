// 1.13c D2Client+0x7C690: build the credits/text scroll object from a wide blob (callers 0x7D3C0, 0x7D200).
// 1.13c D2Client+0x7BF60 (inlined here, edi = the object): lay out width/height from the line list.
#include <cstdint>
#include <emscripten.h>
#include <cstddef>
struct D2c7C690Node { uint32_t str; uint32_t next; };  // retail 32-bit pointers
struct D2c7C690Obj {
    uint32_t head;       // +0x00 (32-bit ptr)
    int32_t count;       // +0x04
    int32_t y;           // +0x08
    int32_t x;           // +0x0c (= rate after layout)
    uint16_t w;          // +0x10
    uint16_t h;          // +0x12
    uint16_t rate;       // +0x14
    uint8_t pad[0x2c - 0x16];
};
static_assert(offsetof(D2c7C690Obj, rate) == 0x14 && sizeof(D2c7C690Obj) == 0x2c, "layout");
struct D2c7C690Env {
    void* (*alloc)(uint32_t size, int line);                                   // Ordinal_10042(0x659,0)
    D2c7C690Node* (*split)(const uint16_t*, int32_t* count, uint16_t* rate);    // 0x7BFB0
    int32_t (*lineWidth)(const uint16_t* s);                                    // Ordinal_10028
    int32_t (*setFont)(int32_t id);                                             // Ordinal_10184 (state, ignored)
    int32_t (*mode3Head)(int32_t);                                              // Ordinal_10059(0xb4), mode==3 path
};
static void layout(D2c7C690Obj* o, const D2c7C690Env* e) {
    int32_t mx = 0;
    o->y = 0;
    o->x = o->rate;
    for (D2c7C690Node* n = (D2c7C690Node*)(uintptr_t)o->head; n; n = (D2c7C690Node*)(uintptr_t)n->next) { int32_t w = e->lineWidth((const uint16_t*)(uintptr_t)n->str); if (w > mx) mx = w; }
    o->w = (uint16_t)(mx + 0x14);
    o->h = (uint16_t)(o->count * 0xf + 6);
}
extern "C" {
// mode = the stack slot the decompile shows as unaff_retaddr; 3 skips the text split.
EMSCRIPTEN_KEEPALIVE D2c7C690Obj* d2c_7c690(const uint16_t* text, int32_t mode, const D2c7C690Env* e) {
    D2c7C690Obj* o = (D2c7C690Obj*)e->alloc(sizeof(D2c7C690Obj), 0x659);
    if (mode == 3) {
        o->head = (uint32_t)e->mode3Head(0xb4);
        layout(o, e);
        return o;
    }
    e->setFont(0);
    o->head = (uint32_t)(uintptr_t)e->split(text, &o->count, &o->rate);
    o->w = 0x145; o->h = 0x70;
    for (int i = 0x1a; i < 0x2a; i += 4) *(uint32_t*)((uint8_t*)o + i) = 0;
    *(uint16_t*)((uint8_t*)o + 0x2a) = 0;
    e->setFont(0);
    layout(o, e);
    return o;
}
}
