// 1.13c D2Client+0x1EF10: the Quest Log panel's per-frame draw.
// Background, the five act tabs, the per-quest icon rows and their sockets, the selected quest's title
// and wrapped description, the buy/sell button and the two hover tooltips. No args, plain ret.
// Every draw is D2Gfx#10041 (thunk +0xD096) with a zeroed 0x48-byte context (frame at +0x00, cel at
// +0x34) and the trailing (x, y, palette, mode, 0); modelled here as drawCel(cel, frame, x, y,
// palette, mode). Row x is +ox and row y is -oy, exactly as the retail instructions compute them.
// Global RVAs are on base 0x6FAB0000.
#include <cstdint>
#include <cstddef>
#include <cstring>

// The six 0x26A-byte result records at 0x1224E9, as sub_1CE00 leaves them: byte +0 = filled, word +5 =
// title string id (0xE8C = none), +7 = the wide description text (first word zero = empty), byte
// +0x261 = row id, byte +0x265 = state, dword +0x266 = mode (0 = in progress, 1/2/3 = the variants).
struct QuestPanelRec { uint8_t b[0x26A]; };
static_assert(sizeof(QuestPanelRec) == 0x26A, "record stride");

// A 16-byte row of the 0x6FBA8188 table: +0 flag, +1 icon index into the 0x122421 handles, +2 row id
// (index into the 0x6FBA8100 position table and the id sub_1DA90 writes to 0x1223F9), +3 act, +12 the
// quest id copied into record+1.
struct QuestPanelRow { uint8_t flag, icon, rowId, act, pad[8]; uint32_t quest; };
static_assert(offsetof(QuestPanelRow, quest) == 12 && sizeof(QuestPanelRow) == 16, "row layout");

// The 16-byte rows of the position table at 0x6FBA8100.
struct QuestPanelPos { int32_t x, y; uint8_t pad[8]; };
static_assert(sizeof(QuestPanelPos) == 16, "position row");

struct QuestPanelGlobals {
    uint32_t* mode;          // 0x11BBFC; [mode+0x10] == 0x11 means another screen owns the frame
    uint32_t  skipA;         // 0xFAADC8
    uint8_t   skipB;         // 0x10F2F3
    uint32_t  celBg;         // 0x122415 quest background
    uint32_t  celTabs;       // 0x122419 act tabs
    uint32_t  celDone;       // 0x12241D quest done
    uint32_t  celSockets;    // 0x122411 row socket
    uint32_t  celLast;       // 0x12248D "last" button
    uint32_t  celInvps;      // 0x122491
    uint32_t  celBuy;        // 0x11B9C4 buy/sell button (cached)
    uint32_t* icon;          // 0x122421 27 loaded per-quest icon cels
    uint32_t  expansion;     // 0x119854 second expansion test
    uint32_t  selAct;        // 0x123395 selected act
    int32_t   ox, oy;        // 0x11B9A0, 0x11B9A4 panel origin
    int32_t   height;        // 0xDBC4C screen height
    uint32_t  count;         // 0x1223E0 rows drawn
    uint32_t  selRow;        // 0x1223F9 selected row id (0xFFFFFFFF = none)
    uint32_t  prevRow;       // 0x1223F5 previously clicked row
    int32_t   doneDX;        // 0x1234BC x offset of the done icon
    int32_t   socketDY;      // 0x1234B8 socket y offset when the row is not the selected one
    int32_t   iconDY;        // 0xFA3F8 socket y offset when the row is the selected one
    int32_t   doneDY;        // 0xFA3FC y offset of the done icon
    uint8_t   descOff;       // 0x1223F3 description suppressed
    uint8_t   buyPressed;    // 0x1223F2 buy/sell button pressed
    uint8_t   lastPressed;   // 0x1223F4 "last" button pressed
    int32_t   mouseX, mouseY;// 0x11B828, 0x11B824
    int32_t   invpsFrame;    // 0x1223E8
    uint32_t  frame[5];      // 0x123365 per-row frame counter
    uint32_t  tick[5];       // 0x12337D per-row last 100 ms deadline tick
    uint32_t  questRec;      // 0x11973B D2BitBuffer* of the quest record
    const QuestPanelRow* rows;   // 0x6FBA8188, the 41-entry table plus the 42nd entry the walk reads
    const QuestPanelPos* pos;    // 0x6FBA8100 (0x6FBA8188 == 0x6FBA8100+0x88: one blob covers both)
    int32_t   lineY, titleY, lineX;   // 0xFA3EC (description line base y), 0xFA3F0, 0xFA3F4
    QuestPanelRec rec[6];    // 0x1224E9
};

