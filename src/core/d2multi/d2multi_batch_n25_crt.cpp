// D2Multi batch n25: MSVCRT small-block heap (6BBF 6C76 6D7C 705B) and low-level io (7372 746B 752B 75ED 770B 787D 7A41 7AC4).
// Host seams (Win32 / CRT calls) come in as a Host struct. Region and block links are 32-bit addresses as in retail.
// ponytail: sbh link fields assume the wasm heap stays below 4 GiB; widen the 32-bit slots if that ever breaks.
#include <cstdint>
#include <cstddef>

struct Crt32File { char* ptr; int32_t cnt; char* base; int32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };

struct D2MultiCrtHost {
    uint32_t (*heap_alloc)(uint32_t size);                 // HeapAlloc(heap, HEAP_ZERO_MEMORY, size)
    uint32_t (*heap_realloc)(uint32_t p, uint32_t size);   // HeapReAlloc(heap, 0, p, size)
    void     (*heap_free)(uint32_t p);                     // HeapFree(heap, 0, p)
    uint32_t (*virt_reserve)(uint32_t size);               // VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE)
    uint32_t (*virt_commit)(uint32_t addr, uint32_t size); // VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE)
    int32_t* errno_p;
    uint32_t* doserrno_p;
    intptr_t (*get_osfhandle)(uint32_t fh);
    void     (*set_std_handle)(uint32_t which, intptr_t h);
    int32_t  (*close_handle)(intptr_t h);
    uint32_t (*get_last_error)(void);
    void     (*dosmaperr)(uint32_t err);
    uint32_t (*set_file_pointer)(intptr_t h, int32_t lo, int32_t* hi, uint32_t method);
    void     (*lock)(int32_t n);
    void     (*unlock)(int32_t n);
    int32_t  (*init_critsec)(void* cs, uint32_t spin);
    void     (*enter_critsec)(void* cs);
    void     (*unlock_fhandle)(void);                      // 7B3B
    int32_t  (*write)(int32_t fd, const void* buf, uint32_t n); // 4918
    void     (*freebuf)(Crt32File* f);                     // 7B5F
    void     (*free)(void* p);
    // user32, resolved lazily by retail; null = absent
    int32_t  (*MessageBoxA)(int32_t, const char*, const char*, uint32_t);
    int32_t  (*GetActiveWindow)(void);
    int32_t  (*GetLastActivePopup)(int32_t);
    intptr_t (*GetProcessWindowStation)(void);
    int32_t  (*GetUserObjectInformationA)(intptr_t, int32_t, void*, uint32_t, void*);
    uint32_t os_platform;  // DAT_6f9eba84
    uint32_t os_major;     // DAT_6f9eba90
};

extern "C" {
extern uint32_t DAT_6fa0a110, DAT_6fa0a114, DAT_6fa0a11c, DAT_6fa0a120, DAT_6fa0a10c, DAT_6fa0a124;
extern uint32_t DAT_6fa0b38c;
extern uint8_t* DAT_6fa0b3a0[];
extern int32_t DAT_6f9ebad8;
}

static inline uint32_t& W(uint32_t a) { return *(uint32_t*)(uintptr_t)a; }
static inline uint8_t& B(uint32_t a) { return *(uint8_t*)(uintptr_t)a; }

// clear / set free-list bit for size class idx in region r, group grp (header hdr)
static void sbh_clr(uint32_t r, uint32_t hdr, uint32_t grp, uint32_t idx) {
    if (idx < 0x20) {
        uint32_t m = ~(0x80000000u >> (idx & 31));
        W(hdr + 0x44 + grp * 4) &= m;
        if (--B(hdr + idx + 4) == 0) W(r) &= m;
    } else {
        uint32_t m = ~(0x80000000u >> ((idx - 0x20) & 31));
        W(hdr + 0xc4 + grp * 4) &= m;
        if (--B(hdr + idx + 4) == 0) W(r + 4) &= m;
    }
}
static void sbh_set(uint32_t r, uint32_t hdr, uint32_t grp, uint32_t idx) {
    uint8_t c = B(hdr + idx + 4)++;
    if (idx < 0x20) {
        if (!c) W(r) |= 0x80000000u >> (idx & 31);
        W(hdr + 0x44 + grp * 4) |= 0x80000000u >> (idx & 31);
    } else {
        uint32_t b = 0x80000000u >> ((idx - 0x20) & 31);
        if (!c) W(r + 4) |= b;
        W(hdr + 0xc4 + grp * 4) |= b;
    }
}

