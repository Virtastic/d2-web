// D2Multi batch 15: 9550 (UI cel loads), 9D60 (chat-line join), AA60/ACB0 (control-set builders), B160 (date format).
// Retail D2Win ordinals are weak externs (not linked in the wasm build); globals are host-side stand-ins.
#include <cstdint>
#include <cstdio>
#include <cstring>
#define W __attribute__((weak))
extern "C" {
void* d2win_loadCel(const char*) W;                       // D2Win ord 10111
void d2win_setCelPair(void*, void*) W;                    // D2Win ord 10101
void* d2win_createControl(const void*) W;                 // D2Win ord 10098
void d2win_ctl10196(void*, int) W;
void d2win_ctl10001(void*, int) W;
void d2win_10000(int, int, int) W;
void d2win_10131(void) W;
void FUN_6f9d8790(void) W;
void d2multi_utf8ToUnicode(uint16_t*, const char*, int) W;
void d2multi_toUtf(char*, const uint16_t*, int) W;
void d2multi_fatal(void) W;                               // Ordinal_10265/10024 + _exit(-1)

void* d2multi_cel[64];                                    // DAT_6fa09f40..6fa0a088 slots, in table order
struct D2MultiChatNode { char text[0x204]; D2MultiChatNode* next; };
int32_t DAT_6fa09f88; D2MultiChatNode* DAT_6fa0a0f0;
void* DAT_6fa08e80[0x20]; uint32_t DAT_6fa09f24, DAT_6fa09f2c; void* DAT_6fa0a0e0;
}

// 1.13c D2Multi+0x9550
extern "C" void FUN_6f9d9550(void) {
    static const char* const n[] = {
        "UI\\FrontEnd\\PopUpLargest2","UI\\FrontEnd\\MediumButtonBlank2","UI\\BIGMENU\\bnet","UI\\BIGMENU\\waitingroombkgd",
        "UI\\CURSOR\\ohand","UI\\CURSOR\\lrotate","UI\\BIGMENU\\scrollbar","UI\\FRONTEND\\joingamescrollbars",
        "UI\\BIGMENU\\chatleftbuttons","UI\\BIGMENU\\chatleftbuttons1","UI\\BIGMENU\\chatleftbuttons2","UI\\BIGMENU\\chatrightbuttons",
        "UI\\BIGMENU\\chatrighttopbuttons","UI\\BIGMENU\\chatarrows","UI\\BIGMENU\\joingamebckg","UI\\BIGMENU\\GameButtonBlank",
        "UI\\BIGMENU\\joingameclickbox","UI\\BIGMENU\\numberarrows","UI\\BIGMENU\\numberarrows","UI\\BIGMENU\\numberarrows",
        "UI\\BIGMENU\\numberarrows","UI\\BIGMENU\\numbergreyarrows","UI\\BIGMENU\\GreyLevelBox","UI\\BIGMENU\\createjoinbckg",
        "UI\\BIGMENU\\okaybutton","UI\\BIGMENU\\creategamebckg","UI\\BIGMENU\\creategamebutton","UI\\BIGMENU\\widebuttonblank",
        "UI\\BIGMENU\\CancelButtonBlank","UI\\BIGMENU\\yesnobuttonblank","ui\\BIGMENU\\D2logoBlackBnet","ui\\BIGMENU\\D2logoBnet",
        "UI\\BIGMENU\\blankbckg2","UI\\BIGMENU\\gembutton","UI\\BIGMENU\\LadderChoiceEXP","UI\\BIGMENU\\HardcoreLadderBckg",
        "UI\\BIGMENU\\PlayerProfileBckg","UI\\BIGMENU\\PlayerProfileBckg","UI\\BIGMENU\\HelpBckg","UI\\BIGMENU\\radiobutton",
        "UI\\BIGMENU\\radiobuttonGrey","ui\\BIGMENU\\PopUpOK","ui\\BIGMENU\\CancelButtonBlank","ui\\BIGMENU\\overalltab",
        "ui\\BIGMENU\\amazontab","ui\\BIGMENU\\sorceresstab","ui\\BIGMENU\\necromancertab","ui\\BIGMENU\\paladintab",
        "ui\\BIGMENU\\barbariantab","ui\\BIGMENU\\druidtab","ui\\BIGMENU\\assassintab","UncompOverlays\\SquelchAura",
        "UncompOverlays\\SelectAura"};
    char p[260];
    for (unsigned i = 0; i < sizeof n / sizeof *n; i++) {
        snprintf(p, sizeof p, "%s\\%s", "DATA\\GLOBAL", n[i]);
        d2multi_cel[i] = d2win_loadCel ? d2win_loadCel(p) : nullptr;
    }
    if (d2win_setCelPair) d2win_setCelPair(d2multi_cel[51], d2multi_cel[52]);  // SquelchAura, SelectAura
}

// 1.13c D2Multi+0x9D60
extern "C" void FUN_6f9d9d60(void) {
    uint16_t u[1024] = {0};
    if (!DAT_6fa09f88) return;
    char s[512]; s[0] = 0;
    int n = 0;
    for (D2MultiChatNode* c = DAT_6fa0a0f0; c && n < 0x200; c = c->next) {
        strncat(s, c->text, 0x200 - n);
        n = (int)strlen(s);
    }
    if (d2win_10131) d2win_10131();
    if (d2multi_utf8ToUnicode) d2multi_utf8ToUnicode(u, s, 0x200);
    if (d2win_10000) d2win_10000(1, 0, 0);
}

// Shared body of AA60/ACB0: three controls appended to the 32-slot table; any overflow is fatal.
static void ctl3(const void* a, const void* b, const void* c, void* cb, bool tick) {
    if (FUN_6f9d8790) FUN_6f9d8790();
    const void* d[3] = {a, b, c};
    for (int i = 0; i < 3; i++) {
        if (DAT_6fa09f24 > 0x1f) { if (d2multi_fatal) d2multi_fatal(); return; }
        void* h = d2win_createControl ? d2win_createControl(d[i]) : nullptr;
        DAT_6fa08e80[DAT_6fa09f24++] = h;
        if (i == 0 && d2win_ctl10196) d2win_ctl10196(h, 0);
        if (d2win_ctl10001) d2win_ctl10001(h, 1);
    }
    if (d2win_10000) d2win_10000(0, 0, 0);
    DAT_6fa0a0e0 = cb;  // ponytail: retail takes the callback from unaff_EBX, default LAB_6f9daa50
    if (tick) DAT_6fa09f2c = 0;  // retail: GetTickCount()
}

// 1.13c D2Multi+0xAA60
extern "C" void FUN_6f9daa60(void* cb) { ctl3((void*)0x6f9ea360, (void*)0x6f9ea3c0, (void*)0x6f9ea3f0, cb, true); }
// 1.13c D2Multi+0xACB0
extern "C" void FUN_6f9dacb0(void* cb) { ctl3((void*)0x6f9ea360, (void*)0x6f9ea390, (void*)0x6f9ea3f0, cb, false); }

// 1.13c D2Multi+0xB160
extern "C" void FUN_6f9db160(const void* ft, char* out, int cap) {
    (void)ft; (void)out; (void)cap;  // ponytail: FILETIME->localized long date (GetDateFormatA lcid 0x400) is host-locale; host shim supplies it
}
