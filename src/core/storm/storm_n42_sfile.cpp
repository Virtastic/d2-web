// n42 batch: Storm SFile/SDraw entry points. Win32 and Storm-internal callees are host seams (H), globals are G.
#include <cstdint>
#include <cstring>
typedef uint32_t u32;
struct N42Host {
    void (*lock)(); void (*unlock)(); void (*setErr)(u32);
    void* (*alloc)(u32 n, const char* file, u32 line, u32 flags);
    void (*free_)(void* p, const char* file, u32 line);
    void (*strcpyN)(char* d, const char* s, u32 n); void (*strcatN)(char* d, const char* s, u32 n);
    u32 (*strlen_)(const char* s); const char* (*strstr_)(const char* s, const char* t); const char* (*strchr_)(const char* s, char c);
    u32 (*getAttr)(const char* p); void* (*createFile)(const char* p, u32 acc, u32 share, u32 disp, u32 flags);
    u32 (*setFilePtr)(void* h, int32_t off, u32 mode); u32 (*readFile)(void* h, void* buf, u32 n);
    void (*initArchives)(); void (*buildPath)(char* out, int mode); int (*findArchiveList)();
    void* (*newFile)(void* list, int a, int b); void (*delFile)(void* f); void (*destroyFile)(void* f);
    void (*hashInit)(void* tbl, void* mem, u32 n, void* name); u32 (*cryptSeed)(u32); void (*decrypt)(u32 key, void* d);
    void (*setCount)(u32); int (*findBlock)(void* arch, u32 hash); void (*postOpen)(void* f, void* tbl, int* cnt);
    void (*afterOpen)(); void (*regArchive)(); int (*fileOpenEx)(u32,u32*,u32,int*,int,int,int);
    const char* (*lastSep)(const char*, char);
    void (*drawLock)(); void (*palSnap)(); int (*lockSurf)(int,void*,int*,void**,int); void (*unlockSurf)(int,int);
    void (*blit)(void*,void*,int,u32,u32,u32,void*,int,int,u32); int (*savePcx)(const char*,void*,void*,u32,u32,u32);
    int (*saveDlg)(char* out260, void* hwnd); void (*surfSnap)();
};
struct N42G {
    N42Host* h; int fileList; int archList; int archHead; u32 archFlags; u32 openMode; char dir[0x104];
    int cbA, cbB; u32 lastErr; void* a0; void* a1; int sd_ok, sd_hwnd, sd_pal, sd_bpp; u32 sd_w, sd_h; void* sd_dd; u32 sd_palBuf[1];
    int archListHead; u32 lastError2;
};
static N42G G;

// 1.13c Storm+0x28DA0
void Storm_28DA0(N42G*, const char* name, void** out, int (*f)(void*, const char*, uint8_t, void**), void* arch) {
    uint8_t m = (G.archFlags & 1) != 0;
    if (G.archFlags & 2) m |= 2;
    if (G.archHead < 1 && G.archFlags == 0) m |= 1;
    f(arch, name, m, out);
}

// 1.13c Storm+0x29BE0
void Storm_29BE0(u32 a, u32* b, u32 c, int* d, int e, int (*f)(u32, u32*, u32, int*, int, int, int)) { f(a, b, c, d, e, 0, 0); }

// 1.13c Storm+0x26190
u32 Storm_26190(const char* path) {
    N42Host* h = G.h;
    if (!path) { G.lastErr = 0x57; h->setErr(0x57); return 0; }
    if (!*path) { G.dir[0] = 0; return 1; }
    u32 n = h->strlen_(path);
    char last = path[n - 1];
    if (0x104 < n + (last != '\\') + 1) { G.lastErr = 0xa1; h->setErr(0xa1); return 0; }
    h->lock();
    h->strcpyN(G.dir, path, 0x104);
    if (last != '\\') h->strcatN(G.dir, "\\", 0x104);
    h->unlock();
    return 1;
}

