// 1.13c D2Client+0x1CE00: quest-log message for one quest. ESI = a 0x26A-byte result record whose
// dword at +1 is the quest id; picks the message-table row, asks D2Common#10174 (quest state) a few questions and fills the record
// (title id, text, sound id, flags, mode). Translated from the disassembly (the decompiler drops half of the branches).
#include <cstdint>
#include <cstddef>
#include <cstring>

struct Row { uint32_t id; uint8_t b262, b261; uint32_t ptr; };   // id, bytes at +0x8189 / +0x818a, message table address (0 = none)
static const Row kRow[41] = {{0,0,0,0x0},{1,0,0,0x6fba79fc},{2,1,1,0x6fba7a3c},{3,2,4,0x6fba7a7c},{4,3,2,0x6fba7abc},{5,4,3,0x6fba7afc},{6,5,5,0x6fba7b3c},{7,0,0,0x0},{8,0,0,0x0},{9,6,0,0x6fba7b7c},{10,7,1,0x6fba7bbc},{11,8,2,0x6fba7bfc},{12,9,3,0x6fba7c3c},{13,10,4,0x6fba7c7c},{14,11,5,0x6fba7cbc},{15,0,0,0x0},{16,0,0,0x0},{17,12,3,0x6fba7d3c},{18,13,2,0x6fba7d7c},{19,14,1,0x6fba7dbc},{20,15,0,0x6fba7dfc},{21,16,4,0x6fba7e3c},{22,17,5,0x6fba7e7c},{23,0,0,0x0},{24,0,0,0x0},{25,18,0,0x6fba7ebc},{26,20,2,0x6fba7efc},{27,19,1,0x6fba7f3c},{28,0,0,0x0},{29,0,0,0x0},{30,0,0,0x0},{31,0,0,0x0},{32,0,0,0x0},{33,0,0,0x0},{34,0,0,0x0},{35,21,0,0x6fba7f7c},{36,22,1,0x6fba7fbc},{37,23,2,0x6fba7ffc},{38,24,3,0x6fba803c},{39,25,4,0x6fba807c},{40,26,5,0x6fba80bc}};
static const uint32_t kRegionLo = 0x6fba79fc;   // message tables 0x6FBA79FC .. 0x6FBA8184 (words)
static const uint16_t kRegion[964] = {3714,76,4,3735,64,3736,64,3737,64,3738,64,3740,64,3725,3725,3725,3725,3725,3725,3725,3725,3740,64,3728,3725,3727,3725,3726,64,0,0,0,3715,92,2,3741,81,3742,81,3743,81,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3743,81,3728,3725,3727,3725,3726,81,0,0,0,3716,163,65535,3755,146,3756,146,3733,146,3732,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3757,163,3728,3725,3727,3725,3726,146,0,0,0,3717,123,65535,3744,97,3745,97,3746,97,3747,97,3748,97,3749,97,3734,97,3725,3725,3725,3725,3750,97,3728,3725,3727,3725,3726,97,0,0,0,3718,127,65535,3751,127,3754,127,3752,127,3753,127,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3728,3725,3727,3725,3726,127,0,0,0,3719,184,9,3758,166,3759,166,3761,166,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3760,166,3728,3725,3727,3725,3726,184,0,0,0,923,334,2,941,304,942,304,943,304,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,943,3725,3728,3725,3727,3725,3726,304,0,0,0,924,339,65535,944,3725,945,335,946,338,948,3725,947,339,947,3725,3725,3725,3725,3725,3730,3725,947,3725,3728,3725,3727,3725,3726,339,0,0,0,925,3725,65535,950,3725,951,3725,952,348,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,952,3725,3728,3725,3727,3725,3726,3725,0,0,0,926,3725,65535,954,3725,953,373,953,377,955,377,956,396,3725,3725,3725,3725,3725,3725,3725,3725,956,396,3728,396,3727,3725,3726,396,0,0,0,927,3725,65535,957,3725,958,3725,3726,3725,959,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,959,3725,3728,3725,3727,3725,3726,3725,0,0,0,928,442,4,960,430,962,431,11030,431,964,431,965,302,966,442,961,396,3729,3725,3730,3725,965,3725,3728,3725,3727,3725,3726,442,0,0,0,929,3725,65535,967,465,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,0,0,0,930,564,65535,968,549,969,549,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3730,3725,3729,3725,3725,3725,3728,3725,3727,3725,3726,3725,0,0,0,931,548,65535,970,543,971,543,972,543,973,543,974,543,975,548,976,3725,3725,3725,3725,3725,3725,3725,3728,3725,3727,3725,3726,3725,0,0,0,932,587,65535,993,3725,977,571,978,571,979,571,980,587,981,587,3730,3725,3729,3725,3725,3725,3725,3725,3725,3725,3728,3725,3727,3725,3726,587,0,933,3725,4,982,3725,983,527,984,529,985,531,986,534,3729,3725,3730,3725,3725,3725,3725,3725,986,534,3728,3725,3727,3725,3726,3725,0,0,0,934,3725,3,994,3725,987,594,988,594,989,594,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,989,594,3728,626,3727,3725,3726,626,0,0,0,935,3725,4,995,3725,990,628,991,628,992,628,935,628,3729,3725,3730,3725,3725,3725,3725,3725,3728,3725,3728,3725,3727,3725,3726,3725,0,0,0,937,675,65535,996,670,997,670,998,670,999,675,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,999,3725,3728,3725,3727,3725,3726,676,0,0,0,938,3725,65535,1004,681,1005,681,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3725,3728,3725,3727,3725,3726,3725,0,0,0,939,3725,65535,1000,678,1001,678,1002,678,1000,679,3725,3725,3729,3725,3730,3725,3725,3725,3725,3725,3728,3725,3728,3725,3727,3725,3726,3725,0,0,0,22618,3725,65535,22619,20077,22620,20077,22621,20077,21786,20090,3725,3725,3729,3725,3730,3725,3725,3725,3725,3725,21786,20090,3728,3725,3727,3725,3726,3725,0,0,0,22622,3725,65535,22623,20096,22624,20104,22625,20096,22626,20096,21789,20096,3729,3725,3730,3725,3725,3725,3725,3725,22625,20096,3728,3725,3727,3725,3726,3725,0,0,0,22627,3725,65535,22628,20116,21788,20116,22629,20116,22630,20116,22631,20116,22632,20116,3730,3725,3725,3725,3725,3725,22631,20116,3728,3725,3727,3725,3726,3725,0,0,0,22633,3725,65535,22634,20137,21787,20137,22635,20137,22636,20137,21790,20148,3729,3725,3730,3725,3725,3725,3725,3725,21790,3725,3728,3725,3727,3725,3726,3725,0,0,0,22637,3725,3,22638,20153,22639,20153,22640,20002,22639,20002,3725,3725,3729,3725,3730,3725,3725,3725,3725,3725,22639,20002,3728,3725,3727,3725,3726,3725,0,0,0,22641,3725,65535,21792,20169,21881,20169,22643,20169,22644,20169,22645,20169,3725,3725,3725,3725,3725,3725,3725,3725,3728,3725,3728,3725,3727,3725,3726,3725,0,0,0,0,0,26,0,121,0,32,0,65,0,123,0,121,0,128,0,65,0,220,0,121,0,224,0,65,0,26,0,218,0,32,0,163,0,123,0,218,0,128,0,163,0,220,0,218,0,224,0,163,0,0,0,5,0,6,0,11,0,12,0,17,0,18,0,20,0,21,0};
struct QuestLogOps {
    void* c;
    int (*qs)(void*, uint32_t rec, uint32_t quest, uint32_t state);             // D2Common#10174 via thunk +0xC58C (stdcall)
    uint32_t (*lang)(void*, uint32_t id);                                       // D2Lang#10003 via thunk +0xD1EC (fastcall ecx)
    void (*ncpy)(void*, uint8_t* dst, uint32_t src, uint32_t n);                // Unicode::strncpy, IAT +0xCEAA8
    void (*ctor)(void*, uint8_t* p);                                            // Unicode default ctor, IAT +0xCEB00
    void (*csprintf)(void*, uint8_t* buf, int32_t v);                           // _sprintf(buf, "%d", v), +0x3146
    void (*touni)(void*, uint8_t* dst, uint8_t* src, uint32_t n);               // Unicode::toUnicode, IAT +0xCEAC4
    void (*ucpy)(void*, uint8_t* dst, uint32_t src);                            // Unicode::strcpy, IAT +0xCEAB8
    int (*ulen)(void*, uint32_t s);                                             // Unicode::strlen, IAT +0xCEAB0
    void (*ucat)(void*, uint8_t* dst, uint8_t* src);                            // Unicode::strcat, IAT +0xCEABC
    void (*usprintf)(void*, uint32_t n, uint8_t* dst, uint32_t fmt, uint32_t a);// Unicode::sprintf, IAT +0xCEAB4 (cdecl)
    void (*fatal)(void*);                                                       // the string-too-long exit (never returns in retail)
};
struct QuestLogGlobals {
    uint32_t rec1, rec2;   // +0x11973B, +0x11973F (quest records)
    int32_t g23e4;         // +0x1223E4
    uint32_t g23ec;        // +0x1223EC
    uint32_t g394;         // +0x11C394
    const uint8_t* init;   // +0x122496 (41 bytes)
    uint8_t* state;        // +0x1224C0 (41 bytes, read and written)
};
static inline uint16_t rw(uint32_t p) { return kRegion[(p - kRegionLo) / 2]; }
static inline void w16(uint8_t* s, uint32_t o, uint16_t v) { memcpy(s + o, &v, 2); }
static inline void w32(uint8_t* s, uint32_t o, uint32_t v) { memcpy(s + o, &v, 4); }

