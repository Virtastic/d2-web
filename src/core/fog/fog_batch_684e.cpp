// Fog.dll MSVC 2003 CRT stdio/lowio rows 0x684E..0x786B, from build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane).
// Retail globals live in FogIoState; every callee/Win32 import is a seam in FogIoEnv.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct FogFile {                 // retail FILE, 0x20 bytes of fields (+ crit section at +0x20 for extra streams)
    char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname;
};
struct FogIoInfo {               // retail ioinfo, 0x24 bytes
    int32_t osfhnd; uint8_t osfile; uint8_t pipech; uint8_t pad[2]; int32_t lockinit; uint8_t cs[24];
};
struct FogIoState {
    FogIoInfo* pio[64];          // DAT_6ffa7140
    uint32_t   nhandle;          // DAT_6ffa7134
    FogFile**  streams;          // DAT_6ffa7240
    int32_t    nstreams;         // DAT_6ffa8260
    int32_t    stbuf_cnt;        // DAT_6ff803d8
    void*      stdbuf[2];        // DAT_6ff803dc
    uint32_t   commode;          // DAT_6ff805c4
    FogFile *iob_first, *iob_end, *std_out, *std_err; // static _iob range / stdout / stderr
};
struct FogIoEnv {
    void*   (*malloc)(uint32_t);
    void    (*free)(void*);
    void    (*lock)(int32_t);                 // __lock
    void    (*unlock)(int32_t);               // 0x3C3F
    int32_t (*mtinitlocknum)(int32_t);        // 0x3C54
    int32_t (*init_cs)(void*, uint32_t);      // ___crtInitCritSecAndSpinCount
    void    (*enter_cs)(void*);
    void    (*leave_cs)(void*);
    void    (*delete_cs)(void*);
    int32_t (*sopen)(const char*, uint32_t, int32_t, int32_t);
    int32_t (*isatty)(int32_t);
    intptr_t(*get_osfhandle)(uint32_t);
    void    (*free_osfhnd)(uint32_t);
    void    (*dosmaperr)(uint32_t);
    void    (*set_errno)(int32_t);
    void    (*set_doserrno)(uint32_t);
    void    (*lock_fhandle)(int32_t);
    void    (*unlock_fhandle)(int32_t);
    uint32_t(*set_file_pointer)(intptr_t, int32_t, uint32_t);
    int32_t (*close_handle)(intptr_t);
    uint32_t(*get_last_error)(void);
    void    (*get_startup_info)(const uint8_t** reserved2, uint32_t* cb);
    intptr_t(*get_std_handle)(uint32_t);
    uint32_t(*get_file_type)(intptr_t);
    void    (*set_handle_count)(uint32_t);
};

static FogIoInfo* io_of(const FogIoState* s, uint32_t fd) { return &s->pio[fd >> 5][fd & 0x1f]; }

// 1.13c FOG+0x684E __lock_file: static _iob entries share the lock table (slot 0x10 + index), others own a section.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_lock_file_684e(FogFile* f, const FogIoState* s, const FogIoEnv* e) {
    if (f > s->iob_first && f < s->iob_end) e->lock((int32_t)(f - s->iob_first) + 0x10);
    else e->enter_cs((uint8_t*)f + 0x20);
}

// 1.13c FOG+0x687D __lock_file2: first 20 streams use the lock table, later ones their own section.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_lock_file2_687d(int32_t idx, void* f, const FogIoEnv* e) {
    if (idx < 0x14) e->lock(idx + 0x10); else e->enter_cs((uint8_t*)f + 0x20);
}

// 1.13c FOG+0x68CF __unlock_file2: mirror of 0x687D.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_unlock_file2_68cf(int32_t idx, void* f, const FogIoEnv* e) {
    if (idx < 0x14) e->unlock(idx + 0x10); else e->leave_cs((uint8_t*)f + 0x20);
}

