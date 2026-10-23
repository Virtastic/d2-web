// 1.13c D2Win+0x13800 (sub_13800): copy a NUL-terminated string to the clipboard (CF_TEXT).
#include <cstdint>
#include <cstring>

struct D2Win13800Env {
    void*  (*getOwner)();                       // Fog #10048
    bool   (*open)(void* hwnd);                 // OpenClipboard
    void   (*empty)();
    void*  (*alloc)(uint32_t n);                // GlobalAlloc(GMEM_DDESHARE,n)
    char*  (*lock)(void* h);
    void   (*unlock)(void* h);
    void   (*setData)(void* h);                 // SetClipboardData(CF_TEXT)
    void   (*close)();
    void   (*free)(void* h);
};

// 1.13c D2Win+0x13800. text = EAX; hMem = DAT_6f9aa678 (previous handle, freed first).
uint32_t d2win_sub_13800(const char* text, void*& hMem, const D2Win13800Env& e) {
    if (hMem) { e.free(hMem); hMem = nullptr; }
    if (!e.open(e.getOwner())) return 0;
    e.empty();
    uint32_t n = (uint32_t)strlen(text);
    hMem = e.alloc(n + 1);
    if (!hMem) { e.close(); return 0; }
    char* p = e.lock(hMem);
    memcpy(p, text, n);
    p[n] = 0;
    e.unlock(hMem);
    e.setData(hMem);
    e.close();
    return 1;
}
