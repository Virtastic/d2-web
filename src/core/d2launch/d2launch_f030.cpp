// 1.13c D2Launch+0xF030: flag a .d2s save (bit 0x20 of the flags dword) and write it back. The D2Win file API
// and sprintf are seams so a replay probe pins them.
#include <cstdint>
#include <cstring>
struct D2lF030Seams {
    void (*dirbuf)(char* buf, uint32_t cap);                                                  // 0x97CC (ECX = buf, EDX = 0x400)
    void (*spf)(char* out, const char* fmt, const char* dir, const char* name);                // sprintf (cdecl) with fmt = image 0x1DC50
    uint32_t (*open)(const char* path, uint32_t access, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t w4);   // 0x97E4
    void (*read)(uint32_t h, uint8_t* buf, uint32_t cap, uint32_t* nread, uint32_t z);         // 0x9802
    void (*seek)(uint32_t h, uint32_t off, uint32_t a, uint32_t b);                           // 0x983E
    uint32_t (*chk)(const uint8_t* buf, uint32_t n);                                          // 0x9838
    uint32_t (*write)(uint32_t h, const uint8_t* buf, uint32_t n, uint32_t* wrote, uint32_t z); // 0x984A
    void (*close)(uint32_t h);                                                                // 0x9808
};
extern "C" uint32_t d2l_f030(const D2lF030Seams* s, const char* fmt, const char* name) {
    char dir[0x400], path[0x200];
    uint8_t buf[0x2000];
    uint32_t nread = 0, wrote = 0, tmp = 0;
    s->dirbuf(dir, 0x400);
    s->spf(path, fmt, dir, name);
    uint32_t h = s->open(path, 0xC0000000u, 0, 0, 3, 0x8000000u, 0);
    if (!h) return 0;
    s->read(h, buf, 0x2000, &nread, 0);
    if (nread < 8) return 0;
    uint32_t magic, ver;
    memcpy(&magic, buf, 4); memcpy(&ver, buf + 4, 4);
    if (magic != 0xAA55AA55u) return 0;
    uint32_t r;
    if (ver <= 0x5B) {
        if (nread < 0x82) return 0;
        memcpy(&tmp, buf + 0x18, 4); tmp |= 0x20;
        s->seek(h, 0x18, 0, 0);
        r = s->write(h, (const uint8_t*)&tmp, 4, &wrote, 0);
    } else {
        if (nread < 0x14F) return 0;
        memcpy(&tmp, buf + 0x24, 4); tmp |= 0x20;
        memcpy(buf + 0x24, &tmp, 4);
        memset(buf + 0xC, 0, 4);
        uint32_t c = s->chk(buf, nread);
        memcpy(buf + 0xC, &c, 4);
        s->seek(h, 0, 0, 0);
        r = s->write(h, buf, nread, &wrote, 0);
    }
    s->close(h);
    return r;
}
