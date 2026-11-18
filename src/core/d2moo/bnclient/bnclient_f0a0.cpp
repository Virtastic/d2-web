// BNClient 0xF0A0 (stdcall, one argument {out buffer, host}; ret 4): the server-list thread. A digit-leading host goes
// through inet_addr into a one-entry hostent kept in DAT_6FF3F910..F92B, any other through gethostbyname; a failure logs
// the WinSock error and returns 0. It seeds the CRT rand with GetTickCount, shuffles the address dwords (j = i +
// rand() * remaining / 32768), then writes every address as inet_ntoa text plus ";" into the buffer, the last ";" cut.
#include <cstdint>

struct BnF0A0Seams {
    int32_t (*isdigit_)(int32_t c);                                  // 0x1E1F
    uint32_t (*inet_addr_)(const char* host);                        // 0x7FD6 ws2_32 10
    uint8_t* (*gethostbyname_)(const char* host);                    // 0x7FC4 ws2_32 52
    int32_t (*wsa_error)();                                          // 0x7FCA ws2_32 111
    void (*log)(const char* fmt, int32_t err);                       // 0x17420
    uint32_t (*tick)();                                              // GetTickCount
    void (*srand_)(uint32_t seed);                                   // 0x15F2
    int32_t (*rand_)();                                              // 0x15FF
    void (*fatal)(uint32_t code);                                    // 0x80E4 Fog 10265, 0x80DE Fog 10024, 0x1569 _exit(-1)
    const char* (*ntoa)(uint32_t addr);                              // 0x7FDC ws2_32 11
    int32_t (*strcopy)(char* dst, const char* src, uint32_t max);    // 0x804E Storm 501
    uint8_t* fake;                                                   // DAT_6FF3F910 (28 bytes: addr, list, hostent)
    const char* err_addr;                                            // 0x6FF3ADB4
    const char* err_host;                                            // 0x6FF3AD70
    const char* sep;                                                 // 0x6FF3AD6C ";"
};

static uint32_t f0a0_rd(const uint8_t* p) { return *(const uint32_t*)p; }
static void f0a0_wr(uint8_t* p, uint32_t v) { *(uint32_t*)p = v; }

// 1.13c BNClient+0x0F0A0
extern "C" uint32_t bnclient_server_list_f0a0(const uint32_t* arg, const BnF0A0Seams* s) {
    char* const buf = (char*)(uintptr_t)arg[0];
    const char* host = (const char*)(uintptr_t)arg[1];
    uint8_t* he;
    if (s->isdigit_((int32_t)(signed char)host[0]) != 0) {
        const uint32_t a = s->inet_addr_(host);
        if (a == 0xFFFFFFFFu) { s->log(s->err_addr, s->wsa_error()); return 0; }
        uint8_t* f = s->fake;
        f0a0_wr(f + 0x18, (uint32_t)(uintptr_t)(f + 4));   // F928: h_addr_list = &F914
        f0a0_wr(f + 4, (uint32_t)(uintptr_t)f);            // F914: list[0] = &F910
        f0a0_wr(f + 8, 0);                                  // F918: list end
        he = f + 0xC;                                       // F91C: the hostent
        f0a0_wr(f, a);                                      // F910: the address
    } else {
        he = s->gethostbyname_(host);
        if (he == nullptr) { s->log(s->err_host, s->wsa_error()); return 0; }
    }
    uint32_t* list = (uint32_t*)(uintptr_t)f0a0_rd(he + 0xC);
    int32_t n = 0;
    if (list[0] != 0) while (list[++n] != 0) {}
    s->srand_(s->tick());
    const int32_t last = n - 1;
    int32_t rem = last;
    for (int32_t i = 0; i < last; ++i, --rem) {
        const int32_t m = s->rand_() * rem;
        const int32_t j = ((m + (int32_t)((uint32_t)(m >> 31) & 0x7FFF)) >> 15) + i;
        if (i > j) { s->fatal(0x1AD); return 0; }
        uint32_t* pi = (uint32_t*)(uintptr_t)list[i];
        uint32_t* pj = (uint32_t*)(uintptr_t)list[j];
        const uint32_t t = *pi;
        *pi = *pj;
        *pj = t;
    }
    list = (uint32_t*)(uintptr_t)f0a0_rd(he + 0xC);
    char* out = buf;
    for (; *list != 0; ++list) {
        out += s->strcopy(out, s->ntoa(*(const uint32_t*)(uintptr_t)*list), 0x7FFFFFFF);
        out += s->strcopy(out, s->sep, 0x7FFFFFFF);
    }
    if (out > buf) out[-1] = 0;
    return 1;
}
