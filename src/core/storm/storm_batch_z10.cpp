// Storm batch z10: SBMP/SFile-side helpers. Retail globals and callees are seam parameters; 32-bit links are uint32_t.
// Implicit register args (EAX/ESI/ECX in the decompile) are explicit params and are marked GUESSED where the register role was inferred.
#include <cstdint>
#include <cstring>

struct StormZ10Env {
    uint32_t* lastError;                                            // DAT_6fc45388
    void (*setLastError)(uint32_t);                                 // SetLastError
    void (*enter)(void*); void (*leave)(void*);                     // Enter/LeaveCriticalSection
    void* csList;                                                   // DAT_6fc45940
    void* csFile;                                                   // DAT_6fc45988
    uint32_t* (*list2690)(void* ecx, uint32_t table);               // Storm+0x2690
    uint32_t table458f0;                                            // 0x6fc458f0
    void (*sub317F0)(uint32_t* e);                                  // Storm+0x317F0
    void (*sub308A0)(uint32_t* e, const float* rect);               // Storm+0x308A0
    void (*sub316D0)(int a, const float* r, uint32_t b, int c);     // Storm+0x316D0
    void (*sub319C0)(int a, const float* r, uint32_t* n, uint32_t* o); // Storm+0x319C0
    void (*sub318F0)(int a, uint32_t* n, uint32_t* o);              // Storm+0x318F0
    int32_t (*ftol)(void* st0);                                     // Storm+0x3C908 (x87 st0 -> int)
    // file side
    int (*read195B0)(uint8_t* f, void* buf, uint32_t n, int* got, int, int, int); // Storm+0x195B0
    int (*read19BE0)(uint8_t* f, void* buf, uint32_t n, int* got, int);           // Storm+0x19BE0
    uint32_t (*size162D0)(uint8_t* f, uint32_t* hi);                              // Storm+0x162D0
    uint32_t (*getFileSize)(uint32_t h);                            // GetFileSize
    uint32_t (*setFilePointer)(uint32_t h, int32_t d, int method);  // SetFilePointer
    void (*close16E20)(uint8_t* f);                                 // Storm+0x16E20
    int (*open18960)(void*, const void* name, uint8_t flags, uint8_t** out); // Storm+0x18960
    void* (*alloc)(uint32_t n, const char* file, int line, int fl); // Storm+0xC580
    void (*free)(void* p, const char* file, int line);              // Storm+0xB960
    uint32_t* openFlags43130; int32_t* cnt45984;                    // DAT_6fc43130 / DAT_6fc45984
    int (*bmp220B0)(uint32_t*, uint32_t*, int, uint32_t*, uint32_t);                     // Storm+0x220B0
    int (*bmp21E80)(const void*, int*, uint32_t, int, uint8_t*, uint32_t, int*);         // Storm+0x21E80
    int (*bmp228C0)(uint32_t, const void*, uint8_t*, uint32_t, uint32_t*, uint32_t*, uint32_t*); // Storm+0x228C0
    int (*bmp21FF0)(uint32_t, void*, void*, uint32_t, uint32_t, uint32_t);              // Storm+0x21FF0
    void (*bmp21C40)(int, uint32_t, uint8_t*, const uint8_t*, uint32_t, uint32_t, uint32_t); // Storm+0x21C40
    int (*tga22690)(void); int (*tga225C0)(const void*, uint8_t*, uint32_t, void*);     // Storm+0x22690 / 0x225C0
    int (*tga222F0)(uint8_t*, const void*, uint32_t, uint8_t*);                         // Storm+0x222F0
};

static inline uint32_t& F(uint8_t* f, uint32_t o) { return *(uint32_t*)(f + o); }
static inline uint32_t Zm(uint32_t* e, uint32_t i) { return e[i]; }

