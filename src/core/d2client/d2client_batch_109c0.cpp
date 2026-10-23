// src/core/d2client/d2client_batch_109c0.cpp -- ten UNMAPPED 1.13c D2Client.dll rows reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, RE-DONE first).
// Base 0x6FAB0000; every RVA below is retail's, and every global this file owns is a static at its
// retail RVA comment. No spec, no vector yet.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// Host seams for a row whose remaining callees are not recovered yet or are imports.
struct D2ClientBatch109C0Fog {
    void     (*fogDisplayHalt)(uint32_t fmt, uint32_t caller_return);  // 0xBF80  Fog ordinal 10024
    uint32_t (*fogCallerReturnAddress)(void);                          // 0xBF92  Fog ordinal 10265
    void     (*exit_)(int32_t code);                                   // 0x3664  _exit (noreturn)
};

// Win32/Storm imports 0x10A60 reaches (import veneers; the decompiler resolves them by name).
struct D2ClientBatch109C0Win {
    uint32_t (*stormStrcmp)(const char* a, const char* b, uint32_t max);  // Storm.dll #508 SStrCmp
    int32_t  (*getUserNameA)(char* buf, uint32_t* size);                  // advapi32!GetUserNameA
    int32_t  (*messageBoxA)(uint32_t hwnd, const char* text,
                            const char* caption, uint32_t type);          // user32!MessageBoxA
};

// 1.13c D2Client+0x039C9, __stricmp (d2client_batch_038a3.cpp).
extern "C" int32_t d2_client_39c9(const char* a, const char* b);
// 1.13c D2Client+0x11110, HUD scrollbar hit A (d2client_batch_10110.cpp).
extern "C" int32_t d2_client_11110(int32_t x, int32_t y, int32_t w, int32_t h);
// 1.13c D2Client+0x12DD0, UI mode toggle (ui_toggle_12dd0.cpp).
extern "C" uint32_t d2_client_12dd0(uint32_t open, uint32_t gate, uint32_t flag, uint32_t* out);

// ---------------------------------------------------------------------------------------------------------
// 0x109C0

// +0x11B96C..+0x11B978  DAT_6fbcb96c/b970/b974/b978: four dwords the pop shifts down.
static uint32_t g_109c0_slots[4];
// +0x11C568  DAT_6fbcc568: the live-entry count.
static uint32_t g_109c0_count;

// 1.13c D2Client+0x109c0 (95 B): pop the front dword of the four-slot queue. When the count is zero the
// retail path is fatal: Fog ordinal 10265 supplies the caller's return address and Fog ordinal 10024
// (FOG_DisplayHalt) reports "queue is empty" with the format at +0x0D359F, then _exit(-1). Otherwise it
// returns slot[0], shifts slot[0] <- slot[1] and slot[2] <- slot[3], and decrements the count. The
// decompile reads slot[2] before overwriting it and stores it into slot[1] (slot[1] <- old slot[2]).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_109c0(const D2ClientBatch109C0Fog* fog) {
    if (g_109c0_count == 0) {
        uint32_t ra = fog->fogCallerReturnAddress();
        fog->fogDisplayHalt(0x0D359Fu, ra);
        fog->exit_(-1);
    }
    uint32_t front = g_109c0_slots[0];
    uint32_t old2 = g_109c0_slots[2];
    g_109c0_slots[0] = g_109c0_slots[1];
    g_109c0_slots[2] = g_109c0_slots[3];
    g_109c0_slots[1] = old2;
    g_109c0_count--;
    return front;
}

// ---------------------------------------------------------------------------------------------------------
// 0x10A60

// +0x0F8C5C  _DAT_6fba8c5c: the selected command-line handler index (the 0x10C80 dispatcher uses it as a
// dword index into the handler table at +0x0F8C60).
static uint32_t g_10a60_state;
// +0x0F8C7C  DAT_6fba8c7c: the seven option-name string pointers, walked by __stricmp.
static const char* g_10a60_names[7];
// +0x11C564  DAT_6fbcc564: the one-shot guard for the developer user-name check.
static uint32_t g_10a60_checked;

