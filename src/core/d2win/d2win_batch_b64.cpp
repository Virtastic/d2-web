// src/core/d2win/d2win_batch_b64.cpp -- 1.13c D2Win rows from cached Ghidra decompiles (alpha batch lane).
// Implementation + marker only; callees are seams in D2WinB64Env. 0x0A470 skipped (direct GetFileAttributesA).
#include <cstdint>
#include <cstdio>
#include <initializer_list>

struct D2WinB64Env {
    void (*fatal)(uint32_t strId);                         // Ordinal_10265(id) -> Ordinal_10024("",s) -> _exit(-1)
    void* (*alloc)(uint32_t size, uint32_t zero);          // Ordinal_10042
    void (*loadPl2)(const char* path);                     // 0xEF20
    void (*applyPalette)(void);                            // 0xBD70
    uint32_t (*cellCount)(void* thisp);                    // Ordinal_10086
    void* (*cellData)(uint32_t* rec);                      // Ordinal_10021
    int32_t (*cellWidth)(void* cell);                      // Ordinal_10060
    void (*drawCell)(uint32_t* rec, int32_t x, int32_t y, uint32_t a, uint32_t b, uint32_t c);   // Ordinal_10041
    void (*drawCellTrans)(uint32_t* rec, int32_t x, int32_t y, uint32_t a, uint32_t b, uint32_t c); // Ordinal_10019
    void (*pre18060)(void);                                // 0x17970
    void (*gfxFree)(uint32_t h);                           // Ordinal_10065
    void (*gfxCall)(uint32_t a, uint32_t b);               // Ordinal_10043
    void (*sub_12200)(void);
    uint32_t (*getWindow)(void);                           // Ordinal_10048
    void (*registerEntry)(uint32_t w, uint32_t a, int32_t b, uint32_t c); // 0x11190
    void (*initWidgetTail)(void);                          // 0xD880
    uint32_t (*lookupColor)(uint32_t idx);                 // 0xBF60
    uint32_t (*fn_dff0)(void);                             // 0xDFF0
    uint32_t (*fn_c410)(uint32_t* w, uint32_t a);          // 0xC410
    uint32_t (*fn_c180)(uint32_t a);                       // 0xC180
    uint32_t (*getMode)(void);                             // Ordinal_10078
    int32_t (*strlen16)(const uint16_t*);
    int32_t (*strncmp16)(const uint16_t*, const uint16_t*, int32_t);
    uint8_t* (*curGlyph)(void);                            // *DAT_6f901508
    int32_t (*glyphSheetW)(void* cell);                    // Ordinal_10000
    void (*drawGlyph)(int32_t* ctx, int32_t x, int32_t y, uint8_t* g, int32_t n, uint32_t m); // Ordinal_10074
    uint8_t (*fontHeight)(void);                           // *(tbl[curFont*0x14]+10)
    uint32_t colorSwitchA, colorSwitchB;                   // DAT_6f9aa670 / DAT_6f9aa66c marker values
    uint32_t extraWord, glyphWord;                         // DAT_6f901628 / *DAT_6f9014f0
    uint32_t* bitMasks;                                    // gdwBitMasks
    uint32_t* palTable;                                    // DAT_6f9a8cb0, 3 words per entry
    int32_t palCount;                                      // DAT_6f9a98a4
    uint32_t** widgetHead;                                 // DAT_6f9a9e30
    uint32_t** listHead;                                   // DAT_6f9014a0
    uint32_t* dat14a8;                                     // DAT_6f9014a8
    uint32_t* dat14ec;                                     // DAT_6f9014ec
    const uint32_t* entryTbl;                              // DAT_6f8ffbb0, 0x20 x 3 words
    void* (*openMpq)(const char*, const char*, const char*); // 0x7E60
    void** mpqVideo; void** mpqExpansion;                  // DAT_6f9a9cc4 / DAT_6f9a9cdc
    void (*unicodeInit)(uint16_t*);                        // closure ctor
    void (*win2Unicode)(uint16_t*, const char*, int32_t);
    void (*sub_8920)(uint16_t*, int32_t, int32_t);
    int32_t (*fog10117)(void);                             // Fog #10117 (result ignored by 0xA820)
};
extern D2WinB64Env g_d2winB64;
#define E g_d2winB64

// 1.13c D2Win+0x0F240
uint32_t d2win_load_palette_pl2(int32_t mode, const char* path) {
    char a[260], b[260];
    if (mode == 0) {
        snprintf(a, sizeof a, "%s\\%s", "DATA\\GLOBAL", "palette\\act1\\pal.dat");
        snprintf(b, sizeof b, "%s\\%s", "DATA\\GLOBAL", "palette\\act1\\pal.pl2");
        E.loadPl2(b);
        E.applyPalette();
        return 1;
    }
    E.loadPl2(path);
    E.applyPalette();
    return 1;
}

