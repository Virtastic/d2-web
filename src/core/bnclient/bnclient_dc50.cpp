// BNClient 0xDC50 (stdcall thread proc, one argument: the 0x230-byte request from 0xE120/0xE740; ret 4): the download thread.
// The address comes from the gateway server DAT_6FF3F654 (Fog 10012, port 0x17E0) or, failing that, the first strtok
// token of the host list 0xF240 fills from the gateway name (0xF3C0). It connects (kind 0x80000002 retries up to 3 times,
// 1 s apart, on the next token), sends the byte 2 and the request, reads a dword size (< 0x100, else fatal) and the rest
// of the reply block into SMemAlloc(size), then: flags 2 checks the local copy (0xCF10; a changed value is stored at
// request+0x14 and the request is re-sent on a new socket) and receives into the file (0xCFA0); other flags receive into
// a buffer (0xDB70). A received file goes to 0xE2E0. Teardown closes, frees, resets the patch list for kind 0x80000002
// when DAT_6FF3F508 < 100, decrements DAT_6FF3F66C and calls _endthreadex(0).
#include <cstdint>
#include <cstring>

struct BnDc50Seams {
    uint32_t (*resolve)(uint32_t server, uint8_t* sa, uint32_t* salen);              // 0x812C Fog 10012 (ECX, EDX; 1 stack)
    void (*gw_name)(char* out, uint32_t max);                                          // 0xF3C0 (ESI = out, EDI = max)
    void (*hosts)(char* out, const char* name);                                        // 0xF240 (EAX = out; stack name)
    char* (*tok)(char* s, uint32_t delims);                                            // 0x1D60 strtok
    uint16_t (*hton)(uint32_t v);                                                      // 0x7FFA ws2 9 htons
    uint32_t (*addr)(const char* s);                                                   // 0x7FD6 ws2 10 inet_addr
    uint32_t (*sock)(uint32_t af, uint32_t type, uint32_t proto);                      // 0x7FE8 ws2 23 socket
    uint32_t (*connect)(uint32_t s, const uint8_t* sa, uint32_t len);                 // 0x8000 ws2 4 connect
    void (*close)(uint32_t s);                                                         // 0x813E (ECX = socket)
    void (*sleep)(uint32_t ms);                                                        // Sleep
    void (*setopt)(uint32_t s, uint32_t level, uint32_t opt, const uint32_t* v, uint32_t len); // 0x7FF4 ws2 21
    uint32_t (*send)(uint32_t s, const uint8_t* b, uint32_t n, uint32_t fl);          // 0x7FE2 ws2 19
    int32_t (*recv)(uint32_t s, uint8_t* b, uint32_t n, uint32_t fl);                 // 0x7FEE ws2 16
    uint8_t* (*alloc)(uint32_t n, uint32_t file, uint32_t line, uint32_t fl);         // 0x8018 Storm 401
    void (*free_)(uint32_t p, uint32_t file, uint32_t line, uint32_t fl);             // 0x801E Storm 403
    void (*fatal)(uint32_t line);                                                      // 0x80E4 + 0x80DE + _exit(-1)
    uint32_t (*check)(const uint8_t* ft, const char* name);                            // 0xCF10 (EAX = ft, ECX = name)
    uint32_t (*to_file)(uint32_t ref, const char* name, uint32_t s, uint32_t size, const uint8_t* ft, uint32_t kind); // 0xCFA0 (EAX, ECX)
    uint32_t (*to_buf)(uint32_t* data, uint32_t s, const char* name, uint32_t size, const uint8_t* ft, uint32_t kind,
                       uint32_t flags);                                                // 0xDB70 (EAX = &data, ECX = s)
    void (*deliver)(uint32_t kind, uint32_t flags, uint32_t a, uint32_t b, const uint8_t* ft, const char* name,
                    const char* text, uint32_t data, uint32_t size, uint32_t one);   // 0xE2E0
    void (*enter)(uint32_t cs);
    void (*leave)(uint32_t cs);
    void (*dec)(uint32_t counter);                                                     // InterlockedDecrement
    void (*end)(uint32_t code);                                                        // 0x1E59 _endthreadex
    const uint32_t* server;   // DAT_6FF3F654
    uint32_t* abort_;         // DAT_6FF3F614
    uint32_t* state;          // DAT_6FF3F508
    uint8_t* list;            // DAT_6FF3F670
    uint32_t cs, counter, delims, file;  // DAT_6FF3F8E8, DAT_6FF3F66C, DAT_6FF3AE80, BnDownload.cpp (DAT_6FF3AFCC)
};

static uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }

