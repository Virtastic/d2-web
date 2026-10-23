// 1.13c D2Win+0x3A6C (sub_3A6C, MSVC CRT _write_nolock): write n bytes to fd; text mode
// (osfile&0x80) expands LF to CRLF through a 0x400-byte chunk buffer. Returns bytes written
// minus inserted CRs, 0 for ^Z on a device (osfile&0x40), else -1 with errno/doserrno set.
#include <cstdint>

struct D2Win3A6CEnv {
    uint8_t* (*osfile)(uint32_t fd);                                   // &osfile flag byte of fd
    void (*lseekEnd)(uint32_t fd);                                     // __lseeki64_lk(fd,0,0,2)
    bool (*writeFile)(uint32_t fd, const char* p, uint32_t n, uint32_t* wr, uint32_t* err);
    void (*setErr)(int32_t errnoV, uint32_t doserr);                   // *_errno, *__doserrno
    void (*dosmaperr)(uint32_t err);
};

// 1.13c D2Win+0x3A6C
int32_t d2win_sub_3a6c(uint32_t fd, const char* buf, uint32_t n, const D2Win3A6CEnv& env) {
    uint32_t total = 0, err = 0, wr = 0;
    int32_t crs = 0;
    if (n == 0) return 0;
    uint8_t* fl = env.osfile(fd);
    if (*fl & 0x20) env.lseekEnd(fd);
    if (!(*fl & 0x80)) {
        if (!env.writeFile(fd, buf, n, &wr, &err)) { /* err set */ } else { err = 0; total = wr; }
    } else {
        const char* p = buf;
        char tmp[0x400 + 2];
        for (;;) {
            int32_t cnt = 0;
            char* o = tmp;
            while ((uint32_t)(p - buf) < n && cnt < 0x400) {
                char c = *p++;
                if (c == '\n') { crs++; *o++ = '\r'; cnt++; }
                *o++ = c; cnt++;
            }
            int32_t len = (int32_t)(o - tmp);
            if (!env.writeFile(fd, tmp, (uint32_t)len, &wr, &err)) break;
            total += wr;
            if (!(len <= (int32_t)wr && (uint32_t)(p - buf) < n)) break;
        }
    }
    if (total) return (int32_t)total - crs;
    if (err) {
        if (err == 5) { env.setErr(9, 5); return -1; }
        env.dosmaperr(err);
        return -1;
    }
    if ((*fl & 0x40) && *buf == 0x1a) return 0;
    env.setErr(0x1c, 0);
    return -1;
}
