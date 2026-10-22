// 1.13c D2Client+0x7BFB0: split a wide credits/text blob into a singly linked list of lines.
// Line 0 is the scroll rate (atol of the narrowed text, 8 if any char > 0x7f); lines 1.. become nodes.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>
struct D2c7BFB0Node { uint16_t* str; D2c7BFB0Node* next; };
struct D2c7BFB0Env {
    void* (*alloc)(uint32_t size);  // Ordinal_10042 (Fog alloc); node = 8 bytes, string = (len+1)*2
};
// Reads one line of <=100 wchars (no NUL check, as retail); consumes the '\n' when under 100.
static const uint16_t* readLine(const uint16_t* p, uint16_t* buf) {
    int i = 0;
    for (; i < 100; i++) {
        if (*p == 10) break;
        buf[i] = *p++;
    }
    if (i < 100 && *p == 10) p++;
    return p;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE D2c7BFB0Node* d2c_7bfb0(const uint16_t* p, int32_t* count, int16_t* rate, const D2c7BFB0Env* e) {
    D2c7BFB0Node *head = 0, *tail = 0;
    *count = 0;
    bool first = true;
    while (*p) {
        uint16_t buf[101] = {0};
        p = readLine(p, buf);
        if (first) {
            first = false;
            bool ascii = true; char a[101]; int n = 0;
            for (; buf[n]; n++) { if (buf[n] > 0x7f) ascii = false; a[n] = (char)buf[n]; }
            a[n] = 0;
            *rate = ascii ? (int16_t)atol(a) : 8;
            for (int k = 0; k < 101; k++) buf[k] = 0;
            p = readLine(p, buf);  // line 1 read into the same buffer, then falls through to node build
        }
        int n = 0; while (buf[n]) n++;
        D2c7BFB0Node* nd = (D2c7BFB0Node*)e->alloc(8);
        nd->str = (uint16_t*)e->alloc((n + 1) * 2);
        for (int k = 0; k <= n; k++) nd->str[k] = buf[k];
        nd->next = 0;
        if (tail) tail->next = nd; else head = nd;
        tail = nd;
        (*count)++;
    }
    return head;
}
}
