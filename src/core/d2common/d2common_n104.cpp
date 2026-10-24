#include <cstdint>
#include <cstring>

// Batch n104: CRT glue and the room-at-point accessor, written against host seams (pointers native width,
// registers of the retail custom conventions are plain arguments; fs:0 SEH chain is a host-owned slot).

// Room record fields read by 0x1330 (retail offsets in the comments).
struct D2N104Room {
    D2N104Room** apNear;   // +0x00 near-room array
    int32_t nNear;         // +0x24
    int32_t nX, nY;        // +0x4C, +0x50
    int32_t nW, nH;        // +0x54, +0x58
};

// (D2COMMON+0x01330; function list-bound to DUNGEON_GetRoomAtPosition, this is a seam-form copy)
// (room in EAX, x in EBX, y in EDI): NULL for NULL; the room itself when the point is inside its own
// rectangle; else the first near room whose rectangle holds the point; else NULL.
D2N104Room* d2n104_room_at_1330(D2N104Room* r, int32_t x, int32_t y)
{
    if (!r) return nullptr;
    if (x >= r->nX && x < r->nX + r->nW && y >= r->nY && y < r->nY + r->nH) return r;
    for (uint32_t i = 0; i < (uint32_t)r->nNear; ++i) {
        D2N104Room* n = r->apNear[i];
        if (n && n->nX <= x && x < n->nX + n->nW && n->nY <= y && y < n->nY + n->nH) return n;
    }
    return nullptr;
}

// FILE layout of the 32-bit CRT (flag word 0xC, fd 0x10, bufsiz 0x18).
struct D2N104File { uint8_t* ptr; int32_t cnt; uint8_t* base; uint32_t flag; int32_t file; int32_t pad[1]; int32_t bufsiz; };

struct D2N104FwriteEnv {
    int32_t (*flush)(D2N104File*);                       // 0x286B
    int32_t (*write)(int32_t fd, const void*, uint32_t); // 0x481F
    int32_t (*flsbuf)(int32_t ch, D2N104File*);          // 0x37DE
};

// 1.13c D2COMMON+0x02056
// __fwrite_lk(buf, size, count, file): returns items written.
uint32_t d2n104_fwrite_lk_2056(const D2N104FwriteEnv& e, const char* p, uint32_t size, uint32_t count, D2N104File* f)
{
    uint32_t total = size * count;
    if (total == 0) return 0;
    uint32_t left = total;
    uint32_t bs = (f->flag & 0x10c) == 0 ? 0x1000 : (uint32_t)f->bufsiz;
    do {
        uint32_t bits = f->flag & 0x108;
        uint32_t n;
        if (bits == 0 || f->cnt == 0) {
            if (bs <= left) {
                if (bits != 0 && e.flush(f) != 0) return (total - left) / size;
                uint32_t chunk = bs ? left - left % bs : left;
                n = (uint32_t)e.write(f->file, p, chunk);
                if (n == 0xffffffffu || (left -= n, n < chunk)) { f->flag |= 0x20; return (total - left) / size; }
            } else {
                if (e.flsbuf((int32_t)(int8_t)*p, f) == -1) return (total - left) / size;
                ++p; bs = (uint32_t)f->bufsiz; --left;
                if ((int32_t)bs < 1) bs = 1;
                continue;
            }
        } else {
            n = left < (uint32_t)f->cnt ? left : (uint32_t)f->cnt;
            std::memcpy(f->ptr, p, n);
            f->cnt -= (int32_t)n; f->ptr += n; left -= n;
        }
        p += n;
    } while (left != 0);
    return count;
}

// Host seams for the CRT bring-up chain.
struct D2N104CrtInitEnv {
    int32_t* osVer;          // DAT_6fdef5a4.. platform/major/minor/build slots (4 ints: plat, major, minor, build&0x7fff)
    int32_t (*getVersion)(int32_t out[4]);   // GetVersionExA: plat, major, minor, build; 0 on failure
    int32_t (*heapInit)(); int32_t (*mtinit)(); void (*rtcInit)(); void (*getCmdEnv)();
    int32_t (*ioinit)(); int32_t (*setargv)(); int32_t (*setenvp)(); int32_t (*cinit)(int32_t);
    void (*ioterm)(); void (*mtterm)(); void (*heapTerm)(); void (*cexit)(); void (*freeptd)(void*);
    int32_t (*initThread)(); // calloc ptd + FLS/TLS set + __initptd + tid; 1 ok
    int32_t* attachCount;    // DAT_6fdef5e8
    int32_t dontCexit;       // DAT_6fdef5e0
};

