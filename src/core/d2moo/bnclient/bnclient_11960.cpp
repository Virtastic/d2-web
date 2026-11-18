// 1.13c BNClient+0x11960 (cdecl, no args): compose and send frame 0x50 (the auth-info request).
// Body (0x4A zeroed dwords):
//   +0x00 0 (protocol)          +0x04 'IX86'                    +0x08 'D2DV', or 'D2XP' (+0x13FA) when 0x8126 answers non-zero
//   +0x0C 0xD (version)         +0x10 filled by 0x8156(+0x10, 3) +0x14 the local address: sin_addr of the 0x10-byte sockaddr 0x810E
//                                                                  fills for socket DAT_1F654 (0 when there is no socket or it fails)
//   +0x18 (UTC FILETIME - local FILETIME) / 600000000, signed (__alldiv), the low dword
//   +0x1C GetUserDefaultLCID    +0x20 GetSystemDefaultUILanguage (kernel32, looked up by name) & 0xFFFF, else GetSystemDefaultLangID
//   +0x24 the first three bytes of GetLocaleInfoA(0x400, 7) and a NUL   +0x28 GetLocaleInfoA(0x400, 0x1002) (0x100 bytes)
// EB00 sends SStrLen(+0x28) + 0x29 bytes, id 0x50. Returns 1.
#include <cstdint>
#include <cstring>

struct Bn11960Env {
    uint32_t (*expansion)();                                            // 0x8126
    void (*lang)(uint8_t* p, uint32_t n);                               // 0x8156, stdcall 2
    uint32_t (*local_addr)(uint32_t sock, uint8_t* addr, uint32_t* len); // 0x810E (ECX socket, EDX addr, stack &len)
    void (*system_time)(uint8_t* st);                                   // kernel32 GetSystemTime
    void (*local_time)(uint8_t* st);                                    // kernel32 GetLocalTime
    uint32_t (*to_filetime)(const uint8_t* st, uint32_t* ft);           // kernel32 SystemTimeToFileTime
    uint32_t (*user_lcid)();                                            // kernel32 GetUserDefaultLCID
    uint32_t (*load_library)(const char* name);                         // kernel32 LoadLibraryA
    uint32_t (*get_proc)(uint32_t h, const char* name);                 // kernel32 GetProcAddress
    uint32_t (*call_proc)(uint32_t p);                                  // the looked-up GetSystemDefaultUILanguage
    void (*free_library)(uint32_t h);                                   // kernel32 FreeLibrary
    uint32_t (*lang_id)();                                              // kernel32 GetSystemDefaultLangID
    void (*locale_info)(uint32_t lcid, uint32_t type, char* buf, uint32_t n);   // kernel32 GetLocaleInfoA
    uint32_t (*slen)(const char* s);                                    // 0x8042 SStrLen
    void (*send)(uint32_t len, uint32_t id, const uint8_t* p);          // 0xEB00 (ECX len, EDI payload, id on the stack)
};

// 1.13c BNClient+0x11960
extern "C" uint32_t d2_bnclient_11960(const uint32_t* sock, const Bn11960Env* e) {
    alignas(4) uint8_t b[0x128];
    memset(b, 0, sizeof b);
    uint32_t* w = (uint32_t*)b;
    w[1] = 0x49583836u;
    w[2] = (e->expansion() ? 0x13FAu : 0u) + 0x44324456u;
    w[3] = 0xD;
    e->lang(b + 0x10, 3);
    uint32_t len = 0x10;
    alignas(4) uint8_t addr[0x10];
    if (*sock && e->local_addr(*sock, addr, &len)) memcpy(&w[5], addr + 4, 4);
    else w[5] = 0;
    alignas(4) uint8_t st_sys[16], st_loc[16];
    uint32_t ft_sys[2], ft_loc[2];
    e->system_time(st_sys);
    e->local_time(st_loc);
    e->to_filetime(st_sys, ft_sys);
    e->to_filetime(st_loc, ft_loc);
    const uint64_t diff = (((uint64_t)ft_sys[1] << 32) | ft_sys[0]) - (((uint64_t)ft_loc[1] << 32) | ft_loc[0]);
    w[6] = (uint32_t)(int64_t)((int64_t)diff / 600000000);
    w[7] = e->user_lcid();
    uint32_t langv;
    const uint32_t h = e->load_library("kernel32.dll");
    uint32_t p = 0;
    if (h) {
        p = e->get_proc(h, "GetSystemDefaultUILanguage");
        if (p) { langv = e->call_proc(p) & 0xFFFF; e->free_library(h); }
        else e->free_library(h);
    }
    if (!p) langv = e->lang_id() & 0xFFFF;
    w[8] = langv;
    alignas(4) char abbr[0x40];
    e->locale_info(0x400, 7, abbr, 0x40);
    abbr[3] = 0;
    memcpy(&w[9], abbr, 4);
    e->locale_info(0x400, 0x1002, (char*)b + 0x28, 0x100);
    e->send(e->slen((const char*)b + 0x28) + 0x29, 0x50, b);
    return 1;
}
