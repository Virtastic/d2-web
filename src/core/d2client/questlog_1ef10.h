#pragma once
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