// 1.13c FOG+0x68F2 __openfile: parse fopen mode, open via __sopen (perm 0x1a4), init the stream; null on failure.
extern "C" EMSCRIPTEN_KEEPALIVE FogFile* fog_openfile_68f2(const char* name, const char* mode, int32_t shflag,
        FogFile* f, FogIoState* s, const FogIoEnv* e) {
    uint32_t of, sf;
    switch (*mode) {
        case 'a': of = 0x109; sf = s->commode | 2; break;
        case 'r': of = 0;     sf = s->commode | 1; break;
        case 'w': of = 0x301; sf = s->commode | 2; break;
        default: return nullptr;
    }
    bool ok = true, seen_rs = false, seen_cn = false;
    for (++mode; *mode && ok; ++mode) {
        switch (*mode) {
            case 'T': if (of & 0x1000) ok = false; else of |= 0x1000; break;
            case '+': if (of & 2) ok = false; else { of = (of & ~1u) | 2; sf = (sf & ~3u) | 0x80; } break;
            case 'D': if (of & 0x40) ok = false; else of |= 0x40; break;
            case 'R': if (seen_rs) ok = false; else { seen_rs = true; of |= 0x10; } break;
            case 'S': if (seen_rs) ok = false; else { seen_rs = true; of |= 0x20; } break;
            case 'b': if (of & 0xc000) ok = false; else of |= 0x8000; break;
            case 't': if (of & 0xc000) ok = false; else of |= 0x4000; break;
            case 'c': if (seen_cn) ok = false; else { seen_cn = true; sf |= 0x4000; } break;
            case 'n': if (seen_cn) ok = false; else { seen_cn = true; sf &= ~0x4000u; } break;
            default: ok = false;
        }
    }
    int32_t fd = e->sopen(name, of, shflag, 0x1a4);
    if (fd < 0) return nullptr;
    s->stbuf_cnt++;
    f->flag = sf; f->cnt = 0; f->ptr = nullptr; f->base = nullptr; f->tmpfname = nullptr; f->file = fd;
    return f;
}

// 1.13c FOG+0x6ADF __getstream: find or create a free stream slot (returned locked), reset it. Holds lock 1 meanwhile.
extern "C" EMSCRIPTEN_KEEPALIVE FogFile* fog_getstream_6adf(FogIoState* s, const FogIoEnv* e) {
    FogFile* f = nullptr;
    e->lock(1);
    for (int32_t i = 0; i < s->nstreams; i++) {
        FogFile* cur = s->streams[i];
        if (!cur) {
            cur = (FogFile*)e->malloc(0x38);
            s->streams[i] = cur;
            if (cur) {
                if (!e->init_cs((uint8_t*)cur + 0x20, 4000)) { e->free(cur); s->streams[i] = nullptr; }
                else { e->enter_cs((uint8_t*)cur + 0x20); f = cur; }
            }
            break;
        }
        if ((cur->flag & 0x83) == 0) {
            if (i > 2 && i < 0x14 && !e->mtinitlocknum(i + 0x10)) break;
            fog_lock_file2_687d(i, cur, e);
            f = s->streams[i];
            if ((f->flag & 0x83) == 0) break;
            fog_unlock_file2_68cf(i, f, e);   // retail calls the 0x68CF twin here; slot was taken meanwhile
            f = nullptr;
        }
    }
    if (f) { f->cnt = 0; f->flag = 0; f->base = nullptr; f->ptr = nullptr; f->tmpfname = nullptr; f->file = -1; }
    e->unlock(1);   // FUN_6ff56bf8
    return f;
}

// 1.13c FOG+0x6F6A __stbuf: give an unbuffered tty stdout/stderr a temp 4 KiB (else 2-byte) buffer; 1 if set.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_stbuf_6f6a(FogFile* f, FogIoState* s, const FogIoEnv* e) {
    if (!e->isatty(f->file)) return 0;
    int32_t i;
    if (f == s->std_out) i = 0; else if (f == s->std_err) i = 1; else return 0;
    s->stbuf_cnt++;
    if (f->flag & 0x10c) return 0;
    if (!s->stdbuf[i]) {
        s->stdbuf[i] = e->malloc(0x1000);
        if (!s->stdbuf[i]) {
            f->base = f->ptr = (char*)&f->charbuf; f->bufsiz = f->cnt = 2;
            f->flag |= 0x1102; return 1;
        }
    }
    f->base = f->ptr = (char*)s->stdbuf[i]; f->bufsiz = f->cnt = 0x1000;
    f->flag |= 0x1102;
    return 1;
}

static bool io_init_table(FogIoInfo* t) {
    for (int i = 0; i < 32; i++) { t[i].osfhnd = -1; t[i].lockinit = 0; t[i].osfile = 0; t[i].pipech = 10; }
    return true;
}

