// src/core/gfx/d2win_batch_93c0.cpp -- twelve 1.13c D2Win.dll rows transcribed from the cached Ghidra
// decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c) and checked against the retail bytes
// (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base 0x6F8E0000). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// The fatal path the text-box/list functions share is Fog Ordinal 10265(id) -> Ordinal 10024("") ->
// _exit(-1); only the exit is observable and is d2win_fatal. Internal callees the decompiles still
// name (0x12FE0, 0x12080, 0x9200, 0x99F0, 0x1450, 0x7FC0, Fog #10042/#10043, D2Gfx #10000/#10052,
// 0x7E60, 0xA340, the Win32 message calls) are seams on D2WinBatch93C0Env, passed after the retail
// arguments, which keep their order. 32-bit pointer fields keep the retail encoding through
// (uint32_t)(uintptr_t) -- the objects live in the low wasm heap, as in the earlier batches.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win (control layouts only; no function bodies).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8(const void* p, uint32_t o)  { uint8_t  v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v)   { std::memcpy((uint8_t*)p + o, &v, 1); }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

[[noreturn]] static void d2win_fatal(uint32_t) { _exit(-1); }

// D2Lang Unicode::isWhitespace: space, or 0x09..0x0D.
static inline bool d2win_isWhitespace(uint16_t ch) { return ch == 0x20 || (ch >= 0x09 && ch <= 0x0D); }

// UTF-16 strcpy/strlen, the D2Lang imports the decompiles name.
static void u16_copy(uint16_t* dst, const uint16_t* src) { while ((*dst++ = *src++) != 0) {} }
static uint32_t u16_len(const uint16_t* s) { uint32_t n = 0; while (s[n] != 0) ++n; return n; }

// Retail source-file literals passed to FOG_Alloc/FOG_Free (read from the DLL's .rdata).
static const char kTextBoxCpp[] = "..\\Source\\D2Win\\Src\\D2WinTextBox.cpp";
static const char kFontCpp[]    = "..\\Source\\D2Win\\Src\\D2WinFont.cpp";
static const char kListCpp[]    = "..\\Source\\D2Win\\Src\\D2WinList.cpp";

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2WinBatch93C0Env {
    // 0x93C0 / 0x9730 (text box).
    void     (*setTextFont)(uint32_t fontIndex);                                    // 0x12FE0
    int32_t  (*textWidth)(const uint16_t* text, int32_t maxChars);                  // 0x12080
    int32_t  (*visibleLines)(void* box);                                            // 0x9200
    uint32_t (*splitLines)(void* box, int32_t param3, int32_t param5);              // 0x99F0
    void*    (*alloc)(uint32_t size, const char* file, int32_t line, int32_t n0);   // Fog #10042 (ECX=size, EDX=file)
    void     (*freeMemory)(void* p, const char* file, int32_t line, int32_t n0);    // Fog #10043 (ECX=p, EDX=file)
    // 0x170B0: the dword pair Fog publishes at gdwBitMasks+0xC / gdwInvBitMasks+0xC.
    uint32_t bitMask3;
    uint32_t invBitMask3;
    // 0x116A0 / 0x16D70.
    void     (*notify1450)(void* this_);                                            // 0x1450
    uint32_t (*release7fc0)(void* this_);                                           // 0x7FC0 (ECX=this)
    // 0x114A0: the D2Gfx wrappers with the renderer slot already bound.
    uint32_t (*d2gfx10052)(uint32_t a0);                                            // D2Gfx #10052 (slot +0x48)
    uint32_t (*d2gfx10000)(uint32_t a0, uint32_t a1, uint32_t a2);                  // D2Gfx #10000 (slot +0x40)
    // 0xA890.
    uint32_t (*fog10117)(void);
    uint32_t (*fog10227)(void);
    int32_t  (*checkObject)(void* obj);                                             // call *(obj+0x20D)
    void*    (*openArchive)(int32_t timeout, const char* dll, const char* mpq, const char* tag,
                            uint32_t a3, uint32_t a4);                              // 0x7E60 (5 stack args, timeout in EAX)
    void     (*cleanupA340)(void);                                                  // 0xA340
    // 0x18980 (message pump).
    uint32_t (*getTickCount)(void);
    int32_t  (*peekMessage)(void* msg, uint32_t hwnd, uint32_t min, uint32_t max, uint32_t remove);
    int32_t  (*getMessage)(void* msg, uint32_t hwnd, uint32_t min, uint32_t max);
    int32_t  (*translateMessage)(const void* msg);
    int32_t  (*dispatchMessage)(const void* msg);
    void     (*sleepMs)(uint32_t ms);
    void     (*idle87e0)(void);                                                     // 0x87E0
};

