// src/core/d2win/d2win_batch_b54a.cpp -- 1.13c D2Win small functions (alpha batch lane, no spec).
// Transcribed from cached decompiles; retail globals/callees arrive via seams. Register-passed
// args (EAX/ESI/EDI/EBX) are explicit parameters. Skipped: 0x35E0 (__chkstk, compiler), 0xC810,
// 0xCBA0, 0x7B30 (Win32 drive scan), 0xC590, 0xD790 (unresolved register args).
#include <cstdint>
#include <emscripten.h>

struct D2WinB54aEnv {
    void (*fatal)(uint32_t stringId);      // Ordinal_10265 + Ordinal_10024("") then _exit(-1)
    int32_t (*sub_10105)(void);            // Ordinal_10105
    void (*sub_276)(void);                 // Ordinal_276
    const uint8_t* tab6f9a8ca8;            // 0xE4-byte records
    const uint8_t* tab6f8fc5f8;            // 0x41 rows x 0x40 bytes
    uint8_t* tab6f96db68;                  // 0x100-byte cells
    int32_t (*maxLineWidth)(const uint16_t*);   // 0x11FB0
    int32_t (*textBlockHeight)(const uint16_t*);// 0x11E80
    void (*ustrcpy)(uint16_t* dst, const uint16_t* src); // Unicode::strcpy
};

// 1.13c D2Win+0x7A60 -- esiNull selects the fatal string (0x59 / 0x5F); returns if #10105 nonzero.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_7a60(int32_t esi, const D2WinB54aEnv* e) {
    uint32_t id = 0x59;
    if (esi != 0) {
        if (e->sub_10105() != 0) return;
        e->sub_276();
        id = 0x5f;
    }
    e->fatal(id);
}

// 1.13c D2Win+0x0b4f0 (a.k.a. 0xB4F0) -- is `target` in the tree rooted at node `root` (children shorts at +4/+6)?
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_b4f0(int32_t root, int32_t target, const D2WinB54aEnv* e) {
    int32_t st[64];
    if (target == 0) return 1;
    if (root == 0) return 0;
    st[0] = root;
    int32_t n = 1;
    for (;;) {
        if (target == st[n - 1]) return 1;
        const uint8_t* r = e->tab6f9a8ca8 + (uint32_t)st[n - 1] * 0xe4;
        int16_t l = *(const int16_t*)(r + 4), rt = *(const int16_t*)(r + 6);
        int32_t m = n - 1;
        if (l != 0) {
            st[n - 1] = l;
            m = n;
            if (rt != 0) { st[n] = rt; m = n + 1; }
        }
        if (m > 0x3f) { e->fatal(0xb47); return 0; }
        n = m;
        if (m == 0) return 0;
    }
}

// 1.13c D2Win+0x0b7b0 -- validate table[row(mask)][idx] <= 0x40; idx = (uint8)a.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_b7b0(uint32_t a, uint32_t mask, const D2WinB54aEnv* e) {
    if ((uint8_t)a >= 0x40) { e->fatal(0x56b); return; }
    uint32_t row = 0;
    if (mask) { while (!((mask >> row) & 1)) row++; row++; }
    if (e->tab6f8fc5f8[(a & 0xff) + row * 0x40] > 0x40) e->fatal(0x56d);
}

// 1.13c D2Win+0xB810 -- cell pointer for packed (group<<5 | slot), or null.
extern "C" EMSCRIPTEN_KEEPALIVE
uint8_t* d2win_sub_b810(uint32_t v, const D2WinB54aEnv* e) {
    int32_t g = (int32_t)v >> 5;
    uint32_t s = v & 0x1f;
    if (s < 0x15) {
        if (g == 0) return e->tab6f96db68 + (s + 0x348) * 0x100;
        if (g >= 0 && g < 9 && (g < 3 || g > 4)) return e->tab6f96db68 + (g * 0x69 + s) * 0x100;
    }
    return 0;
}

// 1.13c D2Win+0x0b860 -- find node whose entry has +4 == key1 and +0xC == key2; list head in EAX.
struct D2WinB54aEntry { int32_t pad0; int32_t k1; int32_t pad8; int32_t k2; };
struct D2WinB54aNode { D2WinB54aEntry* entry; D2WinB54aNode* next; };
extern "C" EMSCRIPTEN_KEEPALIVE
D2WinB54aNode* d2win_sub_b860(D2WinB54aNode** head, int32_t key1, int32_t key2, const D2WinB54aEnv* e) {
    if (!head) { e->fatal(0x4a5); return 0; }
    for (D2WinB54aNode* n = *head; n; n = n->next)
        if (n->entry->k1 == key1 && n->entry->k2 == key2) return n;
    return 0;
}

// 1.13c D2Win+0x0c980 -- append a text row (0x208-byte stride; width at +0x400, height at +0x408).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_c980(uint8_t* base, const uint16_t* text, int32_t* totalH, int32_t* maxW, int32_t* count, const D2WinB54aEnv* e) {
    uint8_t* row = base + *count * 0x208;
    if (*count > 6) { e->fatal(0x72d); return; }
    e->ustrcpy((uint16_t*)row, text);
    int32_t w = e->maxLineWidth((const uint16_t*)row);
    int32_t h = e->textBlockHeight((const uint16_t*)row);
    *(int32_t*)(row + 0x400) = w;
    *(int32_t*)(row + 0x408) = h;
    *maxW = (w + 6 < *maxW) ? *maxW : w + 6;
    *totalH += h;
    *count += 1;
}