// 1.13c BNClient+0x0DC50
extern "C" uint32_t bnclient_download_thread_dc50(uint8_t* req, const BnDc50Seams* s) {
    uint32_t data = 0, n = 0, retries = 0, salen = 0x10, size;
    char* token = nullptr;
    char hostbuf[0x200];
    char name[0x100];
    uint8_t sa[16];
    uint8_t copy[0x230];
    uint8_t* body = nullptr;
    hostbuf[0] = 0;
    *s->abort_ = 0;
    const uint32_t zero = 0;
    if (*s->server != 0 && s->resolve(*s->server, sa, &salen) != 0) {
        const uint16_t port = s->hton(0x17E0);
        memcpy(sa + 2, &port, 2);
        memcpy(sa + 8, &zero, 4);
        memcpy(sa + 12, &zero, 4);
    } else {
        s->gw_name(name, 0x100);
        s->hosts(hostbuf, name);
        token = s->tok(hostbuf, s->delims);
        memset(sa, 0, 16);
        const uint16_t af = 2;
        memcpy(sa, &af, 2);
        const uint16_t port = s->hton(0x17E0);
        memcpy(sa + 2, &port, 2);
        const uint32_t a = s->addr(token);
        memcpy(sa + 4, &a, 4);
    }
    uint32_t sock = s->sock(2, 1, 0);
    while (sock != 0xFFFFFFFFu) {
        if (s->connect(sock, sa, 0x10) != 0) {
            if (*s->abort_ != 0 || rd32(req + 0x228) != 0x80000002u) break;
            s->close(sock);
            sock = 0xFFFFFFFFu;
            if (++retries > 3) break;
            s->sleep(1000);
            if (hostbuf[0] != 0) token = s->tok(token == nullptr ? hostbuf : nullptr, s->delims);
            if (token == nullptr) break;
            const uint32_t a = s->addr(token);
            memcpy(sa + 4, &a, 4);
            sock = s->sock(2, 1, 0);
            continue;
        }
        const uint32_t one = 1, rcvto = 15000;
        s->setopt(sock, 0xFFFF, 8, &one, 4);
        s->setopt(sock, 0xFFFF, 0x1006, &rcvto, 4);
        uint8_t two = 2;
        if (s->send(sock, &two, 1, 0) != 1) break;
        memcpy(copy, req, 0x230);
        uint16_t len16;
        memcpy(&len16, req, 2);
        if (s->send(sock, copy, len16, 0) != len16) break;
        uint8_t szb[4] = { 0, 0, 0, 0 };
        uint32_t got = 0;
        bool fail = false;
        do {
            const int32_t r = s->recv(sock, szb + got, 4 - got, 0);
            if (r == -1 || r <= 0) { fail = true; break; }
            got += (uint32_t)r;
        } while (got < 4);
        if (fail) break;
        size = rd32(szb);
        if (size >= 0x100) { s->fatal(0x2EF); return 0; }
        body = s->alloc(size, s->file, 0x2F0, 0);
        if (body == nullptr) goto teardown;
        n = size - 4;
        for (uint32_t done = 0; done < n;) {
            const int32_t r = s->recv(sock, body + done + 4, n - done, 0);
            if (r == -1 || r <= 0) goto teardown;
            done += (uint32_t)r;
        }
        if (*s->abort_ != 0) goto teardown;
        {
            const uint32_t flags = rd32(req + 0x22C);
            uint32_t line;
            if (flags == 2) {
                const uint32_t v = s->check(body + 0x10, (const char*)body + 0x18);
                if (v != rd32(req + 0x14)) {
                    memcpy(req + 0x14, &v, 4);
                    s->close(sock);
                    line = 0x312;
                    goto again;
                }
                if (*s->abort_ != 0) goto teardown;
            }
            if (flags == 2) {
                if (s->to_file(rd32(req + 0x14), (const char*)body + 0x18, sock, rd32(body + 4), body + 0x10,
                               rd32(req + 0x228)) != 0) goto deliver;
                if (*s->abort_ != 0) goto teardown;
                s->close(sock);
                line = 0x332;
            } else {
                if (s->to_buf(&data, sock, (const char*)body + 0x18, rd32(body + 4), body + 0x10, rd32(req + 0x228),
                              flags) != 0) goto deliver;
                if (*s->abort_ != 0) goto teardown;
                s->close(sock);
                line = 0x34D;
            }
        again:
            s->free_((uint32_t)(uintptr_t)body, s->file, line, 0);
            body = nullptr;
            sock = s->sock(2, 1, 0);
            continue;
        deliver:
            s->deliver(rd32(req + 0x228), rd32(req + 0x22C), rd32(body + 8), rd32(body + 0xC), body + 0x10,
                       (const char*)body + 0x18, (const char*)req + 0x124, data, rd32(body + 4), 0);
            goto teardown;
        }
    }
teardown:
    {
        const uint32_t kind = rd32(req + 0x228);
        if (sock != 0xFFFFFFFFu) s->close(sock);
        if (data != 0) s->free_(data, s->file, 0x367, 0);
        if (body != nullptr) s->free_((uint32_t)(uintptr_t)body, s->file, 0x368, 0);
        s->free_((uint32_t)(uintptr_t)req, s->file, 0x369, 0);
        if (kind == 0x80000002u && *s->state < 100) {
            s->enter(s->cs);
            s->list[0] = 0;
            s->list[1] = 0;
            *s->state = 0x65;
            s->leave(s->cs);
        }
        s->dec(s->counter);
        s->end(0);
    }
    return 0;
}
