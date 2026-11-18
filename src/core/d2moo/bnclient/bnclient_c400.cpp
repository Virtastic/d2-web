// 1.13c BNClient+0xC400 (ECX = text): send a chat line, frame 0x0E.
// A null or empty text returns 0. The text is widened (at most 0x1FF characters; none returns 0), terminated, and
// narrowed again into a 0x1FF-byte buffer; a failed narrowing returns 0. The narrowed bytes are terminated and sent
// with the count the narrowing reported as the length. Returns 1 when the send did not fail, else 0.
#include <cstdint>
#include <cstring>

struct BnC400Env {
    uint32_t (*widen)(uint16_t* dst, const char* src, uint32_t cap);                                               // returns the character count
    uint32_t (*narrow)(char* dst, uint32_t cap, const uint16_t* src, uint32_t n, uint32_t* count, uint32_t flags);  // returns 0 when it worked
    int32_t (*send)(uint32_t len, uint8_t id, const uint8_t* payload);
};

// 1.13c BNClient+0xC400
extern "C" uint32_t bnclient_c400(const BnC400Env* e, const char* text) {
    if (!text || !*text) return 0;
    uint16_t wide[0x200];
    char out[0x200];
    uint32_t count = e->widen(wide, text, 0x1FF);
    if (count == 0) return 0;
    wide[count] = 0;
    if (e->narrow(out, 0x1FF, wide, 0x1FF, &count, 0) != 0) return 0;
    out[count] = 0;
    return e->send(count, 0x0E, (const uint8_t*)out) >= 0 ? 1u : 0u;
}