// 1.13c D2Multi+0x06BBF
uint32_t ___sbh_alloc_new_region(const D2MultiCrtHost* h) {
    if (DAT_6fa0a110 == DAT_6fa0a120) {
        uint32_t p = h->heap_realloc(DAT_6fa0a114, (DAT_6fa0a120 * 5 + 0x50) * 4);
        if (!p) return 0;
        DAT_6fa0a120 += 0x10;
        DAT_6fa0a114 = p;
    }
    uint32_t r = DAT_6fa0a114 + DAT_6fa0a110 * 0x14;
    uint32_t v = h->heap_alloc(0x41c4);
    W(r + 16) = v;
    if (v) {
        uint32_t c = h->virt_reserve(0x100000);
        W(r + 12) = c;
        if (c) {
            W(r + 8) = 0xffffffff; W(r) = 0; W(r + 4) = 0;
            DAT_6fa0a110++;
            W(W(r + 16)) = 0xffffffff;
            return r;
        }
        h->heap_free(W(r + 16));
    }
    return 0;
}

// 1.13c D2Multi+0x06C76
int32_t ___sbh_alloc_new_group(const D2MultiCrtHost* h, uint32_t reg) {
    uint32_t hdr = W(reg + 0x10);
    int32_t g = 0;
    for (int32_t m = (int32_t)W(reg + 8); m > -1; m = (int32_t)((uint32_t)m << 1)) g++;
    uint32_t list = g * 0x204 + 0x144 + hdr;
    uint32_t a = list;
    for (int n = 0x3f; n; n--, a += 8) { W(a + 8) = a; W(a + 4) = a; }
    uint32_t lp = g * 0x8000 + W(reg + 0xc);
    if (!h->virt_commit(lp, 0x8000)) return -1;
    uint32_t q = lp + 0x10;
    for (int n = 8; n; n--, q += 0x1000) {
        W(q - 8) = 0xffffffff;
        W(q + 0x3fb * 4) = 0xffffffff;
        W(q) = q + 0x3ff * 4;
        W(q - 4) = 0xff0;
        W(q + 4) = q - 0x401 * 4;
        W(q + 0x3fa * 4) = 0xff0;
    }
    W(list + 0x1fc) = lp + 0xc;
    W(lp + 0x14) = list + 0x1f8;
    W(list + 0x200) = lp + 0x700c;
    W(lp + 0x7010) = list + 0x1f8;
    W(hdr + 0x44 + g * 4) = 0;
    W(hdr + 0xc4 + g * 4) = 1;
    uint8_t c = B(hdr + 0x43)++;
    if (c == 0) W(reg + 4) |= 1;
    W(reg + 8) &= ~(0x80000000u >> (g & 31));
    return g;
}