// 1.13c Storm+0x26030
u32 Storm_26030(uint8_t* f, int32_t off, int* hi, u32 mode) {
    N42Host* h = G.h;
    if (hi && *hi) { G.lastErr = 0x57; h->setErr(0x57); return 0xffffffff; }
    h->lock();
    void* fh = *(void**)(f + 0x10c);
    if ((intptr_t)fh != -1) { h->unlock(); return h->setFilePtr(fh, off, mode); }
    u32* pos = (u32*)(f + 0x11c);
    u32 size = *(u32*)(*(uint8_t**)(f + 0x114) + 8);
    if (mode != 0) {
        if (mode == 1) {
            if (off < 0 && *pos < (u32)-off) *pos = 0; else *pos += off;
            goto clamp;
        }
        if (mode != 2) goto clamp;
        if (off < 0 && size < (u32)-off) { *pos = 0; goto clamp; }
        off = size + off;
    }
    *pos = off;
clamp:
    u32 lim = size - 1, v = *pos;
    if (lim <= v) v = lim;
    *pos = v;
    *(u32*)(f + 0x138) = 0; *(u32*)(f + 0x13c) = 0;
    h->unlock();
    return v;
}

// 1.13c Storm+0x25D30
u32 Storm_25D30(u32 a, u32 b) { G.cbA = (int)a; G.cbB = (int)b; return 1; }

// 1.13c Storm+0x26330
u32 Storm_26330(uint8_t* f, char* out, int n) {
    N42Host* h = G.h;
    if (out) { *out = 0; h->lock(); h->strcpyN(out, (char*)(f + 8), n); h->unlock(); return 1; }
    G.lastErr = 0x57; h->setErr(0x57); return 0;
}

// 1.13c Storm+0x29D90
void Storm_29D90(void* a, int* b, u32* c, u32 d, int e, void (*f)(void*, void*, int*, u32*, u32, u32, int, int)) {
    u32 m = (G.archFlags & 1) != 0;
    if (G.archFlags & 2) m |= 2;
    if (G.archHead < 1 && G.archFlags == 0) m |= 1;
    f(nullptr, a, b, c, d, m, e, 0);
}

// 1.13c Storm+0x26C40
u32 Storm_26C40(void* p) {
    N42Host* h = G.h;
    if (!p) { G.lastErr = 0x57; h->setErr(0x57); return 0; }
    h->free_(p, "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP", 0x1366);
    return 1;
}

// 1.13c Storm+0x38EB0
u32 Storm_38EB0(void* p, void (*lk)(), void (*ulk)(), void (*dtor)(void*)) {
    N42Host* h = G.h;
    if (!p) { G.lastErr = 0x57; h->setErr(0x57); return 0; }
    lk(); dtor(p);
    h->free_(p, "~AUSTREAM", -2);
    ulk();
    return 1;
}