// 1.13c FOG+0x73EC __ioinit: build the handle table, adopt inherited handles from STARTUPINFO, wire std 0..2. 0 / -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_ioinit_73ec(FogIoState* s, const FogIoEnv* e) {
    FogIoInfo* t = (FogIoInfo*)e->malloc(0x480);
    if (!t) return -1;
    s->nhandle = 0x20; s->pio[0] = t; io_init_table(t);
    const uint8_t* res; uint32_t cb; e->get_startup_info(&res, &cb);
    if (cb && res) {
        int32_t n; memcpy(&n, res, 4);
        const uint8_t* flags = res + 4; const uint8_t* hnds = flags + n;
        if (n > 0x7ff) n = 0x800;
        int32_t cnt = n;
        for (int k = 1; (int32_t)s->nhandle < n; k++) {
            FogIoInfo* nt = (FogIoInfo*)e->malloc(0x480);
            if (!nt) { cnt = (int32_t)s->nhandle; break; }
            s->nhandle += 0x20; s->pio[k] = nt; io_init_table(nt);
        }
        for (int32_t i = 0; i < cnt; i++, hnds += 4, flags++) {
            int32_t h; memcpy(&h, hnds, 4);
            if (h != -1 && (*flags & 1) && ((*flags & 8) || e->get_file_type(h) != 0)) {
                FogIoInfo* io = io_of(s, i);
                io->osfhnd = h; io->osfile = *flags;
                if (!e->init_cs(io->cs, 4000)) return -1;
                io->lockinit++;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        FogIoInfo* io = &s->pio[0][i];
        if (io->osfhnd == -1) {
            io->osfile = 0x81;
            intptr_t h = e->get_std_handle(i == 0 ? 0xfffffff6u : (i == 1 ? 0xfffffff5u : 0xfffffff4u));
            uint32_t ty;
            if (h == -1 || (ty = e->get_file_type(h)) == 0) io->osfile |= 0x40;
            else {
                io->osfhnd = (int32_t)h;
                if ((ty & 0xff) == 2) io->osfile |= 0x40; else if ((ty & 0xff) == 3) io->osfile |= 8;
                if (!e->init_cs(io->cs, 4000)) return -1;
                io->lockinit++;
            }
        } else io->osfile |= 0x80;
    }
    e->set_handle_count(s->nhandle);
    return 0;
}

// 1.13c FOG+0x75EA __ioterm: delete every initialised handle lock and free the 64 table blocks.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_ioterm_75ea(FogIoState* s, const FogIoEnv* e) {
    for (int k = 0; k < 64; k++) {
        if (!s->pio[k]) continue;
        for (int i = 0; i < 32; i++) if (s->pio[k][i].lockinit) e->delete_cs(s->pio[k][i].cs);
        e->free(s->pio[k]); s->pio[k] = nullptr;
    }
}

// 1.13c FOG+0x7636 __lseek_lk: SetFilePointer on the OS handle; clears osfile bit 2 (EOF) on success. -1 on error.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_lseek_lk_7636(uint32_t fd, int32_t off, uint32_t origin,
        const FogIoState* s, const FogIoEnv* e) {
    intptr_t h = e->get_osfhandle(fd);
    if (h == -1) { e->set_errno(9); return 0xffffffffu; }
    uint32_t r = e->set_file_pointer(h, off, origin);
    uint32_t err = r == 0xffffffffu ? e->get_last_error() : 0;
    if (!err) { io_of(s, fd)->osfile &= 0xfd; return r; }
    e->dosmaperr(err);
    return 0xffffffffu;
}

// 1.13c FOG+0x76AA __lseek: validate fd (EBADF 9), take the handle lock, seek, unlock.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_lseek_76aa(int32_t fd, int32_t off, int32_t origin,
        const FogIoState* s, const FogIoEnv* e) {
    if ((uint32_t)fd < s->nhandle && (io_of(s, fd)->osfile & 1)) {
        e->lock_fhandle(fd);
        int32_t r;
        if (!(io_of(s, fd)->osfile & 1)) { e->set_errno(9); e->set_doserrno(0); r = -1; }
        else r = (int32_t)fog_lseek_lk_7636(fd, off, origin, s, e);
        e->unlock_fhandle(fd);
        return r;
    }
    e->set_errno(9); e->set_doserrno(0);
    return -1;
}

// 1.13c FOG+0x77E8 __close_lk: close the OS handle (skipped when stdout/stderr share one), free the slot. 0 / -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_close_lk_77e8(uint32_t fd, const FogIoState* s, const FogIoEnv* e) {
    uint32_t err = 0;
    if (e->get_osfhandle(fd) != -1) {
        bool shared = (fd == 1 || fd == 2) && e->get_osfhandle(1) == e->get_osfhandle(2);
        if (!shared && !e->close_handle(e->get_osfhandle(fd))) err = e->get_last_error();
    }
    e->free_osfhnd(fd);
    io_of(s, fd)->osfile = 0;
    if (!err) return 0;
    e->dosmaperr(err);
    return -1;
}

// 1.13c FOG+0x786B __close: validate fd (EBADF 9), lock, close, unlock.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_close_786b(int32_t fd, const FogIoState* s, const FogIoEnv* e) {
    if ((uint32_t)fd < s->nhandle && (io_of(s, fd)->osfile & 1)) {
        e->lock_fhandle(fd);
        int32_t r;
        if (!(io_of(s, fd)->osfile & 1)) { e->set_errno(9); r = -1; }
        else r = fog_close_lk_77e8(fd, s, e);
        e->unlock_fhandle(fd);
        return r;
    }
    e->set_errno(9); e->set_doserrno(0);
    return -1;
}