// 1.13c D2Multi+0x06D7C
uint32_t ___sbh_resize_block(uint32_t reg, uint32_t blk, int32_t size) {
    uint32_t hdr = W(reg + 0x10);
    uint32_t grp = (blk - W(reg + 0xc)) >> 15;
    uint32_t list = grp * 0x204 + 0x144 + hdr;
    uint32_t want = (size + 0x17u) & 0xfffffff0u;
    int32_t have = (int32_t)W(blk - 4) - 1;
    uint32_t nx = W(blk - 4) - 5 + blk;
    uint32_t nsz = W(nx);
    if (have < (int32_t)want) {
        if ((nsz & 1) || (int32_t)(nsz + have) < (int32_t)want) return 0;
        uint32_t cls = ((int32_t)nsz >> 4) - 1;
        if (cls > 0x3f) cls = 0x3f;
        if (W(nx + 4) == W(nx + 8)) sbh_clr(reg, hdr, grp, cls);
        W(W(nx + 8) + 4) = W(nx + 4);
        W(W(nx + 4) + 8) = W(nx + 8);
        int32_t rest = nsz + (have - want);
        if (rest > 0) {
            uint32_t c = (rest >> 4) - 1;
            uint32_t at = blk - 4 + want;
            if (c > 0x3f) c = 0x3f;
            uint32_t head = list + c * 8;
            W(at + 4) = W(head + 4);
            W(at + 8) = head;
            W(head + 4) = at;
            W(W(at + 4) + 8) = at;
            if (W(at + 4) == W(at + 8)) sbh_set(reg, hdr, grp, c);
            W(at) = rest;
            W(rest - 4 + at) = rest;
        }
        W(blk - 4) = want + 1;
        W(blk - 8 + want) = want + 1;
    } else if ((int32_t)want < have) {
        int32_t rest = have - want;
        W(blk - 4) = want + 1;
        uint32_t at = blk - 4 + want;
        W(at - 4) = want + 1;
        uint32_t c = (rest >> 4) - 1;
        if (c > 0x3f) c = 0x3f;
        if ((nsz & 1) == 0) {
            uint32_t nc = ((int32_t)nsz >> 4) - 1;
            if (nc > 0x3f) nc = 0x3f;
            if (W(nx + 4) == W(nx + 8)) sbh_clr(reg, hdr, grp, nc);
            W(W(nx + 8) + 4) = W(nx + 4);
            W(W(nx + 4) + 8) = W(nx + 8);
            rest += nsz;
            c = (rest >> 4) - 1;
            if (c > 0x3f) c = 0x3f;
        }
        uint32_t head = list + c * 8;
        uint32_t prev = W(head + 4);
        W(at + 8) = head;
        W(at + 4) = prev;
        W(head + 4) = at;
        W(W(at + 4) + 8) = at;
        if (W(at + 4) == W(at + 8)) sbh_set(reg, hdr, grp, c);
        W(at) = rest;
        W(rest - 4 + at) = rest;
    }
    return 1;
}