// 1.13c Storm+0x28960
bool Storm_28960(void* arch, const char* name, uint8_t flags, u32** out) {
    N42Host* h = G.h; const char* SF = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";
    char path[260]; bool ok = false;
    if (!out || !name || !*name) { *(out ? out : (u32**)&G.a0) = nullptr; G.lastErr = 0x57; h->setErr(0x57); return false; }
    *out = nullptr;
    h->lock();
    if (G.archList == 0) h->initArchives();
    if (flags & 3) {
        bool tryDisk = false;
        if (flags & 2) { h->buildPath(path, 1); if ((~(h->getAttr(path) >> 4) & 1) == 0) tryDisk = true; else goto open_disk; }
        else tryDisk = true;
        if (tryDisk && (flags & 1)) {
            if (*name == '\\' || h->strstr_(name, ":\\") || h->strstr_(name, "\\\\")) h->strcpyN(path, name, 0x104);
            else h->buildPath(path, 0);
            if ((~(h->getAttr(path) >> 4) & 1) != 0) {
            open_disk:
                void* fh = h->createFile(path, 0x80000000, 1, 3, 0x8000000);
                if ((intptr_t)fh != -1) {
                    if (!(flags & 4)) {
                        uint32_t* f = (uint32_t*)h->newFile(&G.fileList, 0, 0);
                        h->strcpyN((char*)(f + 2), name, 0x104);
                        f[0x43] = (u32)(uintptr_t)fh; f[0x50] = 1; f[0x52] = 0; f[0x53] = 1;
                        *out = f;
                    } else *out = (u32*)fh;
                    ok = true; goto done;
                }
            }
        }
    }
    {
        char* a = (char*)arch; int idx = -1; bool found = false;
        if (!a) {
            a = (char*)(intptr_t)G.archHead;
            if ((int)(intptr_t)a > 0 && G.archList) {
                for (;;) {
                    idx = h->findArchiveList();
                    if (idx != -1) { found = true; break; }
                    a = *(char**)(a + 4);
                    if ((int)(intptr_t)a < 1 || !a) break;
                }
            }
        } else { idx = h->findArchiveList(); found = idx != -1; }
        if (found) {
            uint8_t* bt = *(uint8_t**)(a + 0x134); uint8_t* ht = *(uint8_t**)(a + 0x138);
            int bi = *(int*)(ht + 0xc + idx * 0x10) * 0x1c;
            u32 bflags = *(u32*)(bt + bi + 0xc); u32* blk = (u32*)(bt + bi);
            if ((int32_t)bflags < 0) {
                if (!(flags & 4)) {
                    const char* p = name;
                    while (h->lastSep(p, ':')) p = h->lastSep(p, ':') + 1;
                    while (h->lastSep(p, '\\')) p = h->lastSep(p, '\\') + 1;
                    u32 key = h->cryptSeed(3);
                    if (blk[3] & 0x20000) key = ((blk[0] - *(u32*)(a + 300)) + key) ^ blk[2];
                    u32 nb = (blk[2] - 1 + *(u32*)(a + 0x120)) / *(u32*)(a + 0x120);
                    void* tbl = nullptr;
                    if (blk[3] & 0xff00) tbl = h->alloc(nb * 4 + 4, SF, 0x1181, 0);
                    ok = true;
                    uint32_t* f = (uint32_t*)h->newFile(&G.fileList, 0, 0);
                    h->strcpyN((char*)(f + 2), name, 0x104);
                    f[0x43] = 0xffffffff; f[0x44] = (u32)(uintptr_t)a; f[0x45] = (u32)(uintptr_t)blk; f[0x46] = key;
                    f[0x49] = nb; f[0x4a] = (u32)(uintptr_t)tbl;
                    f[0x4d] = (u32)(uintptr_t)h->alloc(0x1000, SF, 0x118c, 0);
                    f[0x50] = 1; f[0x53] = 1; f[0x54] = 1; f[0x52] = blk[6] != 0;
                    *(int*)(a + 0x140) += 1;
                    *out = f; goto done;
                }
                if ((bflags & 0x1ff00) == 0) {
                    void* fh = h->createFile(a + 8, 0x80000000, 1, 3, 0x8000000);
                    ok = (intptr_t)fh != -1; *out = (u32*)fh;
                    if (ok) { h->setFilePtr(fh, blk[0], 0); goto done; }
                    goto nofile;
                }
            }
            G.lastErr = 0x3ee; h->setErr(0x3ee); goto done;
        }
    }
nofile:
    G.lastErr = 2; h->setErr(2);
done:
    h->unlock();
    return ok;
}