// Retail module state, one symbol per DLL address so a probe can seed and read it.
extern "C" {
uint32_t d2win_g_6f901494;                       // 0x17B30
char     d2win_g_6f9a9d18[0x104];                // 0x114A0, the copied path
uint8_t  d2win_g_6f9a9e1c;                       // 0x114A0, the flag byte
void*    d2win_g_6f9a9cc0;                       // 0xA890 d2char
void*    d2win_g_6f9a9cd8;                       // 0xA890 d2music
void*    d2win_g_6f9a9cb4;                       // 0xA890 d2Xmusic
void*    d2win_g_6f9a9cc8;                       // 0xA890 d2Xtalk
void*    d2win_g_6f9a9cdc;                       // 0xA890 d2Xvideo
uint32_t d2win_g_6f8ffd30;                       // 0x18980 running flag / GetMessage result
void   (*d2win_g_6f9a9cf0)(void);                // 0x18980 quit callback
uint32_t d2win_g_6f9a9cf4;                       // 0x18980
uint32_t d2win_g_6f9014ac;                       // 0x18980
uint32_t d2win_g_6f9014b0;                       // 0x18980
uint32_t d2win_g_6f8ffba8;                       // 0x18980, nonzero suppresses the sleep
}

// 1.13c D2Win+0x093C0 (export #10000, 872 bytes): append one word-wrapped text line to a text box.
// __fastcall: ECX = box, EDX = UTF-16 text, stack = (param3, param4, param5), ret 0xC. Null box is
// fatal 0x132, type != 4 fatal 0x133. The text is copied into a 0x400-unit stack buffer and leading
// whitespace is skipped. Unless box+0x5C bit 0x20 is set, the usable width
// (box+0x14 - 2*box+0x40, less the width of five spaces when param5 == 2) is compared with the
// measured text; if the text does not fit, 0x99F0 takes over (its result is returned). Otherwise a
// 0x20-byte node is allocated and its column string ("     " + text when param3 == 2, else text),
// param3 at +0x14 and param4's low byte at +0x18; the node is appended through box+0x48/+0x4C, the
// count at +0x54 is bumped, the current line (+0x50) and the shown index (+0x58) advance when the
// visible count is smaller, and the scrollbar at +0x90 gets its maximum (+0x48) and step (+0x4C).
// The type-5 re-checks before each scrollbar write are retail's; a mismatch is fatal (0x74/0x56/
// 0x4b/0x65). Returns 1 or 0x99F0's value.
// 1.13c D2Win+0x093C0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_93C0(void* box, const uint16_t* text, int32_t param3,
                                                        uint32_t param4, int32_t param5,
                                                        const D2WinBatch93C0Env* env) {
    if (box == nullptr) d2win_fatal(0x132);
    if (rd32(box, 0x00) != 4) d2win_fatal(0x133);

    uint16_t buf[0x400];
    for (uint32_t i = 0; i < 0x400; ++i) buf[i] = 0;
    u16_copy(buf, text);
    const uint16_t* p = buf;
    while (*p != 0 && d2win_isWhitespace(*p)) ++p;

    env->setTextFont(rd32(box, 0x94));
    if ((rd8(box, 0x5C) & 0x20) == 0) {
        int32_t width = (int32_t)rd32(box, 0x14) - 2 * (int32_t)rd32(box, 0x40);
        if (param5 == 2) {
            uint16_t spaces[6] = {0x20, 0x20, 0x20, 0x20, 0x20, 0};
            width -= env->textWidth(spaces, (int32_t)u16_len(spaces));
        }
        const int32_t measured = env->textWidth(p, 0xFF);
        if (width <= measured) return env->splitLines(box, param3, param5);
    }

    uint8_t* node = (uint8_t*)env->alloc(0x20, kTextBoxCpp, 0x15D, 0);
    uint8_t* col;
    if (param3 == 2) {
        const uint32_t len = u16_len(p);
        col = (uint8_t*)env->alloc(2 * len + 0xC, kTextBoxCpp, 0x161, 0);
        for (uint32_t i = 0; i < 5; ++i) wr16(col, i * 2, 0x20);
        u16_copy((uint16_t*)(col + 0x0A), p);
    } else {
        const uint32_t len = u16_len(p);
        col = (uint8_t*)env->alloc(2 * len + 2, kTextBoxCpp, 0x167, 0);
        u16_copy((uint16_t*)col, p);
    }
    wr32(node, 0x00, (uint32_t)(uintptr_t)col);
    wr32(node, 0x04, 0);
    wr32(node, 0x08, 0);
    wr32(node, 0x0C, 0);
    wr32(node, 0x10, 0);
    wr32(node, 0x14, (uint32_t)param3);
    wr8 (node, 0x18, (uint8_t)param4);
    wr32(node, 0x1C, 0);

    bool advanced = false;
    if (rd32(box, 0x48) == 0) {
        wr32(box, 0x48, (uint32_t)(uintptr_t)node);
        wr32(box, 0x4C, (uint32_t)(uintptr_t)node);
        wr32(box, 0x50, (uint32_t)(uintptr_t)node);
        wr32(box, 0x54, rd32(box, 0x54) + 1);
    } else {
        wr32((void*)(uintptr_t)rd32(box, 0x4C), 0x1C, (uint32_t)(uintptr_t)node);
        wr32(box, 0x4C, (uint32_t)(uintptr_t)node);
        wr32(box, 0x54, rd32(box, 0x54) + 1);
        if (rd32(box, 0x00) != 4) d2win_fatal(0x394);
        int32_t count = 0;
        for (uint32_t it = rd32(box, 0x50); it != 0; it = rd32((void*)(uintptr_t)it, 0x1C)) ++count;
        if (env->visibleLines(box) < count) {
            const uint32_t scrollbar = rd32(box, 0x90);
            if (scrollbar != 0) {
                if (rd32((void*)(uintptr_t)scrollbar, 0x00) != 5) d2win_fatal(0x74);
                if (rd32((void*)(uintptr_t)scrollbar, 0x00) != 5) d2win_fatal(0x4B);
                if (rd32((void*)(uintptr_t)scrollbar, 0x4C) != rd32((void*)(uintptr_t)scrollbar, 0x48)) goto LAB_9672;
            }
            wr32(box, 0x50, rd32((void*)(uintptr_t)rd32(box, 0x50), 0x1C));
            wr32(box, 0x58, rd32(box, 0x58) + 1);
            advanced = true;
        }
    }
LAB_9672:
    {
        const uint32_t scrollbar = rd32(box, 0x90);
        if (scrollbar == 0) return 1;
        if (rd32((void*)(uintptr_t)scrollbar, 0x00) != 5) d2win_fatal(0x74);
        const uint32_t oldStep = rd32((void*)(uintptr_t)scrollbar, 0x4C);
        const int32_t visible = env->visibleLines(box);
        if (rd32((void*)(uintptr_t)rd32(box, 0x90), 0x00) != 5) d2win_fatal(0x56);
        const int32_t diff = (int32_t)rd32(box, 0x54) - visible;
        wr32((void*)(uintptr_t)rd32(box, 0x90), 0x48, diff < 1 ? 0 : (uint32_t)diff);
        if (advanced) {
            if (rd32((void*)(uintptr_t)rd32(box, 0x90), 0x00) != 5) d2win_fatal(0x65);
            uint32_t v = rd32(box, 0x58);
            if (rd32((void*)(uintptr_t)rd32(box, 0x90), 0x48) < v)
                v = rd32((void*)(uintptr_t)rd32(box, 0x90), 0x48);
            wr32((void*)(uintptr_t)rd32(box, 0x90), 0x4C, v);
            return 1;
        }
        if (rd32((void*)(uintptr_t)rd32(box, 0x90), 0x00) != 5) d2win_fatal(0x65);
        uint32_t v = oldStep;
        if (rd32((void*)(uintptr_t)rd32(box, 0x90), 0x48) < v)
            v = rd32((void*)(uintptr_t)rd32(box, 0x90), 0x48);
        wr32((void*)(uintptr_t)rd32(box, 0x90), 0x4C, v);
        return 1;
    }
}

