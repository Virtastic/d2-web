#include <cstdint>

// 1.13c D2Common+0x08B1B ___crtMessageBoxA(text, caption, type): MSVC CRT. Lazily binds user32
// entry points (MessageBoxA, GetActiveWindow, GetLastActivePopup, and when osplatform==2 also
// GetUserObjectInformationA + GetProcessWindowStation) into the cache below, then picks an owner
// window: on a non-interactive window station (flags&1==0) sets MB_SERVICE_NOTIFICATION
// (0x40000 if winver<4, else 0x200000), otherwise owner = GetLastActivePopup(GetActiveWindow()).
// Env seam: Win32 imports and the CRT globals are owner-supplied; cache lives in the struct.
typedef int (*D2CrtFn)();
struct D2CrtMsgBoxEnv {
    // cache (DAT_6fdef96c/970/974/978/97c); zero-initialised
    int (*messageBoxA)(int, const char*, const char*, uint32_t);
    int (*getActiveWindow)();
    int (*getLastActivePopup)(int);
    int (*getProcessWindowStation)();
    int (*getUserObjectInformationA)(int, int, void*, uint32_t, void*);
    uint32_t osplatform;       // DAT_6fdef5a4
    uint32_t winver;           // DAT_6fdef5b0
    // imports: LoadLibraryA("user32.dll") -> nonzero handle; GetProcAddress(h, name)
    void* (*loadUser32)();
    void* (*getProc)(void* h, const char* name);
};

int d2common_crtmessageboxa_8b1b(D2CrtMsgBoxEnv& e, const char* text, const char* caption, uint32_t type)
{
    int owner = 0;
    if (!e.messageBoxA) {
        void* h = e.loadUser32();
        if (!h) return 0;
        e.messageBoxA = (decltype(e.messageBoxA))e.getProc(h, "MessageBoxA");
        if (!e.messageBoxA) return 0;
        e.getActiveWindow = (decltype(e.getActiveWindow))e.getProc(h, "GetActiveWindow");
        e.getLastActivePopup = (decltype(e.getLastActivePopup))e.getProc(h, "GetLastActivePopup");
        if (e.osplatform == 2) {
            e.getUserObjectInformationA = (decltype(e.getUserObjectInformationA))e.getProc(h, "GetUserObjectInformationA");
            if (e.getUserObjectInformationA)
                e.getProcessWindowStation = (decltype(e.getProcessWindowStation))e.getProc(h, "GetProcessWindowStation");
        }
    }
    uint8_t info[12]; // USEROBJECTFLAGS: flags at +8 (local_c)
    int ws; uint32_t need;
    bool interactive = !e.getProcessWindowStation ||
        ((ws = e.getProcessWindowStation()) != 0 &&
         e.getUserObjectInformationA(ws, 1, info, 0xc, &need) != 0 &&
         (info[8] & 1) != 0);
    if (interactive) {
        if (e.getActiveWindow && (owner = e.getActiveWindow()) != 0 && e.getLastActivePopup)
            owner = e.getLastActivePopup(owner);
    } else
        type |= (e.winver < 4) ? 0x40000 : 0x200000;
    return e.messageBoxA(owner, text, caption, type);
}