// 1.13c D2Win+0x117A0 -- __thiscall; hidden EBX (x) and stack-retaddr slot (y) become leading params.
void d2win_sub_117A0(int32_t x, uint32_t y, void* thisp, int32_t align, uint32_t a2, int32_t a3) {
    uint32_t n = E.cellCount(thisp);
    uint32_t rec[18] = {0};
    if (align == 1 || align == 2) {
        uint32_t sum = 0;
        for (uint32_t i = 0; i < n; i++) { rec[0] = i; sum += (uint32_t)E.cellWidth(E.cellData(rec)); }
        x += (align == 1) ? (int32_t)(-1 - (int32_t)(sum >> 1)) : -(int32_t)sum;
    }
    for (uint32_t i = 0; i < n; i++) {
        rec[0] = i;
        if (a3 == -1) E.drawCell(rec, x, (int32_t)y, 0xffffffffu, a2, 0);
        else E.drawCellTrans(rec, x, (int32_t)y, 0xffffffffu, a2, (uint32_t)a3);
        x += 0x100;
    }
}

// 1.13c D2Win+0x18060
uint32_t d2win_sub_18060(void) {
    *E.dat14a8 = 0;
    E.pre18060();
    uint32_t h = *E.dat14ec;
    E.gfxFree(h);
    if (h) E.gfxCall(0x5c, 0);
    *E.dat14ec = 0;
    E.sub_12200();
    uint32_t w = E.getWindow();
    if (!w) { E.fatal(0x5a); return 0; }
    for (uint32_t o = 0; o < 0x180; o += 0xc) {
        const uint32_t* t = E.entryTbl + o / 4;
        E.registerEntry(w, t[0], (int32_t)t[1], t[2]);
    }
    return 1;
}

// 1.13c D2Win+0xe3c0 -- hidden EBP (cols, 16 bytes) and retaddr slot (tx, 16 chars) are leading params.
uint32_t* d2win_sub_E3C0(const uint8_t* cols, const char* tx) {
    uint32_t* w = (uint32_t*)E.alloc(0x234, 0);
    if (!w) { E.fatal(0xc66); return nullptr; }
    for (int i = 0; i < 0x8d; i++) w[i] = 0;
    uint32_t* s = (uint32_t*)E.alloc(0x85b, 0);
    if (!s) { E.fatal(0x85c); return nullptr; }
    *s = 0;
    w[0x2e] = (uint32_t)(uintptr_t)s;
    E.initWidgetTail();
    w[7] = 0xc65; w[9] = 1; w[8] = 0;
    for (uint32_t i = 0; i < 16; i++) {
        w[0xe + 0x10 + i] = 1;
        uint8_t c = cols[i];
        if (c == 0 || (int32_t)c >= E.palCount || c == 0xff) w[0xe + i] = 0;
        else { E.lookupColor(c); w[0xe + i] = E.palTable[c * 3]; w[0xe + 0x10 + i] = E.palTable[c * 3 + 2]; }
    }
    uint32_t v = w[0] | E.bitMasks[0]; v |= E.bitMasks[1]; w[0] = v | E.bitMasks[2];
    char* p = (char*)w + 0x28;
    for (int i = 0; i < 16; i++) { char c = tx[i]; p[i] = (c != -1) ? (char)(c - 1) : c; }
    if (E.fn_dff0() != 0 && E.fn_c410(w, 0) == 0) return nullptr;
    w[0x32] = 0;
    w[0x31] = (uint32_t)(uintptr_t)*E.widgetHead;
    if (*E.widgetHead) (*E.widgetHead)[50] = (uint32_t)(uintptr_t)w;  // +200
    *E.widgetHead = w;
    return w;
}

// 1.13c D2Win+0x11C30 -- __fastcall (ecx=text, edx=x, then y, startCol, mode)
uint32_t d2win_sub_11C30(const uint16_t* text, int32_t x, uint32_t y, int32_t startCol, int32_t mode) {
    uint32_t ret = (uint32_t)mode;
    if (mode < 0) return ret;
    uint32_t gm = E.getMode();
    int32_t n = E.strlen16(text);
    ret = 0;
    uint32_t rec[18] = {0};
    int32_t lineX = x;
    for (int32_t i = 0; i < n;) {
        int32_t next = i + 1;
        if (text[i] < 0x100 && text[i] == 0xff) {
            uint16_t tag[2] = { text[i + 1], 0 };
            next = i + 2;
            if (E.strncmp16(tag, (const uint16_t*)&E.colorSwitchA, 1) == 0) { next = i + 3; ret = 0; }
            else {
                ret = (uint32_t)E.strncmp16(tag, (const uint16_t*)&E.colorSwitchB, 1);
                if (ret == 0) {
                    next = i + 3;
                    uint8_t* g = E.curGlyph();
                    rec[0] = *(uint16_t*)(g + 8);
                    rec[13] = E.extraWord;
                    E.drawCell(rec, x, (int32_t)y, 0xffffffffu, 5, 0);
                    ret = ((uint32_t)E.fontHeight() << 4) / 10;
                    y += ret;
                }
            }
        } else if (text[i] == 10) {
            ret = (uint32_t)E.fontHeight() * 0x99999990u;  // x * -0x66666670, as decompiled
            y += (uint32_t)((int32_t)(-(int32_t)((uint32_t)E.fontHeight() << 4)) / 10);
            x = lineX;
        } else {
            uint8_t* g = E.curGlyph();
            if (gm != 6) {
                rec[0] = *(uint16_t*)(g + 8);
                rec[13] = E.glyphWord;
                int32_t w = E.glyphSheetW(E.cellData(rec));
                if ((int32_t)((intptr_t)g + startCol) <= w) {
                    E.drawGlyph(&lineX, x, (int32_t)y, g, startCol, 5);
                    ret = (uint32_t)(uintptr_t)g; x += g[3]; i = next; continue;
                }
                E.drawGlyph(&lineX, x, (int32_t)y, nullptr, w, 5);
            }
            ret = (uint32_t)(uintptr_t)g; x += g[3];
        }
        i = next;
    }
    return ret;
}