// 1.13c D2COMMON+0x02B0E
// __CRT_INIT@12(hinst, reason): 1 process attach, 0 detach, 2 thread attach, 3 thread detach.
int32_t d2n104_crt_init_2b0e(D2N104CrtInitEnv& e, int32_t reason)
{
    if (reason == 1) {
        int32_t v[4];
        if (!e.getVersion(v)) return 0;
        e.osVer[0] = v[0];
        if (!e.heapInit()) return 0;
        if (e.mtinit()) {
            e.rtcInit(); e.getCmdEnv();
            if (e.ioinit() >= 0) {
                if (e.setargv() >= 0 && e.setenvp() >= 0 && e.cinit(0) == 0) { ++*e.attachCount; return 1; }
                e.ioterm();
            }
            e.mtterm();
        }
        e.heapTerm();
        return 0;
    }
    if (reason == 0) {
        if (*e.attachCount > 0) {
            --*e.attachCount;
            if (e.dontCexit == 0) e.cexit();
            e.ioterm(); e.mtterm(); e.heapTerm();
            return 1;
        }
        return 0;
    }
    if (reason == 2) return e.initThread() ? 1 : 0;
    if (reason == 3) e.freeptd(nullptr);
    return 1;
}

// 1.13c D2COMMON+0x030A4
// __SEH_prolog: pushes the SEH frame (saved EBX/ESI/EDI/return) below the locals and links the chain head
// fs:0 to the new frame. The chain head is the host-owned slot.
void d2n104_seh_prolog_30a4(void** chainHead, void* frame) { *chainHead = frame; }

// 1.13c D2COMMON+0x030DF
// __SEH_epilog: restore the chain head fs:0 from frame[-4] (ebp-0x10).
void d2n104_seh_epilog_30df(void** chainHead, void** ebp) { *chainHead = ebp[-2]; }

struct D2N104FlsbufEnv {
    int32_t (*write)(int32_t fd, const void*, uint32_t);   // 0x481F
    int32_t (*lseek)(int32_t fd, int32_t off, int32_t whence); // 0x72C9
    int32_t (*isatty)(int32_t fd);                          // 0x73B8
    void (*getbuf)(D2N104File*);                            // 0x7374
    D2N104File* stdoutF; D2N104File* stderrF;               // 0x6FDE9328 / 0x6FDE9348
    uint8_t (*osflag)(int32_t fd);                          // _pioinfo[fd>>5][fd&31].osfile (+4)
};

// 1.13c D2COMMON+0x037DE
// __flsbuf(ch, file): put one byte into a full or unbuffered write stream; ch&0xFF on success, -1 on error.
int32_t d2n104_flsbuf_37de(const D2N104FlsbufEnv& e, int32_t ch, D2N104File* f)
{
    uint32_t fl = f->flag;
    int32_t fd = f->file;
    if ((fl & 0x82) == 0 || (fl & 0x40) != 0) { f->flag = fl | 0x20; return -1; }
    if (fl & 1) {
        f->cnt = 0;
        if ((fl & 0x10) == 0) { f->flag = fl | 0x20; return -1; }
        f->ptr = f->base;
        f->flag = fl & ~1u;
    }
    fl = f->flag;
    f->cnt = 0;
    int32_t wrote = 0, want;
    f->flag = (fl & ~0x10u) | 2;
    if ((fl & 0x10c) == 0 && ((f != e.stdoutF && f != e.stderrF) || e.isatty(fd) == 0)) e.getbuf(f);
    if ((f->flag & 0x108) == 0) {
        want = 1;
        uint8_t b = (uint8_t)ch;
        wrote = e.write(fd, &b, 1);
    } else {
        uint8_t* base = f->base;
        uint8_t* ptr = f->ptr;
        f->ptr = base + 1;
        want = (int32_t)(ptr - base);
        f->cnt = f->bufsiz - 1;
        if (want < 1) {
            if (fd != -1 && (e.osflag(fd) & 0x20) != 0) e.lseek(fd, 0, 2);  // fd -1 reads the shared dummy entry (no append bit)
        } else wrote = e.write(fd, base, (uint32_t)want);
        *f->base = (uint8_t)ch;
    }
    if (wrote == want) return ch & 0xff;
    f->flag |= 0x20;
    return -1;
}
