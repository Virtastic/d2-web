// src/core/d2win/d2win_revive1.cpp -- revived D2Win rows of 1.13c D2Win.dll, transcribed from the cached
// Ghidra decompiles of the 1.13c D2Win.dll. Specs are kept in the private store.
// Guest objects are host memory with the 1.13c field layout; register-passed arguments are explicit
// parameters; callees outside this file come in through the env struct, one member per retail call.
#include <cstdint>
#include <cstdlib>

namespace {
size_t strlen16(const uint16_t* s) { size_t n = 0; while (s[n]) n++; return n; }
}

// point test on a control. EAX = control, ESI = x, EDI = y. Returns 1 when
// x >= +0x0C, x < +0x0C + +0x14, y < +0x10 and y >= +0x10 - +0x18 (signed compares); else 0.
// 1.13c D2Win+0x0F5B0
int32_t d2win_f5b0(const int32_t* o, int32_t x, int32_t y) {
    if (x < o[3]) return 0;
    if ((int32_t)((uint32_t)o[4] - (uint32_t)o[6]) > y) return 0;
    if (!(x < (int32_t)((uint32_t)o[5] + (uint32_t)o[3]))) return 0;
    return y < o[4] ? 1 : 0;
}

struct D2WinF6E0Env {
    uint32_t (*textId)(uint32_t id);                 // Ordinal 10265
    void (*report)(const char* file, uint32_t text); // Ordinal 10024, then _exit(-1)
    void (*cb9)(uint32_t* cell);                     // control +0x24, called with the cell pointer
    void (*cb10)(uint32_t* slot);                    // control +0x28, called with a stack slot
    int32_t* depth;                                  // DAT_6f9014a8, nesting counter around cb10
};

// __fastcall(ECX = cell). The cell holds a control pointer whose first dword must be 6,
// else text 0xBC, report and exit(-1). cb9 runs when +0x24 is set (and +0x4C := 1); cb10 runs when +0x28 is set,
// with the depth counter raised for the call.
// 1.13c D2Win+0x0F6E0
void d2win_f6e0(uint32_t* cell, const D2WinF6E0Env* e) {
    uint32_t* ctl = (uint32_t*)(uintptr_t)cell[0];
    if (ctl[0] != 6) {
        e->report("", e->textId(0xBC));
        std::_Exit(-1);
    }
    if (ctl[9] != 0) {
        ctl[0x13] = 1;
        e->cb9(cell);
        if (ctl[10] != 0) {
            uint32_t slot = 0;
            *e->depth += 1;
            e->cb10(&slot);
            *e->depth -= 1;
        }
    }
}

struct D2WinGlyphEnv {
    const uint8_t* (*glyph)(uint16_t ch);            // DAT_6f901508: glyph record, advance width at +3
};

// word-wrap cut. Walks the UTF-16 string; a line is the prefix [0..k] whose advance widths
// (LF not counted) reach maxWidth. Returns k and writes the terminator at s[k]. Empty string: s[0] := 0, returns 0.
// 1.13c D2Win+0x08790
int32_t d2win_8790(uint16_t* s, int32_t maxWidth, const D2WinGlyphEnv* e) {
    int32_t len = (int32_t)strlen16(s);
    if (len < 1) { s[0] = 0; return 0; }
    int32_t k = 0;
    for (;;) {
        int32_t w = 0;
        if (s[0] != 0) {
            int32_t j = 0;
            const uint16_t* q = s;
            do {
                if (k + 1 <= j) break;
                if (*q != 10) w += e->glyph(*q)[3];
                q++; j++;
            } while (*q != 0);
        }
        if (maxWidth <= w) { s[k] = 0; return k; }
        k++;
        if (len <= k) { s[k] = 0; return k; }
    }
}

struct D2WinTile10280Env {
    // D2Gfx ordinal 10079 through thunk 0x7852, stdcall 5 words (the decompile shows only three on the equal-size path)
    void (*draw5)(uint32_t surface, int32_t x, int32_t y, const int32_t* clip, int32_t m);
};

