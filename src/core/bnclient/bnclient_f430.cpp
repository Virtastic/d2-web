// BNClient 0xF430 (fastcall, ECX = the host list): connect to Battle.net. It clears the patch-list state, then (with a host
// list) opens one connection per host token, one every 5000 ms and at most 0x20, and selects on them until one is writable
// (it becomes the connection DAT_6FF3F654; a socket in the except set is closed), every other connection is closed after.
// Then phase 3: the peer address is logged, the protocol byte 1 is sent, frame 0x50 goes out and its reply is awaited;
// phase 4: frame 0x51 and its reply, then the ini-name request when DAT_6FF3F01C is set. Returns 1 on success or when the
// list is NULL, 0 on failure.
#include <cstdint>

struct BnF430Seams {
    void (*enter)(void* cs);                                             // EnterCriticalSection
    void (*leave)(void* cs);                                             // LeaveCriticalSection
    void (*sleep)(uint32_t ms);                                          // Sleep
    uint32_t (*tick)();                                                  // GetTickCount
    void (*log)(const char* fmt, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);   // 0x17420
    char* (*strtok_)(char* s, const char* sep);                          // 0x1D60
    uint32_t (*connect)(char* host, uint32_t port, uint32_t proc, uint32_t zero, const char* name, uint32_t one);   // 0x811A Fog 10004
    void (*close)(uint32_t conn);                                        // 0x8114 Fog 10005
    int32_t (*select_)(int32_t nfds, uint32_t* rd, uint32_t* wr, uint32_t* ex, const uint32_t* tv);   // 0x7FD0 ws2_32 18
    int32_t (*wsa_error)();                                              // 0x7FCA ws2_32 111
    int32_t (*peer)(uint32_t conn, uint8_t* sa, int32_t* len);           // 0x812C Fog 10012
    void (*reset)(uint32_t conn, uint32_t zero);                         // 0x8102 Fog 10017
    int32_t (*send)(uint32_t conn, uint8_t* buf, uint32_t n);            // 0x80FC Fog 10007
    void (*auth_info)();                                                 // 0x11960 frame 0x50
    int32_t (*wait_slot)(uint32_t id);                                   // 0x10D90
    int32_t (*auth_check)();                                             // 0x12060 frame 0x51
    void (*ini_name)();                                                  // 0xF050
    void* cs_patch;                                                      // DAT_6FF3F8E8
    void* cs_queue;                                                      // DAT_6FF3F1DC
    uint8_t* f670; uint8_t* f671; int32_t* f508;
    int32_t* f65c; int32_t* f1f8; int32_t* f644; int32_t* f660; int32_t* f1f4;
    int32_t* ee98; int32_t* f018; int32_t* ee9c; int32_t* f01c;
    uint32_t* f654;                                                      // the connection
    uint32_t proc;                                                       // 0x6FF2EC50, the reader thread
    const char* sep;                                                     // 0x6FF3AE80 " ,;"
    const char* name;                                                    // 0x6FF3AE70 "Client->BNet"
    const char* m_search;                                                // 0x6FF3AE84
    const char* m_none;                                                  // 0x6FF3AE5C
    const char* m_server;                                                // 0x6FF3AE3C
    const char* m_accounts;                                              // 0x6FF3AE28
    const char* m_version;                                               // 0x6FF3AE08
    const char* m_connected;                                             // 0x6FF3ADF0
};

static uint32_t f430_rd(const uint8_t* p) { return *(const uint32_t*)p; }
static void f430_wr(uint8_t* p, uint32_t v) { *(uint32_t*)p = v; }
static uint8_t* f430_ptr(uint32_t v) { return (uint8_t*)(uintptr_t)v; }

static void f430_clear_patch(const BnF430Seams* s) {
    s->enter(s->cs_patch);
    *s->f670 = 0;
    *s->f671 = 0;
    *s->f508 = 0x66;
    s->leave(s->cs_patch);
}

static int32_t f430_queue_error(const BnF430Seams* s) {
    s->enter(s->cs_queue);
    const int32_t q = *s->f1f4;
    s->leave(s->cs_queue);
    return q;
}

