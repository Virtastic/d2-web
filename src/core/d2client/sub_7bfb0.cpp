// 1.13c D2Client+0x7BFB0: split a wide credits/text blob into a singly linked list of lines.
// Line 0 is the scroll rate (atol of the narrowed text, 8 if any char >= 0x80); lines 1.. become nodes.
// Per line: the node (8 bytes, Fog alloc line 0x5B4) is allocated right after the line is read, before the
// first-line rate parse; then the text width is asked for (thunk 0xD35A, result unused); the string ((len + 1) * 2 bytes, line 0x5E7) after it. A node is two words {str, next}.
// Written over the word type W: uint32_t is retail's layout (the FN vector), uintptr_t the play build's.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

namespace {
// The frame: a 100-wchar line buffer immediately followed by the 104-byte narrow buffer the rate line is
// converted into. Neither is NUL-guarded: a 100-wchar line has no terminator, so retail's strlen/strcpy run
// on into the narrow buffer, read as wchars. That buffer is uninitialised stack in retail; it is modelled
// as zero, holding the rate text (and its NUL) once line 0 has been converted.
struct Frame {
    uint8_t m[200 + 104] = {};
    uint16_t w(int i) const { return (uint16_t)(m[2 * i] | m[2 * i + 1] << 8); }
    void clearLine() { for (int i = 0; i < 200; i++) m[i] = 0; }
    int len() const { int n = 0; while (2 * n + 1 < (int)sizeof m && w(n)) n++; return n; }
};
// Reads one line of <=100 wchars (no NUL check, as retail); consumes the '\n' when under 100.
const uint16_t* readLine(const uint16_t* p, Frame& f) {
    int i = 0;
    for (; i < 100; i++) {
        if (*p == 10) break;
        f.m[2 * i] = (uint8_t)*p; f.m[2 * i + 1] = (uint8_t)(*p >> 8); p++;
    }
    if (i < 100 && *p == 10) p++;
    return p;
}
template <class W, class Alloc, class Width>
W split(const uint16_t* p, int32_t* count, int16_t* rate, Alloc alloc, Width width) {
    W head = 0, tail = 0;
    *count = 0;
    bool first = true;
    Frame f;
    while (*p) {
        f.clearLine();
        p = readLine(p, f);
        W nd = alloc(2 * sizeof(W), 0x5B4);
        if (first) {
            first = false;
            int n = f.len();
            bool ascii = true;
            for (int k = 0; k < n; k++) if (f.w(k) >= 0x80) ascii = false;
            if (ascii) {
                // Unicode::toUtf(narrow, line, 100): one byte per ASCII unit, at most 99, then the NUL.
                int j = 0;
                for (; j < 99; j++) { uint16_t c = f.w(j); if (!c) break; f.m[200 + j] = (uint8_t)c; }
                f.m[200 + j] = 0;
                *rate = (int16_t)atol(reinterpret_cast<const char*>(f.m + 200));
            } else {
                *rate = 8;
            }
            f.clearLine();
            p = readLine(p, f);  // line 1 read into the same buffer, then the node build
        }
        int n = f.len();
        uint16_t buf[152];
        for (int k = 0; k <= n; k++) buf[k] = f.w(k);
        width(buf);  // D2Win text width (thunk 0xD35A); retail only formats a debug line when it is >= 0x134
        W s = alloc((uint32_t)(n + 1) * 2, 0x5E7);
        reinterpret_cast<W*>(static_cast<uintptr_t>(nd))[0] = s;
        uint16_t* str = reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(s));
        for (int k = 0; k <= n; k++) str[k] = buf[k];
        reinterpret_cast<W*>(static_cast<uintptr_t>(nd))[1] = 0;
        if (tail) reinterpret_cast<W*>(static_cast<uintptr_t>(tail))[1] = nd; else head = nd;
        tail = nd;
        (*count)++;
    }
    return head;
}
}  // namespace

struct D2c7BFB0Node { uint16_t* str; D2c7BFB0Node* next; };
struct D2c7BFB0Env {
    void* (*alloc)(uint32_t size, uint32_t line);  // Ordinal_10042 (Fog alloc)
};
extern "C" {
EMSCRIPTEN_KEEPALIVE D2c7BFB0Node* d2c_7bfb0(const uint16_t* p, int32_t* count, int16_t* rate, const D2c7BFB0Env* e) {
    return reinterpret_cast<D2c7BFB0Node*>(split<uintptr_t>(p, count, rate,
        [e](uint32_t size, uint32_t line) { return reinterpret_cast<uintptr_t>(e->alloc(size, line)); }, [](const uint16_t*) {}));
}
// Retail layout (32-bit words), for the FN vector.
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_7bfb0(const uint16_t* p, int32_t* count, int16_t* rate, uint32_t (*alloc)(uint32_t size, uint32_t line),
                                         void (*width)(const uint16_t*)) {
    return split<uint32_t>(p, count, rate, alloc, width);
}
}
