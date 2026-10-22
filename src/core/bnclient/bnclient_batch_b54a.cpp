// BNClient.dll alpha-lane batch b54-bnc-a: the BNCS frame sender (0x0EB00), the receive loop
// (0x0EC50) and the shutdown path (0x0EE20), transcribed from the cached Ghidra decompile of retail
// 1.13c Bnclient.dll. Win32 and the unrecovered callees are host seams on an env struct; the retail
// globals are the fields of BnNetState. Other rows of this batch were skipped: their decompile loses
// register arguments (unaff_E*), so a faithful body cannot be written from it.
#include <cstdint>
#include <cstring>

struct BnNetState {
    void* conn;                 // DAT_6FF3F654
    int32_t closing;            // DAT_6FF3F660
    int32_t disconnect;         // DAT_6FF3F644
    int32_t reader_active;      // DAT_6FF3F658
    int32_t reader_ready;       // DAT_6FF3F65C
    int32_t conn_busy;          // DAT_6FF3F664
    int32_t shutdown_req;       // DAT_6FF3F614
    int32_t workers;            // DAT_6FF3F66C
    int32_t evt_a, evt_b;       // DAT_6FF3F880 / DAT_6FF3F888
    uint32_t* session;          // DAT_6FF3F8E4: [0] live flag, [1],[2] heap blocks
    int32_t conn_flag;          // *(conn + 0x2c)
};

struct BnNetEnv {
    int32_t (*send)(uint32_t len);                 // Ordinal_10007 (frame already in the send buffer)
    int32_t (*poll)();                             // Ordinal_10008
    int32_t (*recv)(int32_t room);                 // Ordinal_10006
    int32_t (*consume)(int32_t total);             // BNClient+0x0EA60
    void (*close_conn)();                          // Ordinal_10005
    void (*release)(int32_t id, int32_t arg);      // Ordinal_10043
    void (*teardown_a)();                          // BNClient+0x132D0
    void (*teardown_b)();                          // BNClient+0x0FD20
    void (*free_block)(uint32_t p);                // Ordinal_606
    void (*sleep_ms)(uint32_t ms);                 // Sleep
    uint32_t (*tick)();                            // GetTickCount
    uint8_t* sendbuf;                              // Ordinal_491 destination (8 KiB)
};

// 1.13c BNClient+0x0EB00: frame = FF, id, u16 len+4, payload.
int32_t bnclient_send_frame(BnNetState* s, const BnNetEnv* e, uint32_t len, uint8_t id,
                            const void* payload) {
    if (s->conn != nullptr && s->closing == 0) {
        if (payload == nullptr) len = 0;
        e->sendbuf[0] = 0xff;
        e->sendbuf[1] = id;
        e->sendbuf[2] = (uint8_t)((len + 4) & 0xff);
        e->sendbuf[3] = (uint8_t)(((len + 4) >> 8) & 0xff);
        if (payload != nullptr && len + 4 < 0x2000) memcpy(e->sendbuf + 4, payload, len);
        return e->send(len + 4);
    }
    s->disconnect = 1;
    return 0;
}

// 1.13c BNClient+0x0EC50
int32_t bnclient_recv_loop(BnNetState* s, const BnNetEnv* e) {
    int32_t ret = 1;
    if (s->reader_active != 0) return 1;
    s->reader_active++;
    while (s->closing == 0 && s->reader_ready == 0) e->sleep_ms(10);
    uint32_t last = e->tick();
    if (s->conn != nullptr) {
        int32_t have = 0;
        do {
            if (s->closing != 0 || s->conn_flag != 0) break;
            int32_t ready = e->poll();
            if (s->conn == nullptr || s->closing != 0 || s->conn_flag != 0) break;
            if (ready != 0) {
                int32_t n = e->recv(0x2000 - have);
                if (n < 1) { s->disconnect = 1; ret = 0; break; }
                have = e->consume(n + have);
            }
            uint32_t now = e->tick();
            if (now - last > 179999) {            // keepalive: FF 00 04 00
                last = now;
                if (s->conn == nullptr || s->closing != 0) s->disconnect = 1;
                else {
                    e->sendbuf[0] = 0xff; e->sendbuf[1] = 0; e->sendbuf[2] = 4; e->sendbuf[3] = 0;
                    e->send(4);
                }
            }
        } while (s->conn != nullptr);
    }
    s->reader_active--;
    return ret;
}

// 1.13c BNClient+0x0EE20
int32_t bnclient_shutdown(BnNetState* s, const BnNetEnv* e) {
    if (s->evt_a != 0) e->release(0x224, 0);
    s->evt_a = 0;
    if (s->evt_b != 0) e->release(0x39a, 0);
    s->evt_b = 0;
    e->teardown_a();
    e->teardown_b();
    if (s->session[0] != 0) { e->free_block(s->session[2]); e->free_block(s->session[1]); }
    s->session[0] = 0;
    s->closing = 1;
    while (s->reader_active != 0) e->sleep_ms(10);
    s->reader_active = 0;
    if (s->conn != nullptr) {
        s->disconnect = 1;
        while (s->conn_busy != 0) e->sleep_ms(10);
        if (s->conn != nullptr) { e->close_conn(); s->conn = nullptr; }
    }
    s->shutdown_req = 1;
    while (s->workers > 0) e->sleep_ms(10);
    if (s->conn_busy != 0) {
        int32_t t = 0;
        do {
            t += 100;
            e->sleep_ms(10);
            if (t > 0x3c) return 0;
        } while (s->conn_busy != 0);
    }
    return 1;
}