// 1.13c D2Win+0x09730 (export #10020, 12 bytes): append an unwrapped line -- the same appender with
// param3 = 4 (plain column string), param4 = 0, param5 = 0. EAX is 0x93C0's and reaches the caller.
// 1.13c D2Win+0x09730
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_9730(void* box, const uint16_t* text,
                                                        const D2WinBatch93C0Env* env) {
    return d2win_sub_93C0(box, text, 4, 0, 0, env);
}

// 1.13c D2Win+0x170B0 (export #10001, 42 bytes): set or clear bit 3 of the flags dword at rec+8.
// stdcall(rec, flag). Fog's gdwBitMasks[3] is OR-ed in when flag != 0, gdwInvBitMasks[3] masks it out
// otherwise. Returns nothing.
// 1.13c D2Win+0x170B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_170B0(void* rec, int32_t flag, const D2WinBatch93C0Env* env) {
    if (flag != 0) wr32(rec, 0x08, rd32(rec, 0x08) | env->bitMask3);
    else           wr32(rec, 0x08, rd32(rec, 0x08) & env->invBitMask3);
}

// 1.13c D2Win+0x17B30 (export #10006, 17 bytes): store the dword argument in the global at
// 0x6F901494 (here d2win_g_6f901494) and return 1.
// 1.13c D2Win+0x17B30
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_17B30(uint32_t value) {
    d2win_g_6f901494 = value;
    return 1;
}

