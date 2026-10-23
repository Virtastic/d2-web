// 1.13c D2Win+0x138C0 (sub_138C0): read CF_TEXT from the clipboard into a 0x200 static buffer.
// OS clipboard + window handle; seam only, NAMED, no vector.
#include <cstdint>
#include <cstring>

struct D2Win138C0Env {
    char* buf;                         // DAT_6f9aa680 (0x200)
    bool (*available)();               // IsClipboardFormatAvailable(CF_TEXT)
    bool (*open)();                    // OpenClipboard(D2Win ordinal 10048 hwnd)
    const char* (*get_locked)();       // GetClipboardData(1)+GlobalLock, null on failure
    void (*unlock)();                  // GlobalUnlock
    void (*close)();                   // CloseClipboard
};

// 1.13c D2Win+0x138C0
const char* d2win_sub_138c0(const D2Win138C0Env& e) {
    if (!e.available() || !e.open()) return "";
    if (const char* s = e.get_locked()) { strncpy(e.buf, s, 0x200); e.unlock(); }
    e.close();
    return e.buf;
}