// 1.13c BNClient+0x0F430
extern "C" uint32_t bnclient_connect_f430(char* hosts, const BnF430Seams* s) {
    uint32_t ent[0x840] = {};                 // 0x20 entries of 0x108 bytes: the host string, +0x100 the connection
    uint8_t* const E = (uint8_t*)ent;
    f430_clear_patch(s);
    if (!hosts) return 1;
    *s->f65c = 0;
    *s->f1f8 = 2;
    s->sleep(10);
    s->log(s->m_search, 0, 0, 0, 0);
    char* tok = s->strtok_(hosts, s->sep);
    int32_t count = 0;
    uint32_t last = s->tick() - 5000;
    int32_t timeouts = 0;
    const uint32_t tv[2] = { 1, 0 };
    uint32_t wr[65], ex[65];
    if (*s->f654 == 0) {
        uint8_t* next = E;
        while (*s->f660 == 0) {
            if (s->tick() - last >= 5000) {
                last = s->tick();
                if (tok && *tok && count < 0x20) {
                    const char* q = tok;
                    uint8_t* d = next;
                    do { *d++ = (uint8_t)*q; } while (*q++);
                    f430_wr(next + 0x100, s->connect((char*)next, 0x17E0, s->proc, 0, s->name, 1));
                    ++count;
                    next += 0x108;
                    tok = s->strtok_(nullptr, s->sep);
                }
            }
            wr[0] = 0;
            ex[0] = 0;
            int32_t live = 0;
            for (int32_t i = 0; i < count; ++i) {
                const uint32_t c = f430_rd(E + 0x108 * i + 0x100);
                if (!c) continue;
                const uint32_t sock = f430_rd(f430_ptr(c) + 8);
                if (wr[0] < 0x40) wr[1 + wr[0]++] = sock;
                if (ex[0] < 0x40) ex[1 + ex[0]++] = sock;
                ++live;
            }
            if (live == 0) {
                if (!tok || !*tok) break;
                s->sleep(10);
            } else {
                const int32_t r = s->select_(0, nullptr, wr, ex, tv);
                if (r == 0) {
                    if (timeouts++ > 0x3c) break;
                } else if (r == -1) {
                    s->wsa_error();
                } else {
                    while (ex[0]) {
                        const uint32_t sock = ex[1 + --ex[0]];
                        for (int32_t i = 0; i < count; ++i) {
                            uint8_t* const slot = E + 0x108 * i + 0x100;
                            const uint32_t c = f430_rd(slot);
                            if (c && f430_rd(f430_ptr(c) + 8) == sock) { s->close(c); f430_wr(slot, 0); break; }
                        }
                    }
                    if (wr[0]) {
                        uint32_t win = *s->f654;
                        while (wr[0]) {
                            const uint32_t sock = wr[1 + --wr[0]];
                            for (int32_t i = 0; i < count; ++i) {
                                const uint32_t c = f430_rd(E + 0x108 * i + 0x100);
                                if (c && f430_rd(f430_ptr(c) + 8) == sock) {
                                    win = c;
                                    f430_wr(f430_ptr(c), 1);
                                    f430_wr(f430_ptr(c) + 4, 0);
                                    break;
                                }
                            }
                        }
                        *s->f654 = win;
                    }
                }
            }
            if (*s->f654) break;
        }
    }
    uint32_t cur = *s->f654;
    for (int32_t i = 0; i < count; ++i) {
        uint8_t* const slot = E + 0x108 * i + 0x100;
        const uint32_t c = f430_rd(slot);
        if (c && c != cur) { s->close(c); cur = *s->f654; f430_wr(slot, 0); }
    }
    if (!cur) { s->log(s->m_none, 0, 0, 0, 0); return 0; }

    *s->f65c = 1;
    int32_t q = f430_queue_error(s);
    if (*s->f644 || q || !*s->f654) return 0;
    uint8_t sa[16];
    int32_t len = 0x10;
    if (s->peer(*s->f654, sa, &len) != 0) s->log(s->m_server, sa[4], sa[5], sa[6], sa[7]);
    *s->f1f8 = 3;
    s->sleep(10);
    s->log(s->m_accounts, 0, 0, 0, 0);
    s->reset(*s->f654, 0);
    uint8_t one = 1;
    if (s->send(*s->f654, &one, 1) <= 0) return 0;
    *s->ee98 = 0;
    *s->f018 = 0;
    s->auth_info();
    if (s->wait_slot(0x50) == 0) {
        q = f430_queue_error(s);
        const int32_t abort_ = *s->f644;
        const uint32_t c = *s->f654;
        if (abort_ || q || !c) { s->close(c); *s->f654 = 0; return 0; }
    } else if (*s->f644 != 0) {
        s->enter(s->cs_queue);
        *s->f1f4 = 0xB;
        s->leave(s->cs_queue);
        return 1;
    }
    *s->f1f8 = 4;
    s->sleep(10);
    s->log(s->m_version, 0, 0, 0, 0);
    f430_clear_patch(s);
    *s->ee9c = 0;
    *s->f01c = 0;
    if (!s->auth_check()) return 0;
    if (!s->wait_slot(0x51)) return 0;
    if (f430_queue_error(s)) return 1;
    if (!*s->f01c) return 1;
    s->ini_name();
    s->log(s->m_connected, 0, 0, 0, 0);
    return 1;
}