struct QuestPanelOps {
    void* c;
    // D2Gfx#10041, thunk +0xD096. The retail call passes a 0x48-byte context (frame +0x00, cel +0x34)
    // followed by (x, y, palette, mode, 0).
    void (*drawCel)(void* c, uint32_t cel, uint32_t frame, int32_t x, int32_t y, int32_t palette, int32_t mode);
    void (*setFont)(void* c, int32_t size);                       // D2Win#10184, +0xD3A8
    int  (*questState)(void* c, uint32_t rec, uint32_t quest, uint32_t what);  // D2Common#10174, +0xC58C
    void (*questSetBit)(void* c, uint32_t rec, uint32_t quest, uint32_t bit);  // D2Common#10003, +0xC904
    uint32_t (*langStr)(void* c, uint32_t id);                    // D2Lang#10003, +0xD1EC
    int  (*isExpansion)(void* c);                                 // Fog#10227, +0xBF8C
    void (*memFree)(void* c, void* p, const char* file, int line, int zero);  // Fog#10043, +0xBF86
    // D2Win#10028, +0xD35A: width in pixels of the wide string in ECX; the other args ride in registers
    // at the call site and are pinned by the capture.
    int  (*textLen)(void* c, uint32_t wide);
    // D2Win#10150, +0xD37E: print the wide string (ECX) at x (EDX), y. Same caveat as textLen.
    void (*printStr)(void* c, uint32_t wide, int32_t x, int32_t y);
    // D2Win#10085, +0xD384: popup text (ECX) at x (EDX), y, then the two trailing dwords 0 and 1.
    void (*popup)(void* c, uint32_t wide, int32_t x, int32_t y, int32_t a, int32_t b);
    uint32_t (*wrap)(void* c, uint32_t wide, uint32_t* ctl, uint32_t width);   // D2Win#10059, +0xD378
    uint32_t (*tickCount)();                                      // GetTickCount
    void (*buildRecord)(uint8_t* rec);                            // 0x1CE00, ESI = the record
    void (*loadPanel)();                                          // 0x1D930
    void (*chooseRow)(uint32_t act, uint32_t reset);              // 0x1DA90, ret 8
    void (*refresh)(uint32_t a);                                  // 0x1E7E0
    void (*sendQuestPacket)(uint16_t quest);                      // 0x14A10, AL = 0x58
    uint32_t (*loadCelPath)(void* c, const char* path);           // 0x2B420
    void (*sound)(uint32_t id);                                   // 0x88A70, id in EBX + 4 zero words
    void (*formatPath)(void* c, char* buf, const char* fmt, const char* arg);  // wsprintfA, +0x7F114
    const char* buysellFmt;   // 0x6FB80564 "%s\\ui\\panel\\buysellbtn"
    const char* dataGlobal;   // 0x6FB8357C "DATA\\GLOBAL"
    const char* fogFile;      // 0xFB86DCC the file string Fog#10043 records
};

namespace {
// x pads for the five act tabs: the expansion tabs sit wider apart than the four base-game tabs.
const int32_t kPadExp[5] = {5, 0x43, 0x81, 0xBF, 0xFD};
const int32_t kPadBase[5] = {5, 0x52, 0x9F, 0xEC, 0xEC};
// The quest-state question the non-selected act tabs ask (tab 0 draws unconditionally).
const uint32_t kActQuestion[5] = {0, 7, 0xF, 0x17, 0x1A};
const int32_t kHoverAY = 0x1030, kHoverBY = 0xE88;   // the two tooltip string ids

int32_t r16(const uint8_t* p) { int16_t v; memcpy(&v, p, 2); return v; }
uint32_t r32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
void w32(uint8_t* p, uint32_t v) { uint32_t x = v; memcpy(p, &x, 4); }
}   // namespace