// 1.13c D2Win+0x13720 (export #10021, 71 bytes): reset a type-1 control's text cursor. __fastcall
// (ECX = this); null is fatal 0x1B3, type != 1 fatal 0x1B5. Cursor at +0x54 becomes 0, length at
// +0x58 becomes the UTF-16 strlen of the buffer at +0x5C. Returns nothing.
// 1.13c D2Win+0x13720
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_13720(void* this_) {
    if (this_ == nullptr) d2win_fatal(0x1B3);
    if (rd32(this_, 0x00) != 1) d2win_fatal(0x1B5);
    wr32(this_, 0x54, 0);
    wr32(this_, 0x58, u16_len((const uint16_t*)((uint8_t*)this_ + 0x5C)));
}

// 1.13c D2Win+0x13930 (export #10010, 54 bytes): store a dword on a type-1 control. __fastcall
// (ECX = this, EDX = value); null fatal 0x86, type != 1 fatal 0x88. The value lands at +0x26C.
// 1.13c D2Win+0x13930
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_13930(void* this_, uint32_t value) {
    if (this_ == nullptr) d2win_fatal(0x86);
    if (rd32(this_, 0x00) != 1) d2win_fatal(0x88);
    wr32(this_, 0x26C, value);
}

// 1.13c D2Win+0x116A0 (export #10011, 53 bytes): type-9 control teardown. __fastcall (ECX = this);
// type != 9 fatal 0x8A. Calls 0x1450(this) with the this pointer on the stack, then tail-jumps to
// 0x7FC0 with ECX = this, so 0x7FC0's return value reaches the caller.
// 1.13c D2Win+0x116A0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_116A0(void* this_, const D2WinBatch93C0Env* env) {
    if (rd32(this_, 0x00) != 9) d2win_fatal(0x8A);
    env->notify1450(this_);
    return env->release7fc0(this_);
}

// 1.13c D2Win+0x16D70 (export #10012, 86 bytes): type-7 control teardown. __fastcall (ECX = this);
// type != 7 fatal 0x80. Each node of the +0x64 list (next at +0x220) is released through Fog #10043
// with the D2WinList.cpp literal and line 0x85, then the control itself goes through 0x7FC0.
// 1.13c D2Win+0x16D70
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_16D70(void* this_, const D2WinBatch93C0Env* env) {
    if (rd32(this_, 0x00) != 7) d2win_fatal(0x80);
    for (uint32_t node = rd32(this_, 0x64); node != 0;) {
        const uint32_t next = rd32((void*)(uintptr_t)node, 0x220);
        env->freeMemory((void*)(uintptr_t)node, kListCpp, 0x85, 0);
        node = next;
    }
    return env->release7fc0(this_);
}

