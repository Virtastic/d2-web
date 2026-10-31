// D2Multi batch 15: ACB0 (control-set builder; 9550, 9D60, AA60 live in batch n37), B160 (date format).
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

// 1.13c D2MULTI+0x0ACB0
// 1.13c D2Multi+0xACB0
extern "C" void FUN_6f9dacb0(void* cb) { ctl3((void*)0x6f9ea360, (void*)0x6f9ea390, (void*)0x6f9ea3f0, cb, false); }

// 1.13c D2MULTI+0x0B160
// 1.13c D2Multi+0xB160
extern "C" void FUN_6f9db160(const void* ft, char* out, int cap) {
    (void)ft; (void)out; (void)cap;  // ponytail: FILETIME->localized long date (GetDateFormatA lcid 0x400) is host-locale; host shim supplies it
}