// 1.13c Storm+0x2A0B0
u32 Storm_2A0B0(const char* name, u32 prio, u32 flags, u32** out) {
    N42Host* h = G.h; const char* SF = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";
    char path[260];
    if (!out || !name || !*name) { G.lastErr = 0x57; h->setErr(0x57); if (out) *out = nullptr; return 0; }
    *out = nullptr;
    h->lock();
    if (G.archList == 0) h->initArchives();
    h->strcpyN(path, name, 0x104);
    if ((~(h->getAttr(path) >> 4) & 1) == 0) {
        if (*name == '\\' || h->strstr_(name, ":\\") || h->strstr_(name, "\\\\")) h->strcpyN(path, name, 0x104);
        else h->buildPath(path, 0);
    }
    int cd = h->findArchiveList();
    u32 wantCd = flags & 1;
    if (wantCd && cd == 0) { G.lastErr = 0xf; h->setErr(0xf); h->unlock(); return 0; }
    void* fh = h->createFile(path, 0x80000000, 1, 3, 0);
    if ((intptr_t)fh == -1) { h->unlock(); return 0; }
    u32* a = (u32*)h->newFile(&G.archList, 0, 0);
    h->strcpyN((char*)(a + 2), path, 0x104);
    a[0x45] = prio; a[0x43] = (u32)(uintptr_t)fh; a[0x50] = 1;
    a[0x44] = cd ? 3 : ((flags & 2) ? 1 : 0);
    u32* hdr = (u32*)h->alloc(0x20, SF, 0x1024, 0);
    a[0x4c] = (u32)(uintptr_t)hdr; a[0x4b] = 0;
    uint8_t* buf = (uint8_t*)h->alloc(0x1000, SF, 0x1029, 0);
    u32 base = 0, got = 0;
    for (;;) {
        if (got + base <= a[0x4b]) {
            u32 o = a[0x4b];
            h->setFilePtr(fh, o, 0);
            u32 n = h->readFile(fh, buf, 0x1000);
            if (n < 0x20) {
                h->delFile(a); h->free_(a, "delete", -1);
                G.lastErr = 0x8510006c; h->setErr(0x8510006c); h->unlock();
                h->free_(buf, SF, 0x1044); return 0;
            }
            got = n; base = o;
        }
        memcpy(hdr, buf + (a[0x4b] - base), 0x20);
        if (hdr[0] == 0x1a51504d && hdr[1] > 0x1f) break;
        a[0x4b] += 0x200;
    }
    h->free_(buf, SF, 0x106b);
    u32 bs = 0x200u << (((uint8_t*)hdr)[0xe] & 0x1f);
    a[0x48] = bs; a[0x49] = (u32)(uintptr_t)h->alloc(bs, SF, 0x1070, 0);
    u32* ht = (u32*)h->alloc(hdr[6] << 4, SF, 0x1079, 0); a[0x4e] = (u32)(uintptr_t)ht;
    h->hashInit(a + 0x4f, ht, hdr[6] << 4, a + 2);
    h->decrypt(h->cryptSeed(3), ht);
    u32* bt = (u32*)h->alloc(hdr[7] * 0x1c, SF, 0x1092, 0); a[0x4d] = (u32)(uintptr_t)bt;
    h->hashInit(a + 0x4f, bt, hdr[7] << 4, a + 2);
    h->decrypt(h->cryptSeed(3), bt);
    h->setCount(hdr[7]);
    if (a[0x4b] && hdr[7]) for (u32 i = 0; i < hdr[7]; i++) { u32* e = (u32*)((uint8_t*)bt + i * 0x1c); if (*e) *e += a[0x4b]; }
    // insert into priority-ordered archive list
    u32* pos = (u32*)(intptr_t)G.archHead;
    if ((int)(intptr_t)pos < 1) pos = nullptr;
    else for (;;) { if ((int)pos[0x45] <= (int)prio) break; pos = (u32*)(uintptr_t)pos[1]; if ((int)(intptr_t)pos < 1) { pos = nullptr; break; } if (!pos) break; }
    if (a[0]) {
        u32 nx = a[1];
        u32* prev = ((int)nx < 0) ? (u32*)(uintptr_t)~nx : (u32*)((uint8_t*)a + (nx - *(u32*)(uintptr_t)(a[0] + 4)));
        *prev = a[0]; *(u32*)(uintptr_t)(a[0] + 4) = a[1]; a[0] = 0; a[1] = 0;
    }
    u32* anchor = pos ? pos : (u32*)&G.archListHead;
    a[0] = *anchor; a[1] = *(u32*)(uintptr_t)(a[0] + 4);
    *(u32*)(uintptr_t)(a[0] + 4) = (u32)(uintptr_t)a; *anchor = (u32)(uintptr_t)a;
    if (a[0x44] == 0) {
        if (wantCd && cd == 0) { h->destroyFile(a); G.lastErr = 0xf; h->setErr(0xf); h->unlock(); return 0; }
    } else h->regArchive();
    *out = a;
    h->unlock();
    h->afterOpen();
    return 1;
}

// 1.13c Storm+0x19C80
u32 Storm_19C80(const char* file) {
    N42Host* h = G.h; const char* SD = "..\\3rdParty\\STORM\\SOURCE\\SDRAW.CPP";
    if (!G.sd_ok || !G.sd_hwnd || !G.sd_pal || G.sd_bpp != 8) return 0;
    h->palSnap();
    void* buf = h->alloc(G.sd_h * G.sd_w, SD, 0x19a, 0);
    if (G.sd_dd) h->surfSnap();
    int pitch; void* bits;
    if (h->lockSurf(0, nullptr, &pitch, &bits, 0)) { h->blit(bits, buf, pitch, G.sd_w, G.sd_h, G.sd_w, bits, 0, 0, 0xcc0020); h->unlockSurf(0, pitch); }
    char out[260]; out[0] = 0;
    if (!file || !*file) { if (!h->saveDlg(out, (void*)(intptr_t)G.sd_hwnd)) { h->free_(buf, SD, 0x1ca); return 0; } }
    else h->strcpyN(out, file, 0x104);
    h->savePcx(out, G.sd_palBuf, buf, G.sd_w, G.sd_h, G.sd_bpp);
    h->free_(buf, SD, 0x1d7);
    return 1;
}