// 1.13c D2Multi+0x0705B
uint32_t ___sbh_alloc_block(const D2MultiCrtHost* h, uint32_t size) {
    uint32_t u7 = (size + 0x17) & 0xfffffff0u;
    int32_t i8 = ((int32_t)(size + 0x17) >> 4) - 1;
    uint32_t end = DAT_6fa0a114 + DAT_6fa0a110 * 0x14;
    uint32_t r = DAT_6fa0a11c, u14, lc;
    if (i8 < 0x20) { u14 = 0xffffffffu >> (i8 & 31); lc = 0xffffffffu; }
    else { u14 = 0; lc = 0xffffffffu >> ((i8 - 0x20) & 31); }
    auto none = [&](uint32_t x) { return (W(x + 4) & lc) == 0 && (W(x) & u14) == 0; };
    while (r < end && none(r)) r += 0x14;
    if (r == end) {
        uint32_t p = DAT_6fa0a114;
        while (p < DAT_6fa0a11c && none(p)) p += 0x14;
        r = p;
        if (p == DAT_6fa0a11c) {
            while (p < end && W(p + 8) == 0) p += 0x14;
            uint32_t p13 = DAT_6fa0a114;
            r = p;
            if (p == end) {
                while (p13 < DAT_6fa0a11c && W(p13 + 8) == 0) p13 += 0x14;
                r = p13;
                if (p13 == DAT_6fa0a11c) { r = ___sbh_alloc_new_region(h); if (!r) return 0; }
            }
            W(W(r + 16)) = (uint32_t)___sbh_alloc_new_group(h, r);
            if ((int32_t)W(W(r + 16)) == -1) return 0;
        }
    }
    uint32_t hdr = W(r + 16);
    int32_t g = (int32_t)W(hdr);
    if (g == -1 || ((W(hdr + (g + 0x31) * 4) & lc) == 0 && (W(hdr + (g + 0x11) * 4) & u14) == 0)) {
        g = 0;
        uint32_t a = hdr + 0x44;
        if ((W(hdr + 0xc4) & lc) == 0 && (W(a) & u14) == 0) {
            uint32_t p12;
            do { p12 = a + 0x84; g++; a += 4; } while ((W(p12) & lc) == 0 && (W(a) & u14) == 0);
        }
    }
    uint32_t list = hdr + (g * 0x81 + 0x51) * 4;
    i8 = 0;
    u14 = W(hdr + (g + 0x11) * 4) & u14;
    if (u14 == 0) { u14 = W(hdr + (g + 0x31) * 4) & lc; i8 = 0x20; }
    for (; (int32_t)u14 >= 0; u14 <<= 1) i8++;
    uint32_t blk = W(list + (i8 * 2 + 1) * 4);
    int32_t rem = (int32_t)W(blk) - (int32_t)u7;
    int32_t c = (rem >> 4) - 1;
    if (c > 0x3f) c = 0x3f;
    DAT_6fa0a11c = r;
    if (c != i8) {
        if (W(blk + 4) == W(blk + 8)) sbh_clr(r, hdr, g, i8);
        W(W(blk + 8) + 4) = W(blk + 4);
        W(W(blk + 4) + 8) = W(blk + 8);
        if (rem != 0) {
            uint32_t head = list + c * 8;
            W(blk + 8) = head;
            W(blk + 4) = W(head + 4);
            W(head + 4) = blk;
            W(W(blk + 4) + 8) = blk;
            if (W(blk + 4) == W(blk + 8)) sbh_set(r, hdr, g, c);
        }
    }
    if (rem != 0) { W(blk) = rem; W(rem - 4 + blk) = rem; }
    blk += rem;
    W(blk) = u7 + 1;
    W(blk + u7 - 4) = u7 + 1;
    uint32_t used = W(list)++;
    if (used == 0 && r == DAT_6fa0a10c && (uint32_t)g == DAT_6fa0a124) DAT_6fa0a10c = 0;
    W(hdr) = g;
    return blk + 4;
}

// 1.13c D2Multi+0x07372
int32_t ___crtMessageBoxA(const D2MultiCrtHost* h, const char* text, const char* caption, uint32_t type) {
    int32_t owner = 0;
    if (!h->MessageBoxA) return 0;
    uint8_t info[12]; uint8_t len[4];
    intptr_t ws;
    if (!h->GetProcessWindowStation ||
        (((ws = h->GetProcessWindowStation()) != 0 &&
          h->GetUserObjectInformationA(ws, 1, info, 0xc, len) != 0) && (info[8] & 1))) {
        if (h->GetActiveWindow && (owner = h->GetActiveWindow()) != 0 && h->GetLastActivePopup)
            owner = h->GetLastActivePopup(owner);
    } else if (h->os_major < 4) type |= 0x40000;
    else type |= 0x200000;
    return h->MessageBoxA(owner, text, caption, type);
}

static uint8_t* fh_entry(uint32_t fh) { return DAT_6fa0b3a0[fh >> 5] + (fh & 0x1f) * 0x24; }

// 1.13c D2Multi+0x0746B
int32_t sub_746B(const D2MultiCrtHost* h, uint32_t fh) {
    if (fh < DAT_6fa0b38c) {
        uint8_t* e = fh_entry(fh);
        if ((e[4] & 1) && *(int32_t*)e != -1) {
            if (DAT_6f9ebad8 == 1) {
                if (fh == 0) h->set_std_handle(0xfffffff6, 0);
                else if (fh == 1) h->set_std_handle(0xfffffff5, 0);
                else if (fh == 2) h->set_std_handle(0xfffffff4, 0);
            }
            *(int32_t*)e = -1;
            return 0;
        }
    }
    *h->errno_p = 9;
    *h->doserrno_p = 0;
    return -1;
}

