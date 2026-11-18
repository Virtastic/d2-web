// 1.13c Storm+0x24CD0 -- error-text setup (registers: EDI = error code, EBX = the string the text names; stack: callback, ret 4).
// The text is the string resource (0x5201 / 0x5202 / 0x5203 for the codes 0x85100065 / 0x8510006D / 0x6E) or, when the resource is empty, the
// default in the 3-entry table at Storm+0x45CE8; a "%s" in it is filled with the EBX string (wsprintfA), a non-empty text gets "\n" appended.
// The callback is handed a stack frame { code, ebx, &text } and the last-error global (Storm+0x45388) takes the code.
#include <cstdint>
#include <cstring>
#include <cstdio>

extern "C" { uint32_t g_storm_lastError; }
static const char* g_storm_defaultText[3] = {"Invalid argument: %s", "The syntax of the command is incorrect.", "Unable to open response file: %s"};
// Stand-in for LoadStringA(hInst, id, buf, 256); retail resources are absent on wasm: returns empty.
static void loadString(unsigned, char* b, int) { b[0] = 0; }

extern "C" void storm_sub_24cd0(uint32_t code, const void* ebx, void (*cb)(uint32_t* frame)) {
    unsigned id; int idx;
    if (code == 0x6e) { idx = 2; id = 0x5203; }
    else if (code == 0x85100065u) { idx = 0; id = 0x5201; }
    else if (code == 0x8510006du) { idx = 1; id = 0x5202; }
    else return;
    char a[256], b[256];
    a[0] = 0;
    loadString(id, a, 256);
    if (!a[0]) { strncpy(a, g_storm_defaultText[idx], 255); a[255] = 0; }
    if (strstr(a, "%s")) { snprintf(b, sizeof b, a, ebx ? (const char*)ebx : "(null)"); }
    else { strncpy(b, a, 255); b[255] = 0; }
    size_t n = strlen(b);
    if (n && n < 255) { b[n] = '\n'; b[n + 1] = 0; }
    g_storm_lastError = code;   // retail: Storm+0x45388 := code, SetLastError(code), then the callback
    uint32_t frame[3] = { code, (uint32_t)(uintptr_t)ebx, (uint32_t)(uintptr_t)b };
    cb(frame);
}