// 1.13c D2Win+0x11760 (export #10003, 58 bytes): free a font's linked records. __fastcall (ECX =
// first node); next pointer at +4, the string at +0 and then the node itself are released through Fog
// #10043 with the D2WinFont.cpp literal, lines 0x579 and 0x57A. Returns nothing.
// 1.13c D2Win+0x11760
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_11760(uint32_t node, const D2WinBatch93C0Env* env) {
    while (node != 0) {
        const uint32_t next = rd32((void*)(uintptr_t)node, 0x04);
        env->freeMemory((void*)(uintptr_t)rd32((void*)(uintptr_t)node, 0x00), kFontCpp, 0x579, 0);
        env->freeMemory((void*)(uintptr_t)node, kFontCpp, 0x57A, 0);
        node = next;
    }
}

// 1.13c D2Win+0x114A0 (export #10002, 124 bytes): install a type-9 control's video path. stdcall
// (box, text, value), ret 0xC; type != 9 fatal 0x51. The text is copied (0x100 bytes) to box+0x40,
// D2Gfx #10052 is called on box+0x140, D2Gfx #10000 gets ECX = box+0x40, EDX = box+0x140 and the
// text on the stack, and the module globals at 0x6F9A9D18 (0x104-byte copy) and 0x6F9A9E1C (low
// byte of value) are written. Returns 1.
// 1.13c D2Win+0x114A0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_114A0(void* box, const char* text, uint32_t value,
                                                         const D2WinBatch93C0Env* env) {
    if (rd32(box, 0x00) != 9) d2win_fatal(0x51);
    std::strncpy((char*)box + 0x40, text, 0x100);
    env->d2gfx10052((uint32_t)(uintptr_t)((uint8_t*)box + 0x140));
    env->d2gfx10000((uint32_t)(uintptr_t)((uint8_t*)box + 0x40),
                    (uint32_t)(uintptr_t)((uint8_t*)box + 0x140),
                    (uint32_t)(uintptr_t)text);
    std::strncpy(d2win_g_6f9a9d18, text, 0x104);
    d2win_g_6f9a9e1c = (uint8_t)value;
    return 1;
}

