// BNClient 0x11330: the BNCS message 0x51 (SID_AUTH_CHECK) entry of the receive dispatch table (0x1E128 + 0x51*4).
#include <cstdint>
#include <emscripten.h>

// Globals the handler writes: 0x6FF3EFE0, 0x6FF3F01C, 0x6FF3F1F4 (under the critical section at
// 0x6FF3F1DC), and the 0x100-byte text buffer at 0x6FF3F058.
struct Bn11330State {
    uint32_t auth_ok;       // 0x6FF3EFE0
    uint32_t auth_done;     // 0x6FF3F01C
    uint32_t key_error;     // 0x6FF3F1F4
    char info[0x100];       // 0x6FF3F058
};

struct Bn11330Env {
    // Fog#10263(ECX = buffer, EDX = &size): fills an optional 0x5D reply; nonzero = send it.
    uint32_t (*build_5d)(uint8_t* buf, uint32_t* size);
    int32_t (*send_frame)(uint32_t size, uint8_t id, const void* payload);         // 0x0EB00
    // Storm#424(section, key, flags 0x82, data, size): writes the "Patch"/"Patches" registry value.
    void (*reg_write)(const char* section, const char* key, uint32_t flags, const void* data, uint32_t size);
    uint32_t (*str_copy)(char* dst, const char* src, uint32_t max);                 // Storm#501
    void (*patch_list)(uint8_t* buf, uint32_t len);                                 // 0x0D580
    void (*start)(uint32_t kind, uint8_t flags, uint32_t p3, uint32_t p4, const char* name,
                  const char* url, const uint32_t* filetime);                       // 0x0E120
    void (*version_fail)(uint32_t reason);                                          // 0x0FCF0
    void (*key_fail)(uint32_t reason);                                              // 0x0FC70
    void (*key_fail_text)(const char* text);                                        // 0x0FC40
    void (*lock)();                                                                 // EnterCriticalSection
    void (*unlock)();                                                               // LeaveCriticalSection
    const char* reg_section;                                                        // 0x6FF3AC3C
    const char* reg_key;                                                            // 0x6FF3AC44
};

// 1.13c BNClient+0x11330 (638 B): handler(ECX = payload, EDX = size).
// Payload: +0 u32 result, +4 additional info (NUL). Rejected (0) under 5 bytes, without a final NUL,
// or with a NUL anywhere in [4, size-1). r = result & 0xFF0F; bit group 0xF0 picks the
// second-key variant (+4 to the key error code / 7 instead of 3).
// For r in {0, 0x100, 0x102} the optional 0x5D reply is built and sent first. Every accepted packet
// clears the "Patch" registry value, info[0] and auth_ok, then: 0 = accepted (auth_ok = done = 1);
// 0x100 = version upgrade (info + an extra NUL goes through 0x0D580, into the registry, and becomes a
// kind 0x80000002 flags 2 download with a zero FILETIME; done = 0); 0x101/0x102 = version failure 1,
// done = 0; 0x200 = key_error 3 or 7; 0x201 key in use (code 6/10, info copied to the text buffer and
// reported); 0x202 banned (4/8); 0x203 wrong product (5/9); anything else only the common part.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid_auth_check_11330(const uint8_t* payload, uint32_t size,
                                                                       Bn11330State* s, const Bn11330Env* e) {
    if (size < 5 || payload[size - 1] != 0) return 0;
    for (uint32_t i = 4; i < size - 1; ++i)
        if (payload[i] == 0) return 0;
    uint32_t result = (uint32_t)payload[0] | (uint32_t)payload[1] << 8 | (uint32_t)payload[2] << 16 |
                      (uint32_t)payload[3] << 24;
    uint32_t r = result & 0xFF0F;
    uint32_t alt = (result & 0xF0) ? 4 : 0;
    const char* info = (const char*)(payload + 4);
    if (r == 0 || r == 0x100 || r == 0x102) {
        uint8_t reply[0x5DC];
        uint32_t len = 0x5DC;
        if (e->build_5d(reply, &len)) e->send_frame(len, 0x5D, reply);
    }
    s->info[0] = 0;
    s->auth_ok = 0;
    e->reg_write(e->reg_section, e->reg_key, 0x82, "", 1);
    switch (r) {
    case 0:
        s->auth_ok = 1;
        s->auth_done = 1;
        break;
    case 0x100: {
        uint8_t list[0x100];
        uint32_t n = e->str_copy((char*)list, info, 0xFE);
        list[n + 1] = 0;
        n += 2;
        e->patch_list(list, n);
        e->reg_write(e->reg_section, e->reg_key, 0x82, list, n);
        uint32_t ft[2] = {0, 0};
        e->start(0x80000002u, 2, 0, 0, (const char*)list, nullptr, ft);
        s->auth_done = 0;
        break;
    }
    case 0x101:
    case 0x102:
        e->version_fail(1);
        s->auth_done = 0;
        break;
    case 0x200:
        e->lock();
        s->key_error = alt + 3;
        e->unlock();
        break;
    case 0x201:
        e->key_fail(alt + 6);
        e->str_copy(s->info, info, 0x100);
        e->key_fail_text(s->info);
        break;
    case 0x202:
        e->key_fail(alt + 4);
        break;
    case 0x203:
        e->key_fail(alt + 5);
        break;
    default:
        break;
    }
    return 1;
}
