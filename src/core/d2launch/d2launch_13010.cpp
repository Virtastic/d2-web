// 1.13c D2Launch+0x13010 (sub_13010). No args, plain ret. The frontend cel files: each path is formatted into one 260-byte local
// (wsprintfA "%s\\..." with "DATA\\GLOBAL"; the joingamescrollbars path goes through the CRT _sprintf 0x101B, same output) and handed to
// D2Win 10111 (thunk 0x9946, fastcall ECX = path, EDX = 0); the handle goes to a global in 0x25BE4..0x25CC8. Fog 10227 (thunk 0x980E,
// called afresh at each of its seven sites) picks the expansion variant; at the fourth site it gates four extra files.
#include <cstdint>

struct D2l13010Seams {
    uint32_t (*isExp)();                      // thunk 0x980E (Fog 10227)
    uint32_t (*load)(const char* path);       // thunk 0x9946 (D2Win 10111), EDX = 0
};

namespace {
const char kBase[] = "DATA\\GLOBAL";
// g: the globals from 0x25BE4 (word index = (rva - 0x25BE4) / 4)
void put(uint32_t* g, uint32_t rva, const char* tail, const D2l13010Seams* s) {
    char b[260];
    uint32_t n = 0;
    for (const char* p = kBase; *p; ++p) b[n++] = *p;
    for (const char* p = tail; *p; ++p) b[n++] = *p;
    b[n] = 0;
    g[(rva - 0x25be4u) / 4] = s->load(b);
}
}

extern "C" void d2l_13010(uint32_t* g, const D2l13010Seams* s) {
    put(g, 0x25be4, "\\UI\\FrontEnd\\blizno", s);
    put(g, 0x25be8, s->isExp() ? "\\UI\\FrontEnd\\trademarkscreenEXP" : "\\UI\\FrontEnd\\trademark", s);
    put(g, 0x25c08, "\\UI\\CURSOR\\ohand", s);
    put(g, 0x25bf0, "\\ui\\FrontEnd\\Diablo2", s);
    put(g, 0x25bf4, "\\ui\\FrontEnd\\D2logoBlackLeft", s);
    put(g, 0x25bf8, "\\ui\\FrontEnd\\D2logoFireLeft", s);
    put(g, 0x25bfc, "\\ui\\FrontEnd\\D2logoBlackRight", s);
    put(g, 0x25c00, "\\ui\\FrontEnd\\D2logoFireRight", s);
    put(g, 0x25c04, s->isExp() ? "\\ui\\FrontEnd\\gameselectscreenEXP" : "\\ui\\FrontEnd\\TitleScreen", s);
    put(g, 0x25c0c, s->isExp() ? "\\ui\\CharSelect\\creditsbckgexpand" : "\\ui\\CharSelect\\creditsbckg", s);
    put(g, 0x25c24, "\\ui\\FrontEnd\\WideButtonBlank", s);
    put(g, 0x25c40, "\\ui\\FrontEnd\\WideButtonBlank02", s);
    put(g, 0x25c44, "\\ui\\FrontEnd\\NarrowButtonBlank", s);
    put(g, 0x25c48, "\\ui\\FrontEnd\\GWListBox", s);
    if (s->isExp()) {
        put(g, 0x25c18, "\\ui\\CharSelect\\Realmselectbutton", s);
        put(g, 0x25c1c, "\\ui\\CharSelect\\TallButtonBlank", s);
        put(g, 0x25c20, "\\ui\\CharSelect\\characterselectscreenEXP", s);
        put(g, 0x25c60, "\\ui\\FrontEnd\\CinematicsSelectionEXP", s);
    }
    put(g, 0x25c28, "\\ui\\FrontEnd\\MediumButtonBlank", s);
    put(g, 0x25c2c, "\\ui\\CharSelect\\ShortButtonBlank", s);
    put(g, 0x25c34, "\\ui\\FrontEnd\\textbox2", s);
    put(g, 0x25c38, "\\ui\\FrontEnd\\3WideButtonBlank", s);
    put(g, 0x25c50, "\\ui\\FrontEnd\\TermsofUseBckg", s);
    put(g, 0x25c54, "\\ui\\CharSelect\\charselectbckg", s);
    put(g, 0x25c58, "\\ui\\CharSelect\\DifficultyLevels", s);
    put(g, 0x25c5c, "\\ui\\FrontEnd\\CinematicsSelection", s);
    put(g, 0x25c64, "\\ui\\CharSelect\\charselectbox", s);
    put(g, 0x25c68, "\\ui\\CharSelect\\charselectboxgrey", s);
    put(g, 0x25c80, "\\ui\\CharSelect\\CreateNewCharImage", s);
    put(g, 0x25c84, "\\ui\\FrontEnd\\PopUpOK", s);
    put(g, 0x25c88, "\\ui\\FrontEnd\\PopUp_340x224", s);
    put(g, 0x25c8c, "\\ui\\FrontEnd\\PopUpWide", s);
    put(g, 0x25c90, "\\ui\\FrontEnd\\PopUpLargest", s);
    put(g, 0x25c94, "\\ui\\FrontEnd\\TileableDialog", s);
    put(g, 0x25c98, "\\ui\\FrontEnd\\PopUpOK2", s);
    put(g, 0x25c6c, "\\ui\\FrontEnd\\PopUpOKCancel", s);
    put(g, 0x25c70, "\\ui\\FrontEnd\\PopUpOKCancel2", s);
    put(g, 0x25c9c, "\\ui\\FrontEnd\\OkCancelButtonBlank", s);
    put(g, 0x25ca0, "\\ui\\FrontEnd\\CancelButtonBlank", s);
    put(g, 0x25c74, "\\ui\\CharSelect\\selectchararrows", s);
    put(g, 0x25c78, s->isExp() ? "\\ui\\CharSelect\\Realmselect" : "\\ui\\FrontEnd\\CurRealm", s);
    put(g, 0x25c7c, s->isExp() ? "\\ui\\CharSelect\\RealmSelectbuttonthin" : "\\ui\\FrontEnd\\NarrowButtonBlank02", s);
    put(g, 0x25ca4, "\\ui\\CharSelect\\password", s);
    put(g, 0x25cb4, "\\UI\\FrontEnd\\realmbckg", s);
    put(g, 0x25cb8, "\\UI\\FRONTEND\\joingamescrollbars", s);   // CRT _sprintf 0x101B
    put(g, 0x25cbc, s->isExp() ? "\\UI\\FrontEnd\\tcpipscreen" : "\\UI\\FrontEnd\\tcpipbckg", s);
    put(g, 0x25cc0, "\\UI\\FrontEnd\\IPAddressBox", s);
    put(g, 0x25cc4, "\\UI\\FrontEnd\\downloadpatchbckg", s);
    put(g, 0x25cc8, "\\UI\\FrontEnd\\downloadpatchball2", s);
    put(g, 0x25c3c, "\\UI\\FrontEnd\\EditTextBoxWide", s);
}
