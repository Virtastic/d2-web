// 1.13c D2Client+0x1FDD0: builds a unit's text line into a UTF-16 buffer.
// eax(low16)=string id, pUnit, buf, size (chars). Strings come from D2Lang (thunk 0xD1EC), widths from 0xD35A, font from 0xD3A8.
#include <cstdint>
struct Client1FDD0Callees {
    const uint16_t* (*lang)(uint32_t ecx);   // D2Lang string by id; ecx is the full register, retail leaves stale high bits on 16-bit loads
    uint32_t (*font)(uint32_t);              // D2Win set-font, returns the previous font
    int32_t (*width)(const uint16_t*);       // D2Win text width
};
static uint32_t len16(const uint16_t* s) { uint32_t n = 0; while (s[n]) ++n; return n; }
static void cat16(uint16_t* d, const uint16_t* s) { d += len16(d); while ((*d++ = *s++)) {} }
// item = pItemData (flags byte +0x16, nine affix-id bytes +0x1c); tbl = word[256] at D2Client+0xF9F88; sep = Unicode at +0x1223D0;
// sepEcx = ecx right after the separator width call (ecx is caller-saved: the capture recorder leaves 0x01DB in the high word; real D2Win would leave its own).
extern "C" void d2_client_1fdd0(uint32_t eax, uint32_t type, int32_t classId, const uint8_t* item, uint16_t* buf, int32_t size,
                                const uint16_t* tbl, const uint16_t* sep, uint32_t sepEcx, const Client1FDD0Callees& c) {
    buf[0] = 0;
    if (type != 1 || !item) return;
    const bool ax = (eax & 0xFFFF) != 0;
    if (item[0x16] & 0x10) {
        if (ax) { buf[0] = 0; const uint16_t* s = c.lang(eax); uint16_t* d = buf; while ((*d++ = *s++)) {}
                  cat16(buf, c.lang(0xF9B)); cat16(buf, c.lang(0xC95)); }
        else { const uint16_t* s = c.lang(0xC95); uint16_t* d = buf; while ((*d++ = *s++)) {} }
        return;
    }
    const uint32_t font = c.font(6);
    int32_t len = 0, w = 0; uint32_t count = 0;
    if (ax) {
        cat16(buf, c.lang(eax));
        len = (int32_t)len16(c.lang(eax));
        w = c.width(c.lang(eax));
        count = 1;
    }
    if (classId < 0x2C0 || classId > 0x2C5) {
        for (int k = 0; k < 9; ++k) {
            const uint32_t id = tbl[item[0x1C + k]];
            if (!id) continue;
            uint32_t ecx = count;
            if (count) {
                ecx = (uint32_t)(size - 1);
                if (len + 1 < size - 1) { cat16(buf, sep); ecx = sepEcx; w += c.width(sep); ++len; }
            }
            if (len < size - 1) {
                const uint16_t* s = c.lang((ecx & 0xFFFF0000u) | id);
                const int32_t l = (int32_t)len16(s);
                if (l + len < size - 1) { len += l; w += c.width(s); cat16(buf, s); ++count; }
            }
            if (w > 0x1E0) break;
        }
        c.font(font);
    }
}