void d2client_sub_1EF10(QuestPanelGlobals* g, const QuestPanelOps* o) {
    void* const c = o->c;

    // 1/2. gate: no mode object, the other screen's mode 0x11, or either skip flag -> refresh and out
    if (g->mode == nullptr || g->mode[4] == 0x11 || g->skipA != 0 || g->skipB != 0) {
        o->refresh(0);
        return;
    }

    // 3. load the panel cels on first use
    if (g->celBg == 0) {
        o->loadPanel();
        o->chooseRow(0, 1);
        if (g->celBg == 0) goto tabs;   // still nothing to draw with
    }

    // 4. four background tiles: frames 0..3 at the panel's four quadrants
    o->drawCel(c, g->celBg, 0, g->ox, g->oy + g->height - 0xE0, -1, 5);
    o->drawCel(c, g->celBg, 1, g->ox + 0x100, g->oy + g->height - 0xE0, -1, 5);
    o->drawCel(c, g->celBg, 2, g->ox, g->oy + g->height - 0x30, -1, 5);
    o->drawCel(c, g->celBg, 3, g->ox + 0x100, g->oy + g->height - 0x30, -1, 5);

tabs:
    // 5. the act tabs
    if (g->celTabs != 0) {
        const bool exp = o->isExpansion(c) != 0 && g->expansion != 0;
        const int32_t* pad = exp ? kPadExp : kPadBase;
        const int32_t yTab = g->oy + g->height - 0x1C0 + (exp ? 1 : 0);
        for (uint32_t act = 0; act < 5; ++act) {
            if (!exp && act > 3) break;                       // the base game has four acts
            if (act == g->selAct) {
                o->drawCel(c, g->celTabs, act * 2, g->ox + pad[act], yTab, -1, 5);
                continue;
            }
            bool draw;
            if (act == 0) draw = true;                         // act 1's tab always draws
            else if (act == 4) draw = exp && o->questState(c, g->questRec, kActQuestion[4], 0) != 0;
            else draw = o->questState(c, g->questRec, kActQuestion[act], 0) != 0;
            if (draw) o->drawCel(c, g->celTabs, act * 2 + 1, g->ox + pad[act], yTab, -1, 5);
        }
    }

    // 6. the result records are rebuilt from scratch every frame
    memset(g->rec, 0, sizeof g->rec);
    bool found3 = false;

    // 7. the icon rows of the selected act
    if (g->selAct < 5) {
        g->count = 0;
        // 42 rows: the retail guard is a post-increment (`cmp ecx,0x29; jg`), so row index 41 is read
        // one 16-byte entry past the 41-entry table.
        for (uint32_t row = 0; row < 42 && g->count < 6; ++row) {
            const QuestPanelRow& r = g->rows[row];
            if (!(r.act == g->selAct && r.flag != 0 && r.icon < 0x1B && g->icon[r.icon] != 0)) continue;
            uint8_t* const rec = g->rec[g->count].b;
            const QuestPanelPos& p = g->pos[r.rowId];
            w32(rec + 1, r.quest);
            o->buildRecord(rec);
            switch (r32(rec + 0x266)) {
            case 0:
                rec[0] = 1;
                {
                    uint32_t frame = g->frame[g->count];
                    if ((int32_t)frame >= 0x19) {
                        frame = 0x18;
                        o->questSetBit(c, g->questRec, r.quest, 0xC);   // complete it in the quest record
                        o->sendQuestPacket((uint16_t)r.quest);
                    }
                    o->drawCel(c, g->icon[r.icon], frame, p.x + g->ox, p.y - g->oy, -1, 5);
                }
                {
                    const uint32_t now = o->tickCount();
                    if (g->tick[g->count] == 0) g->tick[g->count] = now;
                    if (now - g->tick[g->count] > 100) {          // one animation step per 100 ms
                        g->tick[g->count] = now;
                        ++g->frame[g->count];
                        if (g->frame[g->count] == 1 && o->isExpansion(c) != 0)
                            o->sound(0x0E);                       // the quest-completed sound
                    }
                }
                break;
            case 1:
                rec[0] = 1;
                if (r.rowId == g->prevRow) {
                    o->drawCel(c, g->celDone, r.icon, p.x + g->doneDX + g->ox, p.y - g->oy + g->doneDY, -1, 5);
                } else {
                    o->drawCel(c, g->icon[r.icon], 0x18, p.x + g->ox, p.y - g->oy, -1, 5);
                }
                break;
            case 2:
                rec[0] = 1;
                o->drawCel(c, g->icon[r.icon], 0x1A, p.x + g->ox, p.y - g->oy, -1, 5);
                break;
            case 3:
                if (r16(rec + 5) == 0x3A0 && rec[0x265] == 7) found3 = true;
                rec[0] = 1;
                // frame 0x19 when this row is the previously clicked one, 0 otherwise (setne/dec/and 0x19)
                o->drawCel(c, g->icon[r.icon], (uint32_t)(((r.rowId != g->prevRow) - 1) & 0x19),
                           p.x + g->ox, p.y - g->oy, -1, 5);
                break;
            default:   // the record did not resolve: no icon, but the socket still draws
                break;
            }
            // the row's socket: frame 1 and one y offset for the selected row, 0 and the other otherwise
            {
                const bool sel = r.rowId == g->selRow;
                o->drawCel(c, g->celSockets, (uint32_t)sel, p.x - 4 + g->ox,
                           p.y - g->oy + (sel ? g->iconDY : g->socketDY) + 5, -1, 5);
            }
            ++g->count;
        }
        // the completion tally cel, only for a completed quest in the tally row
        if (found3 && g->selRow == 5) {
            int32_t f = g->invpsFrame;
            if (f < 0 || f > 6) f = 0;
            o->drawCel(c, g->celInvps, (uint32_t)f, g->ox + 0x6E, g->oy + g->height - 0x6E, -1, 3);
        }
    }

    // 8. the text below uses font 8
    o->setFont(c, 8);

    // 9. the selected quest's title and wrapped description
    if (g->selRow != 0xFFFFFFFFu && (int32_t)g->selRow < 6 && g->selRow < g->count && g->count > 0) {
        for (uint32_t i = 0; i < g->count; ++i) {
            uint8_t* const rec = g->rec[i].b;
            if (rec[0x261] != g->selRow || rec[0] == 0) continue;
            if (r16(rec + 5) != 0xE8C) {
                const uint32_t title = o->langStr(c, (uint32_t)r16(rec + 5));
                const int32_t w = o->textLen(c, title);          // width in pixels, y in a register arg
                const int32_t x = g->ox - w / 2 + 0xA0;          // cdq/sar: truncating division
                o->printStr(c, title, x, g->titleY - g->oy);
            }
            if (g->descOff == 0 && r16(rec + 7) != 0) {
                uint32_t ctl = 0;
                uint32_t node = o->wrap(c, (uint32_t)(uintptr_t)(rec + 7), &ctl, 0x10E);
                int32_t k = 0;
                while (node != 0) {
                    const uint32_t line = r32((const uint8_t*)(uintptr_t)node);
                    const uint32_t next = r32((const uint8_t*)(uintptr_t)(node + 4));
                    o->printStr(c, line, g->ox + g->lineX, g->lineY + k - g->oy);
                    o->memFree(c, (void*)(uintptr_t)line, o->fogFile, 0x8D1, 0);
                    o->memFree(c, (void*)(uintptr_t)node, o->fogFile, 0x8D2, 0);
                    node = next;
                    k += 0x14;
                }
            }
            break;
        }
    }

    // 10. the buy/sell button, loaded on first use
    if (g->celBuy == 0) {
        char buf[260];
        o->formatPath(c, buf, o->buysellFmt, o->dataGlobal);
        g->celBuy = o->loadCelPath(c, buf);
    }
    o->drawCel(c, g->celBuy, (uint32_t)(g->buyPressed != 0) + 10, g->ox + 0x116, g->oy + g->height - 0x3A, -1, 5);

    // 11. hover A: the buy/sell button's tooltip
    const int32_t mx = g->mouseX - g->ox;
    const int32_t my = g->mouseY + g->oy;
    if (mx > 0x115 && mx < 0x13A && my > 0x187 && my < 0x1AA) {
        o->setFont(c, 9);
        o->popup(c, o->langStr(c, (uint32_t)kHoverAY), g->ox + 0x128, g->oy + g->height - 0x5F, 0, 1);
    }

    // 12. the "last" button
    o->drawCel(c, g->celLast, (uint32_t)(g->lastPressed != 0), g->ox + 0xE2, g->oy + g->height - 0x3A, -1, 5);

    // 13. hover B: the "last" button's tooltip
    if (mx > 0xE5 && mx < 0x104 && my > 0x186 && my < 0x1A8) {
        o->setFont(c, 9);
        o->popup(c, o->langStr(c, (uint32_t)kHoverBY), g->ox + 0xF1, g->oy + g->height - 0x5F, 0, 1);
    }
}