// 1.13c D2Win+0x0E240
void d2win_sub_E240(uint32_t* w, uint32_t p2, uint32_t p3, const uint8_t* cols, const char* tx) {
    if (!w) { E.fatal(0xcd2); return; }
    uint32_t* s = (uint32_t*)E.alloc(0x85b, 0);
    if (!s) { E.fatal(0x85c); return; }
    *s = 0;
    w[0x2e] = (uint32_t)(uintptr_t)s;
    w[7] = p2;
    if (p2 < 7) { w[9] = E.fn_c180(p2); if (!w[9]) return; }
    else w[9] = 1;
    w[8] = p3;
    uint32_t v = w[0] | E.bitMasks[0]; v |= E.bitMasks[1]; w[0] = v | E.bitMasks[2];
    for (uint32_t i = 0; i < 16; i++) {
        w[0xe + 0x10 + i] = 1;
        if (cols[i] == 0 || cols[i] == 0xff) w[0xe + i] = 0;
        else { E.lookupColor(cols[i]); w[0xe + i] = E.palTable[cols[i] * 3]; w[0xe + 0x10 + i] = E.palTable[cols[i] * 3 + 2]; }
    }
    char* p = (char*)w + 0x28;
    for (int i = 0; i < 16; i++) { char c = tx[i]; p[i] = (c != -1) ? (char)(c - 1) : c; }
    if (E.fn_dff0() != 0) E.fn_c410(w, p3);
}

// 1.13c D2Win+0xa250 -- __fastcall(ecx=p1, edx=p2); hidden EBX (r5) and retaddr slot (r6) lead.
uint32_t* d2win_sub_A250(uint32_t r5, uint32_t r6, uint32_t p1, uint32_t p2, uint32_t p3, const uint32_t* p4) {
    uint32_t* o = (uint32_t*)E.alloc(0x24, 0);
    for (int i = 0; i < 0x16; i++) o[i] = 0;
    o[0] = 0xc;
    uint32_t m0 = E.bitMasks[0], m2 = E.bitMasks[2], old = o[2];
    o[2] = old | m0;
    o[0x13] = p3; o[6] = r6;
    o[2] = m2 | old | m0;
    o[3] = p1; o[4] = p2; o[5] = r5;
    // o[7] = &LAB_6f8ea0e0, o[9] = &LAB_6f8e9e10: code addresses, wired by the caller
    if (!p4) { o[0x14] = 1; o[0x15] = 0; } else { o[0x14] = p4[0]; o[0x15] = p4[1]; }
    if (!*E.listHead) { *E.listHead = o; return o; }
    uint32_t* t = *E.listHead;
    while (t[0xf]) t = (uint32_t*)(uintptr_t)t[0xf];
    t[0xf] = (uint32_t)(uintptr_t)o;
    return o;
}

// 1.13c D2Win+0x0a820
uint32_t d2win_sub_A820(int32_t expansion) {
    if (E.fog10117) E.fog10117();
    if (expansion == 0) { *E.mpqVideo = E.openMpq("D2Win.DLL", "d2video.mpq", "D2VIDEO"); return *E.mpqVideo != nullptr; }
    *E.mpqExpansion = E.openMpq("D2Win.DLL", "d2Xvideo.mpq", "D2EXPANSION");
    return *E.mpqExpansion != nullptr;
}

// 1.13c D2Win+0x0B3A0
void d2win_sub_B3A0(uint32_t* w, uint32_t v) {
    if (!w) { E.fatal(0xd81); return; }
    w[6] = v;
}

// 1.13c D2Win+0x0f6b0 -- __fastcall
void d2win_sub_F6B0(int32_t* w, int32_t v) {
    if (w[0] != 6) { E.fatal(0xd6); return; }
    w[0x17] = v;
}

// 1.13c D2Win+0x08a50 -- __fastcall(ecx unused)
void d2win_sub_8A50(uint32_t /*ecx*/, const char* s, int32_t a, int32_t b) {
    uint16_t buf[0x400];
    for (auto& c : buf) E.unicodeInit(&c);
    E.win2Unicode(buf, s, 0x400);
    E.sub_8920(buf, a, b);
}
