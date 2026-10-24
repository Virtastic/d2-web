// Storm n95 batch. Retail structs hold 32-bit addresses; fields are uint32_t and cast through uintptr_t.
#include <cstdint>
static inline uint32_t* W(void* b, uint32_t o) { return (uint32_t*)((uint8_t*)b + o); }

// 1.13c STORM+0x22240 -- Huffman tree constructor. nodeCtor runs the 0x203 x 0x18 vector constructor at +8 (SEH frame dropped).
void* Storm_N95_22240(void* st, void (*nodeCtor)(void* nodes, uint32_t size, uint32_t count)) {
    nodeCtor(W(st, 8), 0x18, 0x203);
    uint32_t a = (uint32_t)(uintptr_t)W(st, 0x3054), b = (uint32_t)(uintptr_t)W(st, 0x3060);
    *W(st, 0x3054) = a; *W(st, 0x3058) = ~a; *W(st, 0x3050) = 0;
    *W(st, 0x3060) = b; *W(st, 0x3064) = ~b; *W(st, 0x305C) = 0;
    *W(st, 4) = 1; *W(st, 0x3068) = 0;
    return st;
}

// 1.13c STORM+0x2B020 -- append bytes of a little-endian dword buffer to a byte vector, dropping a trailing zero byte.
// out {cap, size, data, growFlag}; in {+4 end, +8 data, +0x10 start}. grow = Storm+0x3D40 seam (reserve size+1).
void Storm_N95_2B020(uint32_t* out, uint8_t* in, void (*grow)(uint32_t* out, uint32_t need)) {
    uint32_t end = *W(in, 4), start = *W(in, 0x10), n = end - start, i = 0;
    if (n & 0x3fffffff) do {
        uint32_t idx = start + (i >> 2), v = idx < end ? ((uint32_t*)(uintptr_t)*W(in, 8))[idx] : 0;
        v >>= (i & 3) * 8;
        if (v != 0 || (i >> 2) + 1 < n) {
            uint32_t need = out[1] + 1;
            if (out[0] < need) { if (out[3] == 0 && need >= 0x100) out[3] = 0x100; grow(out, need); }
            ((uint8_t*)(uintptr_t)out[2])[out[1]++] = (uint8_t)v;
        }
        end = *W(in, 4); start = *W(in, 0x10); n = end - start; i++;
    } while (i < n * 4);
}

// Storm+0x25710 callees are seams: 25290, 255A0, 25520, 253C0, 24CD0.
struct Storm_N95_25710_Env {
    uint32_t (*at)(int* st, uint32_t* cur, void* a, void* b);   // 0x25290
    uint32_t (*dash)(uint32_t* s1, int* st, void* b);           // 0x255A0
    uint32_t (*lit)(const char* s, int* arg);                   // 0x25520
    int (*match)(void* cur, int* arg);                          // 0x253C0
    void (*err)(void* b);                                       // 0x24CD0
    const char* dashSet;                                        // DAT_6fc37118, NUL terminated, first char '-'
};
// 1.13c STORM+0x25710 -- parse one token: '@' -> at; a dashSet char -> dash; else literal / match / fallback.
uint32_t Storm_N95_25710(const Storm_N95_25710_Env* e, const char* s, uint32_t* cur, int* st, int arg, void* a, void* b) {
    if (*s == '@' && arg == 0) return e->at(st, cur, a, b);
    for (const char* p = e->dashSet; *p; p++)
        if (*p == *s && arg == 0) { *st = 0; return e->dash((uint32_t*)(s + 1), st, b); }
        else if (*p == *s) break;
    if (*st != 0) return e->lit(s, &arg);
    if (*cur == 0) {
        if (a) return ((uint32_t(*)())a)();
        if (b) e->err(b);
    } else if (e->match((void*)(uintptr_t)*cur, &arg)) {
        uint32_t nx = *W((void*)(uintptr_t)*cur, 4);
        *cur = nx & (((int32_t)nx < 1) - 1);
        return 1;
    }
    return 0;
}