// 1.13c D2Client+0x10a60 (163 B): resolve a command-line option record. Walk the seven name pointers;
// the first non-NULL one that __stricmp(0x39C9) matches case-insensitively against the record's name at
// +0x18 selects that index (stored in the state word) and returns 1. With no match, the first call only
// reads the Windows user name into a 16-byte buffer (size 0xF) and compares it with Storm #508 SStrCmp
// (case-sensitive, INT_MAX) against "rseis"; a non-zero result opens the D2 "Available command line
// options" MessageBox. Always returns 0 when nothing matched. The name pointers are retail .rdata, not
// in the decompile: the table is host-populated, all entries NULL until pinned by a vector.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_10a60(void* rec, const D2ClientBatch109C0Win* win) {
    uint8_t* r = (uint8_t*)rec;
    for (uint32_t i = 0; i < 7; i++) {
        if (g_10a60_names[i] != nullptr &&
            d2_client_39c9(g_10a60_names[i], *(const char**)(r + 0x18)) == 0) {
            g_10a60_state = i;
            return 1;
        }
    }
    if (g_10a60_checked == 0) {
        g_10a60_checked = 1;
        char user[16];
        uint32_t size = 0x0F;
        win->getUserNameA(user, &size);
        if (win->stormStrcmp(user, "rseis", 0x7FFFFFFF) != 0)
            win->messageBoxA(0, "Available command line options:\n"
                "  -set[default command line]\t\tSet new default command line string\n"
                "  -lng[lngfile]\t\t\tUse \"lngfile\" for language resources\n",
                "Diablo II", 0);
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
// 0x10F70, 0x10FD0 -- HUD scrollbar hit boxes E/F (caller 0x11A80). x is
// ESI and y EDI; the globals are passed in the established style (hittest_11070.cpp): w = +0x0DBC48
// DAT_6fb8bc48, h = +0x0F9E18 DAT_6fba9e18, thumbY = +0x1233F4 DAT_6fbd33f4, count = +0x11C500
// DAT_6fbcc500 (the thumb travel is count * 0xC).

// 1.13c D2Client+0x10f70 (91 B), hit F: right-hand column, y between the bottom of the thumb track and
// the panel bottom: x in [w-m-0x1F, w-m-0x12], m = (w-0x26C)/2; thumbY + count*0xC < y < (h-0x1A4)/2 +
// 0x125. Strict on the y edges.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_10f70(int32_t x, int32_t y, int32_t w, int32_t h,
                                              int32_t thumbY, int32_t count) {
    int32_t m = (w - 0x26C) / 2;
    return (x >= w - m - 0x1F && x <= w - m - 0x12 &&
            y < (h - 0x1A4) / 2 + 0x125 && thumbY + count * 0xC < y) ? 1 : 0;
}

// 1.13c D2Client+0x10FD0 (71 B), hit E: the thumb itself. x in the same right-hand column; y inside
// [thumbY, thumbY + count*0xC], both edges inclusive.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_10fd0(int32_t x, int32_t y, int32_t w,
                                              int32_t thumbY, int32_t count) {
    int32_t m = (w - 0x26C) / 2;
    return (x >= w - m - 0x1F && x <= w - m - 0x12 &&
            y >= thumbY && y <= thumbY + count * 0xC) ? 1 : 0;
}

// ---------------------------------------------------------------------------------------------------------
// 0x111B0, 0x111F0 -- HUD scroll list (caller 0x11A80; 0x111F0 also from 0x112E0).

// 1.13c D2Client+0x111B0 (61 B): the x belongs to the left two columns of the scroll list. param_1 is x,
// w = +0x0DBC48; m = (w-0x26C)/2. True while x < m + 0x12 + ((w - 2m - 0x31)/3) * 2.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_111b0(int32_t x, int32_t w) {
    int32_t m = (w - 0x26C) / 2;
    return (x < m + 0x12 + ((w - 2 * m - 0x31) / 3) * 2) ? 1 : 0;
}

// 1.13c D2Client+0x111F0 (77 B): row index under the cursor. y is EBX; first = +0x11C534 DAT_6fbcc534
// (first visible row), count = +0x11C4FC DAT_6fbcc4fc (row count), h = +0x0F9E18. Walk up to 15 rows from
// first, row tops starting at (h-0x1A4)/2 + 0x3A with stride 0x12: the first row whose top is at or below
// y wins; out of rows (or y past all 15) falls back to count-1 (count itself when negative).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_111f0(int32_t y, int32_t first, int32_t count, int32_t h) {
    int32_t i = 0;
    if (first < count) {
        int32_t top = (h - 0x1A4) / 2 + 0x3A;
        int32_t row = first;
        do {
            if (i > 0xE) {
                if (row < count) return row;
                break;
            }
            if (y <= top) return row;
            i++;
            top += 0x12;
            row++;
        } while (row < count);
    }
    return (count < 0) ? count : count - 1;
}

// ---------------------------------------------------------------------------------------------------------
// 0x112E0

