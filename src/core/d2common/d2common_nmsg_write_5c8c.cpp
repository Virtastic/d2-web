#include <cstdint>
#include <cstring>

// 1.13c D2Common+0x05C8C _NMSG_WRITE(msg): find msg in the 19-entry {code,text} table (DAT 0x6fde97a8).
// No match: nothing. __error_mode==1 (or 0 with __app_type==1): write text to stdout. Else unless
// msg==0xFC: build "Runtime Error!\n\nProgram: <name, truncated to "..."+last 57 chars if >59>\n\n<text>"
// and show it in a message box titled "Microsoft Visual C++ Runtime Library" (flags 0x12010).
// Env seam: stdout write, module file name (false = fail -> "<program name unknown>"), message box.
struct D2CommonNmsgEntry { int32_t code; const char* text; };
struct D2CommonNmsgEnv {
    const D2CommonNmsgEntry* table;  // 19 entries
    int32_t errorMode;               // DAT 0x6fdef5f4
    int32_t appType;                 // DAT 0x6fdef5f8
    void (*writeStdout)(const char*, uint32_t);
    bool (*moduleFileName)(char* buf, uint32_t size);
    void (*messageBox)(const char* text, const char* caption, uint32_t flags);
};

void d2common_nmsg_write_5c8c(const D2CommonNmsgEnv& e, int32_t msg)
{
    uint32_t i = 0;
    do { if (msg == e.table[i].code) break; i++; } while (i < 0x13);
    if (msg != e.table[i < 0x13 ? i : 0x12].code) return;
    const char* text = e.table[i < 0x13 ? i : 0x12].text;
    if (e.errorMode == 1 || (e.errorMode == 0 && e.appType == 1)) {
        e.writeStdout(text, (uint32_t)strlen(text));
    } else if (msg != 0xfc) {
        char name[0x104 + 1] = {};
        if (!e.moduleFileName(name, 0x104)) strcpy(name, "<program name unknown>");
        const char* p = name;
        char tail[64];
        if (strlen(p) + 1 > 0x3c) {
            strcpy(tail, "...");
            strncpy(tail, "...", 3);
            size_t n = strlen(p);
            memcpy(tail + 3, p + n - 0x38, 0x38 + 1);  // CRT copies "..." over p+len-0x3c+1+... tail
            p = tail;
        }
        char out[0x104 + 0x100 + 64];
        strcpy(out, "Runtime Error!\n\nProgram: ");
        strcat(out, p);
        strcat(out, "\n\n");
        strcat(out, text);
        e.messageBox(out, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}