// Memory-file / handle-file relative seek (the inlined critical-section idiom). method 2 = from end, else relative.
static void Z10Seek(const StormZ10Env& E, uint8_t* f, int32_t d, int method) {
    E.enter(E.csFile);
    if (F(f, 0x10c) == 0xFFFFFFFFu) {
        uint32_t sz = *(uint32_t*)(uintptr_t)(F(f, 0x114) + 8);
        if (method == 2) F(f, 0x11c) = sz < (uint32_t)-d ? 0 : sz + d;
        else if (d < 0 && F(f, 0x11c) < (uint32_t)-d) F(f, 0x11c) = 0;
        else F(f, 0x11c) += d;
        uint32_t m = sz - 1;
        if (F(f, 0x11c) < m) m = F(f, 0x11c);
        F(f, 0x11c) = m; F(f, 0x138) = 0; F(f, 0x13c) = 0;
        E.leave(E.csFile);
    } else { E.leave(E.csFile); E.setFilePointer(F(f, 0x10c), d, method); }
}
static uint32_t Z10Size(const StormZ10Env& E, uint8_t* f) {
    E.enter(E.csFile);
    uint32_t r;
    if (F(f, 0x10c) == 0xFFFFFFFFu) { r = *(uint32_t*)(uintptr_t)(F(f, 0x114) + 8); E.leave(E.csFile); }
    else { E.leave(E.csFile); r = E.getFileSize(F(f, 0x10c)); }
    return r;
}
static void Z10Err(const StormZ10Env& E) { *E.lastError = 0x57; E.setLastError(0x57); }

// 1.13c Storm+0x318F0
void Storm_Sub318F0(const StormZ10Env& E, int h, uint32_t* n, uint32_t* out) {
    if (h == 0 || !n) { Z10Err(E); return; }
    E.enter(E.csList);
    uint32_t* e = E.list2690(nullptr, E.table458f0);
    if (!e) { E.leave(E.csList); *n = 0; return; }
    if (e[0x17]) { E.sub317F0(e); e[0x17] = 0; }
    if (out) {
        uint32_t c = *n; if (e[0xb] <= c) c = e[0xb];
        *n = c;
        memcpy(out, (void*)(uintptr_t)e[0xc], (size_t)(c & 0xfffffff) * 16);
        E.leave(E.csList); return;
    }
    *n = e[0xb];
    E.leave(E.csList);
}
// 1.13c Storm+0x319C0
void Storm_Sub319C0(const StormZ10Env& E, int h, const float* r, uint32_t* n, uint32_t* out) {
    if (h == 0 || !r || !n) { Z10Err(E); return; }
    if (r[0] < r[2] && r[1] < r[3]) {
        E.enter(E.csList);
        uint32_t* e = E.list2690(nullptr, E.table458f0);
        if (e) {
            if (e[0x17]) { E.sub317F0(e); e[0x17] = 0; }
            E.sub308A0(e, r);
            uint32_t cnt = e[0xf];
            if (out) {
                uint32_t c = *n; if (cnt <= c) c = cnt;
                *n = c;
                const uint32_t* s = (const uint32_t*)(uintptr_t)e[0x10];
                for (uint32_t i = 0; i < c; i++, s += 2) out[i] = *s;
                E.leave(E.csList); return;
            }
            *n = 0; E.leave(E.csList); return;
        }
        E.leave(E.csList);
    }
    *n = 0;
}
// 1.13c Storm+0x31AC0
void Storm_Sub31AC0(const StormZ10Env& E, int h, const int* r, uint32_t b, int c) {
    if (!r) { Z10Err(E); return; }
    float f[4] = { (float)r[0], (float)r[1], (float)r[2], (float)r[3] };
    E.sub316D0(h, f, b, c);
}
// 1.13c Storm+0x31B30  (GUESSED: each 0x3C908 call converts the float already stored in that slot; x87 input implicit)
void Storm_Sub31B30(const StormZ10Env& E, int h, uint32_t* n, uint32_t* out) {
    E.sub318F0(h, n, out);
    if (out && *n) for (uint32_t i = 0; i < *n; i++) for (int k = 0; k < 4; k++) out[i * 4 + k] = (uint32_t)E.ftol(&out[i * 4 + k]);
}
// 1.13c Storm+0x31BA0
void Storm_Sub31BA0(const StormZ10Env& E, int h, const int* r, uint32_t* n, uint32_t* out) {
    if (!r) { Z10Err(E); return; }
    float f[4] = { (float)r[0], (float)r[1], (float)r[2], (float)r[3] };
    E.sub319C0(h, f, n, out);
}

