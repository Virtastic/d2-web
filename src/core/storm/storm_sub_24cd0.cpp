// 1.13c Storm+0x24CD0 -- error-text setup. EDI (implicit) = error code.
#include <cstdint>
#include <cstring>
#include <cstdio>

// Storm globals: 0x45388 last error, 0x3712c message buffer, 0x35ce8 default-text table[3].
uint32_t g_storm_lastError;
char g_storm_errText[256];
const char* g_storm_defaultText[3] = {"", "", ""};
// Stand-in for LoadStringA(hInst, id, buf, 256); retail resources are absent on wasm: returns empty.
static void loadString(unsigned, char* b, int) { b[0] = 0; }

void storm_sub_24cd0(uint32_t code, void (*cb)(void*), void* frame) {
    unsigned id; int idx;
    if (code == 0x6e) { idx = 2; id = 0x5203; }
    else if (code == 0x85100065u) { idx = 0; id = 0x5201; }
    else if (code == 0x8510006du) { idx = 1; id = 0x5202; }
    else return;
    char a[256], b[256];
    a[0] = 0;
    loadString(id, a, 256);
    if (!a[0]) { strncpy(a, g_storm_defaultText[idx], 255); a[255] = 0; }
    if (strstr(a, "%s")) { strncpy(b, a, 255); b[255] = 0; } // retail wsprintfA(b,a) with no args
    else { strncpy(b, a, 255); b[255] = 0; }
    if (b[0]) { strncpy(g_storm_errText, b, 255); g_storm_errText[255] = 0; }
    g_storm_lastError = code;
    cb(frame); // retail: SetLastError(code) then tail-calls param_1
}