void d2client_sub_1CE00(uint8_t* S, const QuestLogGlobals* g, const QuestLogOps* o, uint8_t* scratch) {
    void* c = o->c;
    uint32_t qid, ptr, A, Bv, C, id, tmp;
    uint8_t st, bl;
    int idx, r;
    uint32_t e, ebp;
    uint16_t sv, ax;
    uint8_t* const T = S + 7;
    memcpy(&id, S + 1, 4);
    w16(S, 5, 0xE8C); w16(S, 0x25F, 0xE8C); w16(S, 7, 0); S[0x263] = 0; S[0x264] = 0;
    for (idx = 0; idx < 41; ++idx) if (kRow[idx].id == id) break;
    if (idx == 41) { w32(S, 0x266, 2); return; }
    S[0x261] = kRow[idx].b261; S[0x262] = kRow[idx].b262; st = g->init[idx]; S[0x265] = st;
    ptr = kRow[idx].ptr;
    if (!ptr) goto L6cc;
    qid = kRow[idx].id;
    A = o->qs(c, g->rec1, qid, 0);
    Bv = o->qs(c, g->rec1, qid, 0xD);
    C = o->qs(c, g->rec1, qid, 1);
    if (ptr == 0x6fba7f7c && C) { if (!st) st = 4; goto L28d; }
    if (ptr == 0x6fba7cbc && !A) {
        if (Bv) goto L07c;
        r = o->qs(c, g->rec1, 0xC, 0);
        if (!r) { r = o->qs(c, g->rec1, 0xC, 0xD); if (!r) goto Lfa9; }
        if (st != 1) goto Lfa9;
        w16(S, 5, rw(ptr)); o->ncpy(c, T, o->lang(c, rw(ptr + 0x1E)), 0x12C); w16(S, 0x25D, 0);
        st = 7; g->state[idx] = 7; w16(S, 0x25F, rw(ptr + 0x20)); goto Ld046;
    Lfa9:
        if (o->qs(c, g->rec1, 0xE, 3) == 1) {
            w16(S, 5, rw(ptr)); o->ncpy(c, T, o->lang(c, rw(ptr + 0x16)), 0x12C); w16(S, 0x25D, 0);
            st = 5; g->state[idx] = 5; w16(S, 0x25F, rw(ptr + 0x18)); goto Ld046;
        }
        if (o->qs(c, g->rec1, 0xE, 4) == 1) {
            w16(S, 5, rw(ptr)); o->ncpy(c, T, o->lang(c, rw(ptr + 0x1A)), 0x12C); w16(S, 0x25D, 0);
            st = 6; g->state[idx] = 6; w16(S, 0x25F, rw(ptr + 0x1C)); goto Ld046;
        }
        goto L18c;
    Ld046:
        w32(S, 0x266, 3); S[0x264] = 1; S[0x265] = st; return;
    }
    if (!Bv) {   // d116
        if (!A) goto L18c;
        sv = 0xE90; st = 0xB; goto Ld124;
    }
    if (A) { sv = 0xE8E; st = 0xD; goto Ld124; }
L07c:
    if (rw(ptr + 4) == 0xFFFF) goto L18c;
    if (!C) goto L281;
    if (ptr == 0x6fba7ebc) goto L18c;
    bl = (uint8_t)(rw(ptr + 4) & 0xFF) + 1;
    e = ptr + bl * 4u;
    w16(S, 5, rw(ptr)); o->ncpy(c, T, o->lang(c, rw(e + 2)), 0x12C); w16(S, 0x25D, 0);
    S[0x263] = bl != g->state[idx];
    ax = rw(e + 4); g->state[idx] = bl; S[0x265] = bl; w16(S, 0x25F, ax); w32(S, 0x266, 3); S[0x264] = 1; return;
Ld124:
    S[0x265] = st;
    w16(S, 5, rw(ptr)); o->ncpy(c, T, o->lang(c, sv), 0x12C);
    w16(S, 0x25D, 0); w16(S, 0x25F, rw(ptr + 2)); g->state[idx] = st; S[0x264] = 1; goto L56d;
L18c:
    if (!C) goto L281;
    if (!o->qs(c, g->rec1, qid, 0xF)) goto L281;
    bl = 0xA;
    if (id == 0x25) {
        if (!o->qs(c, g->rec1, 0x25, 8) || o->qs(c, g->rec1, 0x25, 9)) {
            if (!o->qs(c, g->rec1, 0x25, 8)) bl = 5;
        } else bl = 6;
    }
    if (id == 0x22) bl = (o->qs(c, g->rec1, 0x26, 4) != 0) + 4;
    S[0x265] = bl;
    if (rw(ptr + bl * 4u + 2) == 0xE8D) goto L6cc;
    ebp = ptr + bl * 4u;
    w16(S, 5, rw(ptr)); o->ncpy(c, T, o->lang(c, rw(ebp + 2)), 0x12C); w16(S, 0x25D, 0);
    w16(S, 0x25F, rw(ebp + 4)); g->state[idx] = bl; w32(S, 0x266, (id != 0x1B) * 2 + 1); S[0x264] = 1; return;
L281:
    if (!st) goto L58f;
L28d:
    ax = rw(ptr);
    if (ax != 0xE82) goto L40b;
    tmp = o->lang(c, rw(ptr + st * 4u + 2));
    if (st == 3 || st == 4) {
        if (g->g23e4 <= 1) { tmp = o->lang(c, 0xE9B); o->ncpy(c, T, tmp, 0x12C); w16(S, 0x25D, 0); }
        else {
            for (int i = 0; i < 10; ++i) o->ctor(c, scratch + 8 + 2 * i);
            o->csprintf(c, scratch, g->g23e4);
            o->touni(c, scratch + 8, scratch, 9);
            o->ucpy(c, T, tmp);
            if (o->ulen(c, tmp) >= 0x118) { o->fatal(c); return; }
            o->ucat(c, T, scratch + 8);
        }
    } else { o->ncpy(c, T, tmp, 0x12C); w16(S, 0x25D, 0); }
    w16(S, 5, rw(ptr)); S[0x263] = st != g->state[idx]; g->state[idx] = st;
    w16(S, 0x25F, rw(ptr + st * 4u + 4)); S[0x265] = st; goto L554;
L40b:
    w16(S, 5, ax);
    if (!A && !Bv && !o->qs(c, g->rec1, qid, 1) && g->rec2 && o->qs(c, g->rec2, qid, 0xD) && id != 0x13) goto Ld466;
    if (o->qs(c, g->rec1, qid, 0xE)) {
    Ld466: sv = (g->g394 != 0) + 0xE91;
    } else {
        sv = rw(ptr + st * 4u + 2);
        if (sv == 0xE8F && !g->g394) sv = 0xE91;
    }
    o->ncpy(c, T, o->lang(c, sv), 0x12C);
    if (rw(ptr) == 0x585E && sv == 0x5860) {
        if (!g->g23ec) o->ncpy(c, T, o->lang(c, 0xE91), 0x12C);
        else o->usprintf(c, 0x12C, T, o->lang(c, 0x5860), g->g23ec);
    }
    w16(S, 0x25D, 0); S[0x263] = 0;
    if (st != g->state[idx]) S[0x263] = 1;   // ptr != 0 here
    ax = rw(ptr + st * 4u + 4); g->state[idx] = st; w16(S, 0x25F, ax);
L554:
    w32(S, 0x266, 3); S[0x264] = 1;
    if (st != 0xD) return;
L56d:
    w32(S, 0x266, o->qs(c, g->rec1, qid, 0xC) != 0); return;
L58f:
    if (o->qs(c, g->rec2, qid, 0xD)) {
        if (id == 0x15 && !o->qs(c, g->rec1, 0x15, 0) && o->qs(c, g->rec1, 0x15, 4) == 1) { sv = 0x3DD; goto L68d; }
        if (Bv) goto L6cc;
        if (o->qs(c, g->rec1, qid, 1)) goto L6cc;
        sv = g->g394 ? 0xE92 : 0xE91; goto L68d;
    }
    if (!o->qs(c, g->rec2, qid, 0xF)) goto L6cc;
    if (Bv) goto L6cc;
    if (o->qs(c, g->rec1, qid, 1)) goto L6cc;
    sv = g->g394 ? 0xE92 : 0xE91;
L68d:
    o->ncpy(c, T, o->lang(c, sv), 0x12C);
    w16(S, 0x25D, 0); w16(S, 0x25F, 0xE8D); S[0x264] = 1; w32(S, 0x266, 3); return;
L6cc:
    w32(S, 0x266, 2);
}