static const char* kSbmp = "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP";
// type: 1 = BMP ('BM'), 2 = PCX (0x50a), 3 = TGA
static int Z10Kind(const uint8_t* p, uint32_t n) {
    if (n <= 1) return 0;
    uint16_t m = *(const uint16_t*)p;
    return m == 0x50a ? 2 : m != 0x4d42 ? 3 : 1;
}
static uint8_t Z10OpenFlags(const StormZ10Env& E) {
    uint8_t b = (*E.openFlags43130 & 1) != 0;
    if (*E.openFlags43130 & 2) b |= 2;
    if ((*E.cnt45984 < 1 || *E.cnt45984 == 0) && *E.openFlags43130 == 0) b |= 1;
    return b;
}

// 1.13c Storm+0x333F0  (SBmpLoadImage-from-name: size probe, then decode into allocated buffer)
int Storm_Sub333F0(const StormZ10Env& E, const char* name, const int* pal, uint8_t** outBuf, uint32_t* w, uint32_t* h, uint32_t* bpp,
                   uint32_t wantBpp, uint8_t* (*userAlloc)(uint32_t)) {
    if (outBuf) *outBuf = nullptr; if (w) *w = 0; if (h) *h = 0; if (bpp) *bpp = 0;
    if (!name || !*name || !outBuf) { Z10Err(E); return 0; }
    uint8_t* f;
    if (!E.open18960(nullptr, name, Z10OpenFlags(E), &f)) return 0;
    uint32_t sz = Z10Size(E, f);
    uint8_t* buf = (uint8_t*)E.alloc(sz, kSbmp, 0x5b6, 0);
    E.read195B0(f, buf, sz, nullptr, 0, 0, 0);
    E.close16E20(f);
    int ok = 0; uint32_t bw = 0, bh = 0, bb = 0;
    if (!buf || !sz) Z10Err(E);
    else {
        int k = Z10Kind(buf, sz); int r = 0;
        if (k == 1) r = E.bmp220B0(&bh, &bb, 0, nullptr, 0);
        else if (k == 2) r = E.bmp21E80(buf, (int*)&bh, sz, 0, nullptr, 0, (int*)&bb);
        else if (k == 3) r = E.bmp228C0(sz, nullptr, nullptr, 0, &bw, &bh, &bb);
        if (r) {
            if (wantBpp == 0) wantBpp = bb;
            if (wantBpp == bb) {
                int bits = (int)(bw * bh * wantBpp);
                uint32_t bytes = (uint32_t)(bits + ((bits >> 31) & 7)) >> 3;
                *outBuf = userAlloc ? userAlloc(bytes) : (uint8_t*)E.alloc(bytes, name, -4, 0);
                if (*outBuf) {
                    if (bytes == 0) Z10Err(E);
                    else {
                        k = Z10Kind(buf, sz);
                        if (k == 1) E.bmp220B0(nullptr, nullptr, (int)(intptr_t)pal, (uint32_t*)*outBuf, bytes);
                        else if (k == 2) E.bmp21E80(buf, nullptr, sz, (int)(intptr_t)pal, *outBuf, bytes, nullptr);
                        else if (k == 3) E.bmp228C0(sz, pal, *outBuf, bytes, nullptr, nullptr, nullptr);
                    }
                    if (h) *h = bw; if (w) *w = bh; if (bpp) *bpp = wantBpp;  // decompile stores local_18 (bw) to param_4, local_14 to param_5
                    ok = 1;
                }
            }
        }
    }
    E.free(buf, kSbmp, 0x5f3);
    return ok;
}

