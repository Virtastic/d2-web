// BNClient 0xD320 (stdcall, three arguments: out1, out2 dwords and the out3 string; ret 0xC): CheckRevision. Clears the
// three outputs, waits up to 20 s (GetTickCount, Sleep 10) for the revision file name DAT_6FF3F510, opens it (0x8096),
// rejects a size of 1..4 (0x80A8), extracts the check DLL (0xD160), loads it and calls its "CheckRevision" export with
// the game exe path, bnclient.dll and d2client.dll beside it and the formula DAT_6FF3F770. Closes, Sleep(50), deletes
// both files. Returns 1 on the timeout, 0 when the open fails, else the export's result (0 when any step fails).
#include <cstdint>
#include <cstring>

struct BnD320Seams {
    uint32_t (*tick)();                                                                // GetTickCount
    void (*sleep)(uint32_t ms);                                                        // Sleep
    uint32_t (*open)(uint32_t name, uint32_t a, uint32_t b, uint32_t* h);             // 0x8096 Storm 266
    uint32_t (*size)(uint32_t h, uint32_t* out);                                       // 0x80A8
    uint32_t (*extract)(uint32_t name, uint32_t h, char* out);                         // 0xD160 (EAX = name; stack h, out)
    uint32_t (*load)(const char* path);                                                // LoadLibraryA
    uint32_t (*proc)(uint32_t hm, uint32_t name);                                      // GetProcAddress
    uint32_t (*module)(uint32_t name);                                                 // GetModuleHandleA
    uint32_t (*filename)(uint32_t hm, char* out, uint32_t n);                          // GetModuleFileNameA
    void (*copy)(char* d, const char* s, uint32_t max);                                // 0x804E SStrCopy
    char* (*rchr)(const char* s, int c);                                               // 0x1FB0 strrchr
    uint32_t (*check)(uint32_t fn, const char* exe, const char* bn, const char* d2c, uint32_t formula, uint32_t* o1,
                      uint32_t* o2, uint8_t* o3);                                      // the export, stdcall 7
    void (*free_lib)(uint32_t hm);                                                     // FreeLibrary
    void (*close)(uint32_t h);                                                         // 0x80C0
    void (*del)(uint32_t path);                                                        // DeleteFileA
    const char* path_read;                 // DAT_6FF3F510 contents
    const char* bn_name; const char* d2c_name;  // "bnclient.dll" (0x6FF3AF70), "d2client.dll" (0x6FF3AF60)
    uint32_t path, formula, s_check;       // DAT_6FF3F510, DAT_6FF3F770, "CheckRevision" (0x6FF3AF80)
};

// 1.13c BNClient+0x0D320
extern "C" uint32_t bnclient_check_revision_d320(uint32_t* o1, uint32_t* o2, uint8_t* o3, const BnD320Seams* s) {
    if (o1) *o1 = 0;
    if (o2) *o2 = 0;
    if (o3) *o3 = 0;
    char dll[0x104];
    memset(dll, 0, sizeof dll);
    uint32_t h = 0, result = 0, count = 0;
    const uint32_t t0 = s->tick();
    while (s->path_read[0] == 0) {
        s->sleep(10);
        if (s->tick() - t0 > 0x4E20) return 1;
    }
    if (s->open(s->path, 0, 0, &h) == 0) return 0;
    s->size(h, &count);
    if ((count == 0 || count >= 5) && s->extract(s->path, h, dll) != 0) {
        const uint32_t hm = s->load(dll);
        if (hm != 0) {
            const uint32_t fn = s->proc(hm, s->s_check);
            if (fn != 0) {
                char exe[0x104], bn[0x104], d2c[0x104];
                memset(exe, 0, sizeof exe);
                memset(bn, 0, sizeof bn);
                memset(d2c, 0, sizeof d2c);
                s->filename(s->module(0), exe, 0x104);
                s->copy(bn, exe, 0x7FFFFFFF);
                s->copy(s->rchr(bn, '\\') + 1, s->bn_name, 0x7FFFFFFF);
                s->copy(d2c, exe, 0x7FFFFFFF);
                s->copy(s->rchr(d2c, '\\') + 1, s->d2c_name, 0x7FFFFFFF);
                result = s->check(fn, exe, bn, d2c, s->formula, o1, o2, o3);
            }
            s->free_lib(hm);
        }
    }
    if (h != 0) s->close(h);
    s->sleep(0x32);
    s->del(s->path);
    if (dll[0] != 0) s->del((uint32_t)(uintptr_t)dll);
    return result;
}