// 1.13c D2Client+0x112E0 (105 B, caller 0x130B0): confirm the press on the live row. `param` is ECX, x is
// EDI and y is EBX at entry (live-in registers of the UI dispatcher). True only when hit A
// (0x11110) contains (x, y), the row under y (0x111F0) is the selected one (+0x11C538 DAT_6fbcc538), and
// (param < left-column bound) equals the left-column flag +0x0F8BE8 DAT_6fba8be8. The bound is the same
// expression as 0x111B0; `param` and x are separate inputs because the comments reads both.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_112e0(int32_t param, int32_t x, int32_t y, int32_t w, int32_t h,
                                              int32_t selected, int32_t first, int32_t count,
                                              int32_t left_flag) {
    if (d2_client_11110(x, y, w, h) == 0) return 0;
    if (d2_client_111f0(y, first, count, h) != selected) return 0;
    int32_t m = (w - 0x26C) / 2;
    int32_t left = (param < m + 0x12 + ((w - 2 * m - 0x31) / 3) * 2) ? 1 : 0;
    return (left == left_flag) ? 1 : 0;
}

// ---------------------------------------------------------------------------------------------------------
// 0x117C0 -- HUD scroll list page step (caller 0x11A80; it also recurses into itself and calls 0x1240).

// The scroll-list record at +0x11C4FC..: total rows, visible rows, first visible, selected; the thumb and
// the HUD height are the same globals the hit boxes use.
static int32_t g_scroll_total;        // +0x11C4FC  DAT_6fbcc4fc
static int32_t g_scroll_visibleRows;  // +0x11C500  DAT_6fbcc500
static int32_t g_scroll_first;        // +0x11C534  DAT_6fbcc534
static int32_t g_scroll_selected;     // +0x11C538  DAT_6fbcc538
static int32_t g_scroll_thumbY;       // +0x1233F4  DAT_6fbd33f4
static int32_t g_scroll_hudH;         // +0x0F9E18  DAT_6fba9e18

// D2Client+0x1240 is not a recovered row: the thumb ratio's divisor is a host seam. Called with no
// argument (the decompiler shows a 64-bit result whose low dword is used).
struct D2ClientBatch109C0Scroll {
    int32_t (*row_count_1240)(void);
};

// 1.13c D2Client+0x117c0 (333 B): move the selection by ECX (0 = no-op). |n| > 1 is applied one unit at a
// time through recursive calls. -1 clamps the selected row down to 0; +1 stops at total-1. It then
// ensures the selected row is inside the 15-row window starting at the first visible row, scrolling the
// window by one row at a time and recomputing the thumb Y each step:
//   thumbY = ((visibleRows * -0xC + 0xF6) * first) / 0x1240() + 0x2F + (hudH - 0x1A4) / 2
// When the first visible row is already past the selection it jumps first := selected and recomputes
// thumbY once. All divisions are signed (truncate toward zero). Ghidra's callee list also names 0x11240,
// which the body never calls.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_117c0(int32_t param, const D2ClientBatch109C0Scroll* env) {
    int32_t rows = g_scroll_visibleRows;
    if (param == 0) return;
    if (param < -1) {
        int32_t n = -param;
        do { d2_client_117c0(-1, env); n--; } while (n != 0);
        return;
    }
    if (param > 1) {
        do { d2_client_117c0(1, env); param--; } while (param != 0);
        return;
    }
    if (param < 0) {
        int32_t sel = g_scroll_selected;
        if (g_scroll_selected > 0) sel = g_scroll_selected - 1;
        if (sel < 0) goto recompute;
        g_scroll_selected = sel;
    } else {
        int32_t sel;
        if (g_scroll_total <= g_scroll_selected) goto recompute;
        sel = g_scroll_selected + 1;
        if (g_scroll_total <= sel) goto recompute;
        g_scroll_selected = sel;
    }
recompute:
    {
        int32_t sel = g_scroll_selected;
        int32_t first = g_scroll_first;
        if (first > sel) {
            int32_t h = g_scroll_hudH - 0x1A4;
            g_scroll_first = sel;
            g_scroll_thumbY = ((rows * -0xC + 0xF6) * sel) / env->row_count_1240() +
                              0x2F + h / 2;
            return;
        }
        for (;;) {
            int32_t last = first;
            for (int32_t i = 0; last < g_scroll_total && i < 0xF; i++) last++;
            if (sel < last) break;
            int32_t limit = env->row_count_1240();
            int32_t cand = (first < limit) ? first + 1 : first;
            if (cand <= limit) first = cand;
            g_scroll_thumbY = ((rows * -0xC + 0xF6) * first) / limit +
                              0x2F + (g_scroll_hudH - 0x1A4) / 2;
        }
        g_scroll_first = first;
    }
}

// ---------------------------------------------------------------------------------------------------------
// 0x12EB0