// 1.13c Storm+0x33730  (PCX info/palette/pixels; GUESSED: EAX = out width, ECX(this) = file object)
int Storm_Sub33730(const StormZ10Env& E, uint8_t* f, uint32_t* outW, int pal, uint8_t* pix, int, int* outH, int* outBpp) {
    uint8_t hdr[0x80];
    if (!E.read195B0(f, hdr, 0x80, nullptr, 0, 0, 0)) return 0;
    uint16_t xmin = *(uint16_t*)(hdr + 4), ymin = *(uint16_t*)(hdr + 6), xmax = *(uint16_t*)(hdr + 8), ymax = *(uint16_t*)(hdr + 10);
    uint16_t bpl = *(uint16_t*)(hdr + 0x42);
    uint32_t w = (uint32_t)xmax - xmin + 1;
    int bpp = hdr[0x41] * hdr[3];
    if (outW) *outW = w; if (outH) *outH = (int)ymax - ymin + 1; if (outBpp) *outBpp = bpp;
    bool has = pal != 0 && bpp == 8;
    if (!pix) {
        if (!has) return 1;
        E.enter(E.csFile);
        if (F(f, 0x10c) == 0xFFFFFFFFu) E.leave(E.csFile); else E.leave(E.csFile);
        Z10Seek(E, f, -0x300, 2);
    } else {
        uint32_t total = Z10Size(E, f);
        Z10Seek(E, f, 0, 1);
        uint32_t cur = F(f, 0x10c) == 0xFFFFFFFFu ? F(f, 0x11c) : E.setFilePointer(F(f, 0x10c), 0, 1);
        uint32_t n = (total - ((bpp != 8) ? 0u : 0x300u)) - cur;
        uint8_t* b = (uint8_t*)E.alloc(n, kSbmp, 0x46b, 0);
        E.read195B0(f, b, n, nullptr, 0, 0, 0);
        E.bmp21C40((int)(intptr_t)pix, n - (bpp == 8), pix, b, w, bpl, hdr[0x41]);
        E.free(b, kSbmp, 0x474);
    }
    if (has) {
        uint8_t p[0x300];
        E.read195B0(f, p, 0x300, nullptr, 0, 0, 0);
        uint8_t* d = (uint8_t*)(intptr_t)pal; const uint8_t* s = p;
        for (int i = 0; i < 0x100; i++, s += 3, d += 4) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = 0; }
    }
    return 1;
}

// 1.13c Storm+0x33A10  (BMP info/palette/pixels; GUESSED: ECX(this) = out width ptr, EAX = out bpp ptr, ESI = file object)
int Storm_Sub33A10(const StormZ10Env& E, uint8_t* f, uint32_t* outW, uint32_t* outBpp, int pal, uint32_t* pix, uint32_t arg, uint32_t* outH) {
    uint8_t fh[0xe]; uint8_t ih[0x28];
    if (!E.read195B0(f, fh, 0xe, nullptr, 0, 0, 0) || !E.read195B0(f, ih, 0x28, nullptr, 0, 0, 0)) return 0;
    uint32_t imgSz = *(uint32_t*)(ih + 0x14);
    uint16_t bits = *(uint16_t*)(ih + 0xe);
    int32_t off = *(int32_t*)(fh + 0xa);
    if (outW) *outW = *(uint32_t*)(ih + 4);
    if (outH) *outH = *(uint32_t*)(ih + 8);
    if (outBpp) *outBpp = bits;
    if (bits < 9) {
        if (pal) {
            uint8_t p[0x400];
            if (!E.read195B0(f, p, 0x400, nullptr, 0, 0, 0)) return 0;
            uint8_t* d = (uint8_t*)(intptr_t)pal;
            for (int i = 0; i < 0x100; i++, d += 4) { d[0] = p[i * 4 + 2]; d[1] = p[i * 4 + 1]; d[2] = p[i * 4]; d[3] = 0; }
        }
    } else pal = 0;
    if (pix) {
        int32_t d = (off - (pal ? 0x400 : 0)) - 0x36;
        Z10Seek(E, f, d, 1);
        int got = 0;
        void* b = E.alloc(imgSz, kSbmp, 0x1ff, 0);
        E.read195B0(f, b, imgSz, &got, 0, 0, 0);
        uint32_t row = (uint32_t)((int)bits * *(int32_t*)(ih + 4)) >> 3, stride = row;
        if (row & 3) stride = row + (4 - (row & 3));
        E.bmp21FF0(arg, pix, b, arg, row, stride);
        E.free(b, kSbmp, 0x20b);
        if ((uint32_t)got < imgSz) return 0;
    }
    return 1;
}