// 1.13c D2Multi+0x0752B
int32_t __lock_fhandle(const D2MultiCrtHost* h, int32_t fh) {
    uint8_t* e = fh_entry(fh);
    if (*(int32_t*)(e + 8) == 0) {
        h->lock(10);
        if (*(int32_t*)(e + 8) == 0) {
            if (h->init_critsec(e + 0xc, 4000) == 0) { h->unlock(10); return 0; }
            (*(int32_t*)(e + 8))++;
        }
        h->unlock(10);
    }
    h->enter_critsec(fh_entry(fh) + 0xc);
    return 1;
}

// 1.13c D2Multi+0x075ED
uint64_t __lseeki64_lk(const D2MultiCrtHost* h, uint32_t fh, int32_t lo, int32_t hi, uint32_t whence) {
    int32_t hi_out = hi;
    uint32_t r;
    intptr_t os = h->get_osfhandle(fh);
    if (os == -1) {
        *h->errno_p = 9;
        r = 0xffffffff; hi_out = -1;
    } else {
        r = h->set_file_pointer(os, lo, &hi_out, whence);
        if (r == 0xffffffff) {
            uint32_t e = h->get_last_error();
            if (e != 0) { h->dosmaperr(e); r = 0xffffffff; hi_out = -1; return ((uint64_t)(uint32_t)hi_out << 32) | r; }
        }
        fh_entry(fh)[4] &= 0xfd;
    }
    return ((uint64_t)(uint32_t)hi_out << 32) | r;
}

// 1.13c D2Multi+0x0770B
int32_t sub_770B(const D2MultiCrtHost* h, Crt32File* f) {
    int32_t r = 0;
    if ((f->flag & 3) == 2 && (f->flag & 0x108) != 0) {
        int32_t n = (int32_t)(f->ptr - f->base);
        if (n > 0) {
            if (h->write(f->file, f->base, n) == n) {
                if ((int8_t)f->flag < 0) f->flag &= ~2;
            } else { f->flag |= 0x20; r = -1; }
        }
    }
    f->cnt = 0;
    f->ptr = f->base;
    return r;
}

int32_t sub_7AC4(const D2MultiCrtHost* h, uint32_t fh);

// 1.13c D2Multi+0x0787D
int32_t sub_787D(const D2MultiCrtHost* h, Crt32File* f) {
    int32_t r = -1;
    if (f->flag & 0x83) {
        r = sub_770B(h, f);
        h->freebuf(f);
        if (sub_7AC4(h, f->file) < 0) r = -1;
        else if (f->tmpfname) { h->free(f->tmpfname); f->tmpfname = nullptr; }
    }
    f->flag = 0;
    return r;
}

// 1.13c D2Multi+0x07A41
int32_t sub_7A41(const D2MultiCrtHost* h, uint32_t fh) {
    uint32_t err = 0;
    if (h->get_osfhandle(fh) != -1) {
        bool skip = (fh == 1 || fh == 2) && h->get_osfhandle(2) == h->get_osfhandle(1);
        if (!skip && !h->close_handle(h->get_osfhandle(fh))) err = h->get_last_error();
    }
    sub_746B(h, fh);
    fh_entry(fh)[4] = 0;
    if (err == 0) return 0;
    h->dosmaperr(err);
    return -1;
}

// 1.13c D2Multi+0x07AC4
int32_t sub_7AC4(const D2MultiCrtHost* h, uint32_t fh) {
    if (fh < DAT_6fa0b38c && (fh_entry(fh)[4] & 1)) {
        int32_t r;
        __lock_fhandle(h, fh);
        if ((fh_entry(fh)[4] & 1) == 0) { *h->errno_p = 9; r = -1; }
        else r = sub_7A41(h, fh);
        h->unlock_fhandle();
        return r;
    }
    *h->errno_p = 9;
    *h->doserrno_p = 0;
    return -1;
}
