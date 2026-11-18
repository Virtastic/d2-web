// BNClient 0xEC50 (stdcall, one unused stack argument; ret 4): the BNCS receive loop. It runs once (DAT_6FF3F658 is the
// reader count), waits for the ready or closing flag, then while the connection DAT_6FF3F654 lives: polls it (Fog 10008),
// receives into the 0x2000-byte buffer (Fog 10006), hands the bytes to the frame pump 0xEA60 and keeps the partial rest,
// and every 180000 ms sends the keepalive FF 00 04 00 (Storm 491 into the send buffer, Fog 10007). A failed receive sets
// DAT_6FF3F644 and returns 0; otherwise it returns 1.
#include <cstdint>

struct BnEC50Seams {
    void (*inc)(int32_t* p);                                   // InterlockedIncrement
    void (*dec)(int32_t* p);                                   // InterlockedDecrement
    void (*sleep)(uint32_t ms);                                // Sleep
    uint32_t (*tick)();                                        // GetTickCount
    int32_t (*poll)(uint8_t* conn);                            // 0x8120 Fog 10008 (ECX conn)
    int32_t (*recv)(uint8_t* conn, uint8_t* buf, int32_t n);   // 0x8108 Fog 10006 (ECX conn, EDX buf, stack n)
    int32_t (*pump)(uint8_t* buf, int32_t total);              // 0xEA60 (EAX buf, stack total)
    void (*copy)(void* dst, const void* src, uint32_t n);      // 0x807E Storm 491
    void (*send)(uint8_t* conn, uint8_t* buf, uint32_t n);     // 0x80FC Fog 10007 (ECX conn, EDX buf, stack n)
    int32_t* disconnect;                                       // DAT_6FF3F644
    uint8_t** conn;                                            // DAT_6FF3F654 (+0x2c: the closed flag)
    int32_t* active;                                           // DAT_6FF3F658
    int32_t* ready;                                            // DAT_6FF3F65C
    int32_t* closing;                                          // DAT_6FF3F660
};

static int32_t ec50_closed(const uint8_t* c) { return *(const int32_t*)(c + 0x2C); }

// 1.13c BNClient+0x0EC50
extern "C" uint32_t bnclient_recv_loop_ec50(const BnEC50Seams* s) {
    uint8_t hdr[4];
    uint32_t result = 1;
    uint8_t sendbuf[0x2000];
    uint8_t buf[0x2000];
    if (*s->active != 0) return 1;
    s->inc(s->active);
    if (*s->closing == 0) {
        while (*s->ready == 0) {
            s->sleep(10);
            if (*s->closing != 0) break;
        }
    }
    uint32_t last = s->tick();
    int32_t have = 0;
    uint8_t* c = *s->conn;
    if (c != nullptr) {
        do {
            if (*s->closing != 0 || ec50_closed(c) != 0) break;
            const int32_t avail = s->poll(c);
            c = *s->conn;
            if (c == nullptr || *s->closing != 0 || ec50_closed(c) != 0) break;
            if (avail != 0) {
                const int32_t n = s->recv(c, buf + have, 0x2000 - have);
                if (n < 1) { *s->disconnect = 1; result = 0; break; }
                have = s->pump(buf, n + have);
            }
            const uint32_t now = s->tick();
            c = *s->conn;
            if (now - last >= 0x2BF20) {
                hdr[0] = 0xFF; hdr[1] = 0; hdr[2] = 4; hdr[3] = 0;
                if (c == nullptr || *s->closing != 0) *s->disconnect = 1;
                else {
                    s->copy(sendbuf, hdr, 4);
                    s->send(*s->conn, sendbuf, 4);
                    c = *s->conn;
                }
                last = now;
            }
        } while (c != nullptr);
    }
    s->dec(s->active);
    return result;
}
