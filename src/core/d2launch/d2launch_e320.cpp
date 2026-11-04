// 1.13c D2Launch+0xE320 (sub_E320), the Bnclient load. ESI = the state block (the flag dword at +0x221). The null-state exit
// (Ordinal_10265/10024 and _exit) is not reproduced: a capture of it would end the process. Module words DAT_6fa656c0 (the handle)
// and DAT_6fa656c4 (the interface) are passed by pointer; the imports and the vtable call are raw seams.
#include <cstdint>
#include <cstdio>

struct D2lE320Seams {
    uint32_t (*loadLibrary)(const char* name);
    uint32_t (*getProc)(uint32_t h, const char* name);
    uint32_t (*getLastError)();
    void (*messageBox)(const char* text, const char* caption, uint32_t type);
    uint32_t (*callQI)(uint32_t fp);     // the QueryInterface result: the interface pointer
    void (*vtCall)(uint32_t obj);        // (*obj)(): the vtable slot 0 call
};

extern "C" int32_t d2l_e320_core(uint8_t* st, uint32_t* hmSlot, uint32_t* ifcSlot, const D2lE320Seams* s) {
    if (*(const uint32_t*)(st + 0x221) != 0) return 1;
    const uint32_t h = s->loadLibrary("Bnclient.dll");
    *hmSlot = h;
    if (!h) {
        char b[100];
        snprintf(b, sizeof b, "Cannot load Bnclient.dll: Error %d", (int)s->getLastError());
        s->messageBox(b, "D2Multi", 0x10);
        return 0;
    }
    const uint32_t fp = s->getProc(h, "QueryInterface");
    if (!fp) {
        char b[100];
        snprintf(b, sizeof b, "Unable to QueryInterface for Bnclient.dll: Error %d", (int)s->getLastError());
        s->messageBox(b, "D2Multi", 0x10);
        return 0;
    }
    const uint32_t obj = s->callQI(fp);
    *ifcSlot = obj;
    *(uint32_t*)(st + 0x221) = obj;
    s->vtCall(obj);
    return 1;
}
