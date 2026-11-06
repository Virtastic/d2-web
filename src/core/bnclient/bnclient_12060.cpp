// 1.13c BNClient+0x12060 (cdecl, no args): compose and send frame 0x51 (the auth-check request).
// Frame body (the zeroed 0x97-dword local, then the 0x100-byte version-info string D320 fills):
//   +0x00 seed: DAT_1F930 += GetTickCount() + time(NULL)        +0x04/+0x08 the two D320 out dwords
//   +0x0C key count (1, or 2 when 0x8126 answers non-zero)       +0x10 0
//   +0x14 key block 0 (0x103B0 over DAT_1E8D8), 0x24 bytes       +0x38 key block 1 (0x103B0 over DAT_1E8E0), two-key form only
//   then the info string (SStrCopy, max 0x200) and, after its NUL, the owner string DAT_1E8E4 (SStrCopy, max = end of the zeroed local
//   minus the write position: 0x25C - pos for two keys, 0x238 - pos for one). The key handles DAT_1E8D8 and DAT_1E8E4 are released
//   (0x8048) and cleared; EB00 sends (SStrLen(owner) + pos + 1) bytes. Before the build: spin on the ready byte DAT_1F510 (Sleep 10 per
//   turn), returning 0 as soon as DAT_1F644 is set; a D320 failure sets DAT_1F1F4 = 1 (only if it was 0) under the 0x1F1DC lock and returns 0.
#include <cstdint>
#include <cstring>

struct Bn12060Globals {
    uint32_t* seed;                  // DAT_1F930
    const volatile uint8_t* ready;   // DAT_1F510
    const volatile uint32_t* abort;  // DAT_1F644
    uint32_t* fail;                  // DAT_1F1F4
    uint32_t* keys;                  // DAT_1E8D8 [0] key 0, [2] key 1, [3] the owner string
    const uint32_t* stamp;           // DAT_1F874 (EAX of 0x103B0)
};
struct Bn12060Env {
    uint32_t (*time)();                                              // 0x1621 time(NULL), cdecl
    uint32_t (*tick)();                                              // kernel32 GetTickCount
    void (*sleep)(uint32_t ms);                                      // kernel32 Sleep
    uint32_t (*lookup)(uint32_t* a, uint32_t* b, char* info);         // 0xD320 stdcall 3
    void (*enter)();                                                 // EnterCriticalSection(0x1F1DC)
    void (*leave)();
    void (*load)();                                                  // 0x162B0, cdecl
    uint32_t (*expansion)();                                         // 0x8126
    uint32_t (*scopy)(char* dst, const char* src, uint32_t max);     // 0x804E SStrCopy
    void (*key_block)(uint8_t* self, uint32_t eax, uint32_t edi, uint32_t tag);   // 0x103B0 (ECX self, EAX, EDI key, stack tag)
    void (*release)(uint32_t h);                                     // 0x8048
    uint32_t (*slen)(const char* s);                                 // 0x8042 SStrLen
    void (*send)(uint32_t len, uint32_t id, const uint8_t* p);       // 0xEB00 (ECX len, EDI payload, id on the stack)
};

// 1.13c BNClient+0x12060
extern "C" uint32_t d2_bnclient_12060(const Bn12060Globals* g, const Bn12060Env* e) {
    alignas(4) uint8_t f[0x360];
    memset(f, 0, 0x97 * 4);
    uint32_t* w = (uint32_t*)f;
    const uint32_t now = e->time();
    *g->seed = e->tick() + *g->seed + now;
    w[0] = *g->seed;
    while (!*g->ready) {
        if (*g->abort) return 0;
        e->sleep(10);
    }
    char* info = (char*)f + 0x260;
    if (!e->lookup(w + 1, w + 2, info)) {
        e->enter();
        if (*g->fail == 0) *g->fail = 1;
        e->leave();
        return 0;
    }
    w[4] = 0;
    e->load();
    char* pos; uint32_t max;
    if (e->expansion()) {
        pos = (char*)f + 0x5C + e->scopy((char*)f + 0x5C, info, 0x200) + 1;
        w[3] = 2;
        e->key_block(f + 0x14, *g->stamp, g->keys[0], *g->seed);
        e->key_block(f + 0x38, *g->stamp, g->keys[2], *g->seed);
        max = (uint32_t)(((char*)f + 0x25C) - pos);
    } else {
        pos = (char*)f + 0x38 + e->scopy((char*)f + 0x38, info, 0x200) + 1;
        w[3] = 1;
        e->key_block(f + 0x14, *g->stamp, g->keys[0], *g->seed);
        max = (uint32_t)(((char*)f + 0x238) - pos);
    }
    e->scopy(pos, (const char*)(uintptr_t)g->keys[3], max);
    if (g->keys[0]) { e->release(g->keys[0]); g->keys[0] = 0; }
    if (g->keys[3]) { e->release(g->keys[3]); g->keys[3] = 0; }
    const uint32_t n = e->slen(pos);
    e->send(n + (uint32_t)(pos - (char*)f) + 1, 0x51, f);
    return 1;
}