// 1.13c Storm+0x33C30  (TGA header/palette/pixels; GUESSED: ESI = file object)
int Storm_Sub33C30(const StormZ10Env& E, uint8_t* f, void* palOut, uint8_t* pixOut, uint32_t* outW, uint32_t* outH, uint32_t* outBpp) {
    uint8_t h[0x12]; int got = 0;
    if (!E.read195B0(f, h, 0x12, &got, 0, 0, 0) || got != 0x12 || !E.tga22690()) return 0;
    uint32_t w = *(uint16_t*)(h + 12), ht = *(uint16_t*)(h + 14), bpp = h[16];
    uint32_t idLen = h[0];
    uint32_t palBytes = h[1] ? (uint32_t)((h[7] + 7) >> 3) * *(uint16_t*)(h + 5) : 0;
    uint32_t pixBytes = h[2] < 9 ? (uint32_t)((bpp + 7) >> 3) * ht * w : 1;
    if (E.size162D0(f, nullptr) < pixBytes + idLen + 0x12 + palBytes) return 0;
    if (outW) *outW = w; if (outH) *outH = ht; if (outBpp) *outBpp = bpp;
    Z10Seek(E, f, (int32_t)idLen, 1);
    if (palOut && h[1]) {
        uint32_t tmp[256];
        E.read19BE0(f, tmp, palBytes, nullptr, 0);
        if (!E.tga225C0(h, (uint8_t*)tmp, palBytes, palOut)) return 0;
    }
    if (pixOut) {
        uint32_t n = E.size162D0(f, nullptr) + ((-0x12 - idLen) - palBytes);
        void* b = E.alloc(n, kSbmp, 0x116, 0);
        E.read19BE0(f, b, n, nullptr, 0);
        if (!E.tga222F0((uint8_t*)b, h, n, pixOut)) { E.free(b, kSbmp, 0x11d); return 0; }
        E.free(b, kSbmp, 0x120);
    }
    return 1;
}

// 1.13c Storm+0x33EA0  (SBmpLoadImage dispatcher; kind sniffed from the first two bytes)
int Storm_Sub33EA0(const StormZ10Env& E, const char* name, int pal, uint8_t* pix, int pixSz, uint32_t* w, uint32_t* h, uint32_t* bpp,
                   int (*pcx)(uint8_t*, int, uint8_t*, int, int*, int*), int (*bmp)(uint32_t*, int, uint8_t*, int, uint32_t*), int (*tga)(int, uint8_t*, int, uint32_t*, uint32_t*, uint32_t*)) {
    (void)pcx; (void)bmp; (void)tga;
    if (w) *w = 0; if (h) *h = 0; if (bpp) *bpp = 0;
    if (!(name && *name && pixSz == 0 && pix)) { Z10Err(E); return 0; }
    uint8_t* f;
    if (!E.open18960(nullptr, name, Z10OpenFlags(E), &f)) return 0;
    int k = 0; uint16_t magic = 0;
    if (E.read195B0(f, &magic, 2, nullptr, 0, 0, 0)) {
        Z10Seek(E, f, -2, 1);
        k = magic == 0x50a ? 2 : magic != 0x4d42 ? 3 : 1;
    }
    int r = 0;
    if (k == 1) r = Storm_Sub33A10(E, f, w, bpp, pal, (uint32_t*)pix, (uint32_t)pixSz, h);
    else if (k == 2) r = Storm_Sub33730(E, f, w, pal, pix, pixSz, (int*)h, (int*)bpp);
    else if (k == 3) r = Storm_Sub33C30(E, f, (void*)(intptr_t)pal, pix, w, h, bpp);
    E.close16E20(f);
    return r;
}

// 1.13c Storm+0x340B0
uint32_t Storm_Sub340B0(uint32_t* g42fd0, uint32_t v) { *g42fd0 = v; return 1; }
// 1.13c Storm+0x34370
void* Storm_Sub34370(const StormZ10Env& E, int a, void* fn24300) {
    if (a != 0) { Z10Err(E); return nullptr; }
    return fn24300;
}