// Seams for the rows 0x12EB0 reaches that are not implemented yet, plus the Fog/win32 imports. The
// 0x3CC60/0x3CCC0 calls enter with EAX=4 (sub_12DD0.md) and take the ordinal-10048 result and the retail
// .data block (passed as its RVA) on the stack. The six 0xC-stride records at +0x0F8BF4 are read only in
// their first dword.
struct D2ClientBatch109C0Reset {
    int32_t  (*is_42090)(void);
    int32_t  (*is_420E0)(void);
    uint32_t (*fog_10048)(void);
    void     (*f_3cc60)(uint32_t eax, uint32_t ord, uint32_t data_rva);
    void     (*f_3ccc0)(uint32_t eax, uint32_t ord, uint32_t data_rva);
    void     (*f_3ca80)(uint32_t value);
    int32_t  (*getSystemMetrics)(int32_t index);
};

static uint32_t g_12eb0_gate;        // +0x11C540  DAT_6fbcc540, set to 1 at entry
static uint32_t g_12eb0_flag;        // +0x12349C  DAT_6fbd349c, read by 0x12DD0
static uint32_t g_12eb0_ids[6];      // +0x0F8BF4  0xC-stride records, first dword only
static uint32_t g_12eb0_sel;         // +0x11C3CC  DAT_6fbcc3cc
static uint32_t g_12eb0_mode;        // +0x119898  DAT_6fbc9898
static uint32_t g_12eb0_sub;         // +0x11988C  DAT_6fbc988c
static uint32_t g_12eb0_sub2;        // +0x119890  DAT_6fbc9890

// 1.13c D2Client+0x12eb0 (358 B, caller 0x130B0): reset the UI. Latch the gate (+0x11C540 = 1); if either
// user-interface predicate (0x42090, 0x420E0) holds, draw block +0x0D0774 through 0x3CC60; always draw
// +0x0D06D8; close the UI with 0x12DD0(0) (gate is now 1, so its close block is skipped); if the Fog
// surface handle (ordinal 10048) is 0, fatal via Fog 10265/10024 and _exit(-1); feed the six record ids
// at +0x0F8BF4 to 0x3CA80; then, in the selected/visible-mode states, draw +0x0F8C38, +0x0F8C44 and (when
// the vertical scroll metric is present) +0x0F8C50 through 0x3CCC0. Every 0x3CC60/0x3CCC0 call re-reads
// the Fog surface handle.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_12eb0(const D2ClientBatch109C0Reset* env,
                                           const D2ClientBatch109C0Fog* fog) {
    uint32_t out[16];
    g_12eb0_gate = 1;
    if (env->is_42090() != 0 || env->is_420E0() != 0)
        env->f_3cc60(4, env->fog_10048(), 0x0D0774u);
    env->f_3cc60(4, env->fog_10048(), 0x0D06D8u);
    d2_client_12dd0(0, g_12eb0_gate, g_12eb0_flag, out);
    if (env->fog_10048() == 0) {
        uint32_t ra = fog->fogCallerReturnAddress();
        fog->fogDisplayHalt(0x0D359Fu, ra);
        fog->exit_(-1);
    }
    for (uint32_t u = 0; u < 0x48; u += 0xC)
        env->f_3ca80(g_12eb0_ids[u / 0xC]);
    if (g_12eb0_sel != 0) {
        if (g_12eb0_mode == 1 || (g_12eb0_mode == 2 && (g_12eb0_sub == 4 || g_12eb0_sub == 5)))
            env->f_3ccc0(4, env->fog_10048(), 0x0F8C38u);
        if (g_12eb0_sel != 0 && g_12eb0_mode != 1 && g_12eb0_mode == 2 &&
            g_12eb0_sub != 4 && g_12eb0_sub == 5)
            env->f_3ccc0(4, env->fog_10048(), 0x0F8C44u);
    }
    if (env->getSystemMetrics(0x4B) != 0 && g_12eb0_sel != 0) {
        if (g_12eb0_mode == 1) {
            if (g_12eb0_sub2 == 0) return;
        } else {
            if (g_12eb0_mode != 2) return;
            if (g_12eb0_sub != 4 && g_12eb0_sub != 5) return;
        }
        env->f_3ccc0(4, env->fog_10048(), 0x0F8C50u);
    }
}

// ---------------------------------------------------------------------------------------------------------
// 0x134E0

// 1.13c D2Client+0x134E0 (128 B, caller 0x13CE0): fill 8 records of 0x60 bytes. In each record the eight
// dwords at +0x00, +0x0C, +0x18, +0x24, +0x30, +0x3C, +0x48 and +0x54 become 0xFFFFFFFF; the other 0x40
// bytes are untouched. The decompile writes the four bytes of each dword in the order 0, 0xC-relative,
// 1, 3, 2 -- one byte each, so the result is the same 0xFF fill.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_134e0(uint8_t* p) {
    for (int32_t r = 8; r != 0; r--, p += 0x60) {
        for (int32_t off = 0; off < 0x60; off += 0xC) {
            p[off + 0] = 0xFF;
            p[off + 1] = 0xFF;
            p[off + 2] = 0xFF;
            p[off + 3] = 0xFF;
        }
    }
}
