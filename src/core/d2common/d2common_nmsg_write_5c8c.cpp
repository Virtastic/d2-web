#include <cstring>
#include "d2common_crt.h"

// 1.13c D2Common+0x05C8C _NMSG_WRITE(msg): find msg in the 19-entry {code,text} table (DAT 0x6fde97a8).
// No match: nothing. __error_mode==1 (or 0 with __app_type==1): write text to stdout. Else unless
// msg==0xFC: build "Runtime Error!\n\nProgram: <name, truncated to "..."+last 57 chars if >59>\n\n<text>"
// and show it in a message box titled "Microsoft Visual C++ Runtime Library" (flags 0x12010).
// Win32 boundary: g_d2common_win32 WriteStdout, GetModuleFileName, MessageBox.
struct D2CommonNmsgEntry { int32_t code; const char* text; };
static const D2CommonNmsgEntry kNmsg[0x13] = {
    {0x02, "R6002\r\n- floating point not loaded\r\n"},
    {0x08, "R6008\r\n- not enough space for arguments\r\n"},
    {0x09, "R6009\r\n- not enough space for environment\r\n"},
    {0x0a, "\r\nThis application has requested the Runtime to terminate it in an unusual way.\nPlease contact the application's support team for more information.\r\n"},
    {0x10, "R6016\r\n- not enough space for thread data\r\n"},
    {0x11, "R6017\r\n- unexpected multithread lock error\r\n"},
    {0x12, "R6018\r\n- unexpected heap error\r\n"},
    {0x13, "R6019\r\n- unable to open console device\r\n"},
    {0x18, "R6024\r\n- not enough space for _onexit/atexit table\r\n"},
    {0x19, "R6025\r\n- pure virtual function call\r\n"},
    {0x1a, "R6026\r\n- not enough space for stdio initialization\r\n"},
    {0x1b, "R6027\r\n- not enough space for lowio initialization\r\n"},
    {0x1c, "R6028\r\n- unable to initialize heap\r\n"},
    {0x1d, "R6029\r\n- This application cannot run using the active version of the Microsoft .NET Runtime\nPlease contact the application's support team for more information.\r\n"},
    {0x78, "DOMAIN error\r\n"},
    {0x79, "SING error\r\n"},
    {0x7a, "TLOSS error\r\n"},
    {0xfc, "\r\n"},
    {0xff, "runtime error "},
};

extern "C" void d2common_nmsg_write_5c8c(int32_t msg)
{
    uint32_t i = 0;
    do { if (msg == kNmsg[i].code) break; i++; } while (i < 0x13);
    if (msg != kNmsg[i < 0x13 ? i : 0x12].code) return;
    const char* text = kNmsg[i < 0x13 ? i : 0x12].text;
    const D2CommonWin32& w = g_d2common_win32;
    if (g_d2common_crt.errorMode == 1 || (g_d2common_crt.errorMode == 0 && g_d2common_crt.appType == 1)) {
        w.WriteStdout(text, (uint32_t)strlen(text));
    } else if (msg != 0xfc) {
        char name[0x104 + 1] = {};
        if (!w.GetModuleFileName(name, 0x104)) strcpy(name, "<program name unknown>");
        const char* p = name;
        char tail[64];
        if (strlen(p) + 1 > 0x3c) {
            size_t n = strlen(p);
            strcpy(tail, "...");
            memcpy(tail + 3, p + n - 0x38, 0x38 + 1);
            p = tail;
        }
        char out[0x104 + 0x100 + 64];
        strcpy(out, "Runtime Error!\n\nProgram: ");
        strcat(out, p);
        strcat(out, "\n\n");
        strcat(out, text);
        w.MessageBox(out, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}

struct D2CommonNmsgEnv {
    const D2CommonNmsgEntry* table;
    int32_t errorMode;
    int32_t appType;
    void (*writeStdout)(const char*, uint32_t);
    bool (*moduleFileName)(char* buf, uint32_t size);
    void (*messageBox)(const char* text, const char* caption, uint32_t flags);
};
// Old seam form: installs the caller's modes and Win32 hooks, then runs the real function.
void d2common_nmsg_write_5c8c_seam(const D2CommonNmsgEnv& e, int32_t msg)
{
    g_d2common_crt.errorMode = e.errorMode;
    g_d2common_crt.appType = e.appType;
    g_d2common_win32.WriteStdout = e.writeStdout;
    g_d2common_win32.GetModuleFileName = e.moduleFileName;
    g_d2common_win32.MessageBox = e.messageBox;
    d2common_nmsg_write_5c8c(msg);
}