// 1.13c D2Win+0x0A890 (export #10005, 396 bytes): open the game's MPQs. __fastcall: ECX and EDX are
// the two arguments 0x7E60 receives as its last stack words; two stack arguments follow (ret 8), the
// second of which is an optional object whose function pointer at +0x20D must return nonzero to take
// the expansion branch. Fog #10117 is called first; a nonzero result zeroes ECX/EDX. The global at
// 0x6F9A9CDC is cleared, then Fog #10227 decides: zero (or no object, or +0x20D returning zero)
// opens only d2char.mpq and d2music.mpq and clears the expansion globals; otherwise the three
// d2X*.mpq expansion archives are opened too, with timeouts 0x3E8 for the base pair and 0xBB8 for
// the expansion ones. Success needs d2char and d2music (plus d2Xmusic, d2Xtalk and d2Xvideo in the
// expansion branch); failure calls 0xA340, clears the handles and returns 0. Returns 1 on success.
// 1.13c D2Win+0x0A890
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_A890(uint32_t ecxArg, uint32_t edxArg,
                                                        uint32_t stackArg0, void* obj,
                                                        const D2WinBatch93C0Env* env) {
    (void)stackArg0;
    if (env->fog10117() != 0) { ecxArg = 0; edxArg = 0; }
    d2win_g_6f9a9cdc = nullptr;
    const bool expanded = env->fog10227() != 0 &&
                          (obj == nullptr || env->checkObject(obj) != 0);
    if (!expanded) {
        d2win_g_6f9a9cc0 = env->openArchive(0x3E8, "D2Win.DLL", "d2char.mpq", "D2CHAR", 0, ecxArg);
        d2win_g_6f9a9cd8 = env->openArchive(0x3E8, "D2Win.DLL", "d2music.mpq", "D2MUSIC", 0, ecxArg);
        d2win_g_6f9a9cb4 = nullptr;
        d2win_g_6f9a9cc8 = nullptr;
        if (d2win_g_6f9a9cc0 != nullptr && d2win_g_6f9a9cd8 != nullptr) return 1;
        env->cleanupA340();
        return 0;
    }
    d2win_g_6f9a9cc0 = env->openArchive(0x3E8, "D2Win.DLL", "d2char.mpq", "D2CHAR", ecxArg, edxArg);
    d2win_g_6f9a9cd8 = env->openArchive(0x3E8, "D2Win.DLL", "d2music.mpq", "D2MUSIC", ecxArg, edxArg);
    d2win_g_6f9a9cb4 = env->openArchive(0xBB8, "D2Win.DLL", "d2Xmusic.mpq", "D2EXPANSION", ecxArg, edxArg);
    d2win_g_6f9a9cc8 = env->openArchive(0xBB8, "D2Win.DLL", "d2Xtalk.mpq", "D2EXPANSION", ecxArg, edxArg);
    d2win_g_6f9a9cdc = env->openArchive(0xBB8, "D2Win.DLL", "d2Xvideo.mpq", "D2EXPANSION", ecxArg, edxArg);
    if (d2win_g_6f9a9cc0 != nullptr && d2win_g_6f9a9cd8 != nullptr && d2win_g_6f9a9cb4 != nullptr &&
        d2win_g_6f9a9cc8 != nullptr && d2win_g_6f9a9cdc != nullptr) return 1;
    env->cleanupA340();
    return 0;
}

// 1.13c D2Win+0x18980 (export #10014, 283 bytes): the message pump. stdcall(callback), ret 4; the
// callback (if non-null) receives an increasing 32-bit counter every idle tick. PeekMessageA with no
// message drains the tick accumulator (deltas capped at 1000), adds 0x28 when it goes below 1
// (clamped at -1000 back to 0), runs the callback and 0x87E0, copies 0x6F9014AC to 0x6F9014B0, and
// sleeps min(accumulator, 0x14) -- or nothing when 0x6F8FFBA8 is nonzero. With a message it stores
// GetMessageA's result in the running flag, calls the quit callback at 0x6F9A9CF0 when that result
// is 0, then translates and dispatches. On exit 0x87E0 runs once more and, when 0x6F9A9CF4 is 0,
// the running flag is reset to 1. Returns 0.
// 1.13c D2Win+0x18980
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_18980(void (*callback)(int32_t),
                                                         const D2WinBatch93C0Env* env) {
    uint8_t msg[0x1C];
    int32_t counter = 0;
    uint32_t lastTick = env->getTickCount();
    uint32_t accum = 0;
    while (d2win_g_6f8ffd30 != 0) {
        if (env->peekMessage(msg, 0, 0, 0, 0) == 0) {
            uint32_t delta = env->getTickCount() - lastTick;
            lastTick = env->getTickCount();
            if (delta > 1000) delta = 1000;
            accum -= delta;
            if ((int32_t)accum < 1) {
                accum += 0x28;
                if ((int32_t)accum < -1000) accum = 0;
                if (callback != nullptr) { callback(counter); ++counter; }
                env->idle87e0();
            }
            d2win_g_6f9014b0 = d2win_g_6f9014ac;
            uint32_t ms = accum;
            if (ms > 0x14) ms = 0x14;
            if (d2win_g_6f8ffba8 != 0) ms = 0;
            env->sleepMs(ms);
        } else {
            d2win_g_6f8ffd30 = (uint32_t)env->getMessage(msg, 0, 0, 0);
            if (d2win_g_6f8ffd30 == 0 && d2win_g_6f9a9cf0 != nullptr) d2win_g_6f9a9cf0();
            env->translateMessage(msg);
            env->dispatchMessage(msg);
        }
    }
    env->idle87e0();
    if (d2win_g_6f9a9cf4 == 0) d2win_g_6f8ffd30 = 1;
    return 0;
}
