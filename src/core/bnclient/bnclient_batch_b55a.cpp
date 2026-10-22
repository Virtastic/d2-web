// BNClient.dll alpha-lane batch b55-bnc-a: BNCS error slot / message queue / connect-wait / packet
// handlers, from the cached Ghidra decompile of retail 1.13c Bnclient.dll (base 0x6FF20000). Win32 and
// unrecovered internal callees are seams on BnB55Env; retail globals live in BnB55State.
// Skipped (register-passed args or uninitialised-local artefacts): 0x0FE00, 0x0FEC0, 0x11BB0; skipped
// (large Win32 surface, no faithful seam yet): 0x11780, 0x11960, 0x11B00.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct BnB55Env {
    void (*enter_cs)();                      // EnterCriticalSection(&DAT_6FF3F1DC)
    void (*leave_cs)();                      // LeaveCriticalSection(&DAT_6FF3F1DC)
    void (*fatal)(uint32_t msg_id);          // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    void (*free_msg)(uint32_t tag, void* p); // Ordinal_10043(tag, 0)-style release
    uint32_t (*tick_count)();                // GetTickCount
    void (*sleep_ms)(uint32_t ms);           // Sleep
    void (*set_error)(uint32_t code);        // FUN_6FF2FCF0 (BNClient+0x0FCF0)
    int  (*parse_0)(const void* p);          // FUN_6FF32000 (BNClient+0x12000)
    void (*aed0)(void* p);                   // FUN_6FF2AED0 (BNClient+0x0AED0)
    void (*send)(const void* p, uint32_t n); // Ordinal_491 / Ordinal_10007 pair
    void (*notify)(uint32_t ev, uint32_t a, uint32_t b, uint32_t c, const char* s, const char* t); // BNClient+0x0E120
};

struct BnB55State {
    uint32_t err_code;            // DAT_6FF3F1F4
    const void* err_text;         // DAT_6FF3F1D8
    char err_text_buf[0x80];      // DAT_6FF3F158
    uint32_t* msg_head;           // DAT_6FF3F64C (next link at +0x128)
    uint32_t ready[256];          // DAT_6FF3ED58
    uint32_t aborted;             // DAT_6FF3F644
    uint32_t pkt5_count;          // DAT_6FF3EB50
    uint32_t pkt1_flag;           // DAT_6FF3E120
    uint32_t have_sock;           // DAT_6FF3F654
    uint32_t sock_busy;           // DAT_6FF3F660
    uint32_t ver_a, ver_b;        // DAT_6FF3F874 / DAT_6FF3F878
    char file_name[0x104];        // DAT_6FF3F770
    char file_aux[0x100];         // DAT_6FF3F510
    uint8_t scratch[0x20];        // DAT_6FF3F8E4 target
};

// 1.13c BNClient+0x0FBD0
extern "C" uint32_t bnb55_pop_error(const BnB55Env* e, BnB55State* s, int32_t* out) {
    if (!out) { e->fatal(0xE5); return 0; }
    e->enter_cs();
    const void* t = s->err_text;
    out[0] = static_cast<int32_t>(s->err_code);
    out[1] = static_cast<int32_t>(reinterpret_cast<uintptr_t>(t));
    if (s->err_code == 0x1B) out[1] = static_cast<int32_t>(reinterpret_cast<uintptr_t>(s->err_text_buf));
    s->err_code = 0;
    e->leave_cs();
    return 1;
}

// 1.13c BNClient+0x0FCA0
extern "C" uint32_t bnb55_set_error(const BnB55Env* e, BnB55State* s, uint32_t code, const char* text) {
    e->enter_cs();
    if (s->err_code == 0) {
        s->err_code = code;
        size_t n = 0;
        while (n < 0x80 && text[n]) n++;
        std::memcpy(s->err_text_buf, text, n);
        if (n < 0x80) s->err_text_buf[n] = 0;   // Ordinal_501: bounded copy, NUL-terminated if room
    }
    e->leave_cs();
    return 1;
}

// 1.13c BNClient+0x0FD20
extern "C" uint32_t bnb55_flush_queue(const BnB55Env* e, BnB55State* s) {
    e->enter_cs();
    while (s->msg_head) {
        uint32_t* cur = s->msg_head;
        s->msg_head = *reinterpret_cast<uint32_t**>(reinterpret_cast<uint8_t*>(cur) + 0x128);
        e->free_msg(0xA2, cur);
    }
    e->leave_cs();
    return 1;
}

// 1.13c BNClient+0x10D90
extern "C" uint32_t bnb55_wait_ready(const BnB55Env* e, BnB55State* s, uint8_t idx) {
    uint32_t t0 = e->tick_count();
    for (;;) {
        if (s->ready[idx] != 0) return 1;
        if (s->aborted != 0) break;
        if (e->tick_count() - t0 > 45000) {
            e->enter_cs();
            if (s->err_code == 0) s->err_code = 0x11;
            e->leave_cs();
            return 0;
        }
        e->sleep_ms(10);
    }
    e->enter_cs();
    if (s->err_code == 0) s->err_code = 0x12;
    e->leave_cs();
    return 0;
}

// 1.13c BNClient+0x11220
extern "C" uint32_t bnb55_handle_net_packet(const BnB55Env* e, BnB55State* s, const int32_t* p, uint32_t len) {
    if (len <= 0x43) return 0;
    int32_t kind = p[0];
    if (kind == 0) return static_cast<uint32_t>(e->parse_0(p + 1));
    if (kind == 1) { e->set_error(0xD); s->pkt1_flag = 1; return 1; }
    if (kind == 5 && ++s->pkt5_count < 2) {
        e->aed0(s->scratch);
        static const uint8_t pkt[4] = {0xFF, 0x57, 0x04, 0x00};   // 0xFF 0x57 len=4
        if (s->have_sock != 0 && s->sock_busy == 0) { e->send(pkt, 4); return 1; }
        s->aborted = 1;
        return 1;
    }
    e->set_error(0x12);
    return 0;
}

// 1.13c BNClient+0x115B0
extern "C" uint32_t bnb55_handle_file_info(const BnB55Env* e, BnB55State* s, const uint32_t* p, uint32_t len) {
    const char* b = reinterpret_cast<const char*>(p);
    if (len <= 0x15 || b[len - 1] != '\0') return 0;
    int nuls = 0;
    for (uint32_t i = 0x14; i < len - 1; i++) if (b[i] == '\0') nuls++;
    if (nuls != 1) return 0;
    const char* str = b + 0x14;
    s->ver_a = p[1];
    s->ver_b = p[0];
    size_t l = std::strlen(str);
    size_t n = 0;
    while (n < 0x104 && str[l + 1 + n]) n++;
    std::memcpy(s->file_name, str + l + 1, n);
    if (n < 0x104) s->file_name[n] = 0;
    s->file_aux[0] = 0;
    e->notify(0x80000001u, 3, 0, 0, str, nullptr);
    return 1;
}