// tile the source rect over the destination rect. src = {left, right, top, bottom}, dst in
// EAX the same layout. Equal sizes: one draw5(surface, dst.left - src.left, dst.bottom - src.bottom + 0x100, dst, 5)
// (the disassembly passes the destination rect itself as the clip).
// Otherwise ceil-division columns x rows, each cell clipped to dst and drawn by draw5(surface, x, y, clip, 5).
// 1.13c D2Win+0x10280
void d2win_10280(const int32_t* src, uint32_t surface, const int32_t* dst, const D2WinTile10280Env* e) {
    int32_t sh = src[3] - src[2];
    int32_t sw = src[1] - src[0];
    int32_t dw = dst[1] - dst[0];
    int32_t dh = dst[3] - dst[2];
    if (sw == dw && sh == dh) {
        e->draw5(surface, dst[0] - src[0], (dst[3] - src[3]) + 0x100, dst, 5);
        return;
    }
    int32_t cols = (dw - 1 + sw) / sw;
    int32_t rows = (dh - 1 + sh) / sh;
    int32_t y = (dst[2] - src[2]) + 0x100;
    for (int32_t r = rows; r > 0; r--) {
        int32_t x = dst[0] - src[0];
        for (int32_t c = cols; c > 0; c--) {
            int32_t clip[4];
            clip[0] = src[0] + x;
            if (src[0] + x < dst[0]) clip[0] = dst[0];
            clip[1] = dst[1];
            if (clip[0] + sw <= dst[1]) clip[1] = clip[0] + sw;
            clip[2] = src[2] - 0x100 + y;
            if (clip[2] < dst[2]) clip[2] = dst[2];
            clip[3] = dst[3];
            if (clip[2] + sh <= dst[3]) clip[3] = clip[2] + sh;
            e->draw5(surface, x, y, clip, 5);
            x += sw;
        }
        y += sh;
    }
}

struct D2WinMpqEnv {
    // FUN_6F8E7E60: archive open, EAX = priority, stdcall (caller tag, file, label, 0, 0), ret 0x14; a 32-bit handle or 0
    uint32_t (*open)(uint32_t prio, const char* who, const char* file, const char* tag, uint32_t a3, uint32_t a4);
    int32_t (*ord10117)(void);                                         // thunk 0x76EA (IAT 0x1B158): already-open probe
    int32_t (*ord10227)(void);                                         // thunk 0x76F6 (IAT 0x1B150): expansion-check probe
};

struct D2WinArchives {                // the seven archive handles (32-bit retail words), in open order
    uint32_t data, sfx, speech, delta, kfixup, patch, exp;   // 0xC9CCC 0xC9CB8 0xC9CD4 0xC9CBC 0xC9CD0 0xC9CE0 0xC9CE4
};

// export 10086. Opens D2DATA, D2SFX, D2SPEECH, D2DELTA, D2KOREANFIXUP (priority 1000), PATCH_D2 (5000), D2EXPANSION (3000),
// each with the "D2Win.DLL" caller tag. Returns 1 when 10117 is nonzero; with data+sfx+speech open, 1 when 10227
// is zero or the expansion archive opened; else 0.
// 1.13c D2Win+0x0AA20
int32_t d2win_aa20(D2WinArchives* a, const D2WinMpqEnv* e) {
    a->data = e->open(1000, "D2Win.DLL", "d2data.mpq", "D2DATA", 0, 0);
    a->sfx = e->open(1000, "D2Win.DLL", "d2sfx.mpq", "D2SFX", 0, 0);
    a->speech = e->open(1000, "D2Win.DLL", "d2speech.mpq", "D2SPEECH", 0, 0);
    a->delta = e->open(1000, "D2Win.DLL", "d2delta.mpq", "D2DELTA", 0, 0);
    a->kfixup = e->open(1000, "D2Win.DLL", "d2kfixup.mpq", "D2KOREANFIXUP", 0, 0);
    a->patch = e->open(5000, "D2Win.DLL", "patch_d2.mpq", "PATCH_D2", 0, 0);
    a->exp = e->open(3000, "D2Win.DLL", "d2exp.mpq", "D2EXPANSION", 0, 0);
    if (e->ord10117() != 0) return 1;
    if (a->data && a->sfx && a->speech) {
        if (e->ord10227() == 0) return 1;
        if (a->exp) return 1;
    }
    return 0;
}
