// Fog n50 batch: QServer completion-port core, pool realloc, PKWARE-style Huffman table build,
// CPU banner, CRT fragments. Retail's pointers/handles/Win32 calls are host seams (args); the
// logic and its order follow the 1.13c decompile. Struct fields carry the retail dword offsets.
#include <cstdint>
#include <cstring>

struct QSLock { void* os; };
struct QSConn {
    uint32_t id;        // +0x00
    QSConn*  nextId;    // +0x04
    int32_t  refs;      // +0x08
    uint32_t closing;   // +0x0c
    uint32_t sock;      // +0x10 (0xFFFFFFFF = closed)
    uint32_t key;       // +0x18
    QSConn*  nextKey;   // +0x1c
    QSLock   lock;      // +0x2c
    uint32_t flags;     // +0x58
    uint32_t sendMode;  // +0x74
    uint32_t lastRecv;  // +0x9c
    uint32_t lastSend;  // +0xa0
    uint32_t rxBytes;   // +0xa8
};
struct QServer {
    uint32_t mask;          // +0x0c
    QSConn** byId;          // +0x14
    QSLock*  idLocks;       // +0x1c (stride 0x18 in retail)
    QSConn** byKey;         // +0x20
    QSLock   keyLock;       // +0x28
    QSLock   countLock;     // +0x854
    int32_t  nconn;         // +0x880
    void   (*freeCb)(const char* file, int line);  // +0xbc8
    uint64_t hA[4], pA[4];  // +0xbd8, +0xbe8
    uint32_t fA[4];         // +0xbf8
    uint64_t iocp[4];       // +0xc08
    uint32_t hasB;          // +0xc18
    uint64_t hB[4], pB[4];  // +0xc1c, +0xc2c
    uint32_t fB[4];         // +0xc3c
};
struct QSHost {
    void*    user;
    void     (*enter)(void*, QSLock*);
    void     (*leave)(void*, QSLock*);
    uint32_t (*tick)(void*);                                  // Fog+0xC9E0
    void     (*fatal)(void*, int line);                       // Fog+0xFC20/0x10F30/0x184A0 then _exit
    void     (*e3b0)(void*, QServer*);
    void     (*e550)(void*, QServer*, QSConn*);
    void     (*d0a0)(void*, QServer*);
    void     (*fe60)(void*, QServer*, QSConn*, const uint8_t*, uint32_t);
    void     (*ff60)(void*);
    void     (*fef0)(void*);
    void     (*f70010)(void*);
    void     (*ee60)(void*, QServer*, QSConn*);
    int64_t  (*send)(void*, uint32_t sock, const uint8_t*, uint32_t);   // Ordinal_19; -1 on error
    int32_t  (*wsaErr)(void*);                                // Ordinal_111
    bool     (*gqcs)(void*, uint64_t port, uint32_t* bytes, uint32_t* key, uint8_t* ovKind, bool* haveOv, uint32_t timeout);
    uint32_t (*lastErr)(void*);
    bool     (*waitAll)(void*, uint32_t n, const uint64_t* h, uint32_t ms);
    void     (*closeH)(void*, uint64_t);
};

void fog_qserver_remove_e1b0(QServer* q, const QSHost* h, uint32_t id, uint32_t id2);

// 1.13c Fog+0xE1B0
void fog_qserver_remove_e1b0(QServer* q, const QSHost* h, uint32_t id, uint32_t id2)
{
    uint32_t b = id & q->mask;
    QSLock* bl = &q->idLocks[b];
    h->enter(h->user, bl);
    QSConn* prev = nullptr; QSConn* c = q->byId[b];
    while (c && c->id != id) { prev = c; c = c->nextId; }
    if (!c) { h->leave(h->user, bl); return; }
    h->enter(h->user, &q->countLock);
    q->nconn--;
    h->leave(h->user, &q->countLock);
    if (c == q->byId[b]) q->byId[b] = c->nextId; else prev->nextId = c->nextId;
    c->nextId = nullptr;
    h->leave(h->user, bl);
    if (id != c->id) h->fatal(h->user, 0x3dc);
    h->tick(h->user);
    h->enter(h->user, &q->keyLock);
    uint32_t k = c->key & q->mask;
    QSConn* head = q->byKey[k]; QSConn* p = nullptr;
    for (QSConn* n = head; n; p = n, n = n->nextKey)
        if (n->id == id2) { if (n == head) q->byKey[k] = n->nextKey; else p->nextKey = n->nextKey; break; }
    h->leave(h->user, &q->keyLock);
    h->enter(h->user, &q->countLock);
    h->d0a0(h->user, q);
    h->leave(h->user, &q->countLock);
}

static void qs_unref(QServer* q, const QSHost* h, QSConn* c)
{
    if (__atomic_sub_fetch(&c->refs, 1, __ATOMIC_SEQ_CST) == 0) {
        if (q->freeCb) q->freeCb("..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x374);
        fog_qserver_remove_e1b0(q, h, c->id, c->id);
        h->e3b0(h->user, q);
    }
}

// 1.13c Fog+0x20180
uint32_t fog_qserver_send_20180(QServer* q, const QSHost* h, QSConn* c, const uint8_t* buf, uint32_t len)
{
    h->enter(h->user, &c->lock);
    c->lastSend = h->tick(h->user);
    if (c->sendMode == 0) {
        if (c->sock == 0xFFFFFFFFu) {
            h->leave(h->user, &c->lock);
            h->e550(h->user, q, c);
            qs_unref(q, h, c);
            return 0;
        }
        int64_t s = h->send(h->user, c->sock, buf, len);
        uint32_t sent = (uint32_t)s;
        if (s == -1) {
            if (h->wsaErr(h->user) != 0x2733) {
                h->leave(h->user, &c->lock);
                h->e550(h->user, q, c);
                return 0;
            }
            sent = 0;
        }
        if (sent < len) { h->fe60(h->user, q, c, buf + sent, len - sent); h->ff60(h->user); }
    } else {
        h->fe60(h->user, q, c, buf, len);
    }
    h->leave(h->user, &c->lock);
    return 1;
}

// 1.13c Fog+0x202B0
void fog_qserver_shutdown_202b0(QServer* q, const QSHost* h)
{
    uint64_t w[8]; uint32_t n = 0;
    h->enter(h->user, &q->countLock);
    for (int i = 0; i < 4; i++) {
        if (q->hA[i]) w[n++] = q->hA[i];
        q->fA[i] = 1;
        if (q->hasB && q->hB[i]) w[n++] = q->hB[i];
        q->fB[i] = 1;
    }
    h->leave(h->user, &q->countLock);
    if (n) h->waitAll(h->user, n, w, 1000);
    for (int i = 0; i < 4; i++) {
        if (q->hA[i]) { h->closeH(h->user, q->hA[i]); q->hA[i] = 0; q->pA[i] = 0; }
        if (q->hasB && q->hB[i]) { h->closeH(h->user, q->hB[i]); q->hB[i] = 0; q->pB[i] = 0; }
    }
}

// 1.13c Fog+0x207F0
uint32_t fog_qserver_poll_207f0(QServer* q, const QSHost* h, int port, uint32_t timeoutMs)
{
    uint32_t result = 0;
    uint32_t t0 = h->tick(h->user);
    if (100 < t0 - h->tick(h->user)) return result;
    for (;;) {
        bool isSend = false; QSConn* c = nullptr;
        uint32_t bytes = 0, key = 0, recvFlag = 0; uint8_t kind = 0; bool haveOv = false;
        bool ok = h->gqcs(h->user, q->iocp[port], &bytes, &key, &kind, &haveOv, timeoutMs);
        if (!ok && !haveOv) {
            uint32_t e = h->lastErr(h->user);
            if (e == 0x102) return result;
            h->fatal(h->user, 0x3c1);
            return result;
        }
        recvFlag = key;
        if (q) {
            uint32_t b = q->mask & key;
            h->enter(h->user, &q->idLocks[b]);
            for (c = q->byId[b]; c && c->id != recvFlag; c = c->nextId) {}
            h->leave(h->user, &q->idLocks[b]);
            if (c) {
                recvFlag = (kind & 1) == 1;
                isSend = (kind & 3) == 2;
                if (!ok) {
                    h->e550(h->user, q, c);
                    qs_unref(q, h, c);
                } else if (bytes == 0) {
                    if (c->sock != 0xFFFFFFFFu) h->e550(h->user, q, c);
                    qs_unref(q, h, c);
                } else if (isSend) {
                    h->fef0(h->user);
                    h->enter(h->user, &c->lock);
                    if (c->closing == 0) h->ff60(h->user);
                    h->leave(h->user, &c->lock);
                    qs_unref(q, h, c);
                } else if (recvFlag != 0) {
                    h->enter(h->user, &c->lock);
                    c->flags &= 0xfffffffbu;
                    c->rxBytes += bytes;
                    c->lastRecv = h->tick(h->user);
                    h->leave(h->user, &c->lock);
                    h->ee60(h->user, q, c);
                    result = 1;
                    if (c->closing == 0) h->f70010(h->user);
                    qs_unref(q, h, c);
                }
            }
        }
        timeoutMs = 0;
        if (100 < t0 - h->tick(h->user)) return result;
    }
}

// 1.13c Fog+0x1BDD0
struct FogPoolInfo { QSLock lock; int32_t elemSize; int32_t elemCount; };
struct FogPool { uint8_t* base; uint32_t* bitmap; FogPoolInfo* info; };
struct FogReallocHost {
    void* user;
    uint8_t* (*allocFrom)(void*, void* arg);        // Fog+0xB560
    uint8_t* (*allocSize)(void*, int size);         // Fog+0xB9F0
    FogPool* (*findPool)(void*, uint8_t* p);        // Fog+0xA9F0
    void     (*freeBlk)(void*, uint8_t* p);         // Fog+0xB0C0
    void     (*enter)(void*, QSLock*);
    void     (*leave)(void*, QSLock*);
    void     (*fatal)(void*, int line);             // never returns
};
uint8_t* fog_realloc_bdd0(const FogReallocHost* h, void* arg, uint8_t* p, int size)
{
    if (!p) {
        uint8_t* r = h->allocFrom(h->user, arg);
        if (!r) h->fatal(h->user, 0x62d);
        return r;
    }
    FogPool* pool = h->findPool(h->user, p);
    uint8_t* r;
    if (!pool) {
        r = h->allocSize(h->user, size);
        if (!r) h->fatal(h->user, 0x635);
        return r;
    }
    if (pool->base > p) {
        r = h->allocSize(h->user, size);
        if (!r) h->fatal(h->user, 0x63c);
        return r;
    }
    FogPoolInfo* in = pool->info;
    int32_t idx = (int32_t)(p - pool->base) / in->elemSize;
    if (idx >= in->elemCount) {
        r = h->allocSize(h->user, size);
        if (!r) h->fatal(h->user, 0x648);
        return r;
    }
    h->enter(h->user, &in->lock);
    if (!((1u << (idx & 31)) & pool->bitmap[idx >> 5])) { h->leave(h->user, &in->lock); h->fatal(h->user, 0x6a0); return nullptr; }
    int32_t es = in->elemSize;
    if (size <= es) { h->leave(h->user, &in->lock); return p; }
    h->leave(h->user, &in->lock);
    r = h->allocFrom(h->user, arg);
    if (!r) { h->fatal(h->user, 0x68b); return nullptr; }
    memcpy(r, p, (size_t)es);
    h->freeBlk(h->user, p);
    return r;
}

// 1.13c Fog+0x1EFD0
struct FogHuff { uint8_t lens[256]; uint8_t code[256]; uint32_t table[256]; uint8_t pool[0x300]; };
uint32_t fog_huff_build_1efd0(FogHuff* H, const uint8_t* L)
{
    int32_t cnt[16] = {0};
    uint8_t sym[256], slen[256];
    for (int i = 0; i < 256; i++) {
        if (L[i] == 0 || L[i] >= 0x10) return 0;
        cnt[L[i]]++;
    }
    int32_t acc = 0;
    for (int i = 15; i > 0; i--) { int32_t t = cnt[i]; cnt[i] = acc; acc += t; }
    cnt[0] = acc;
    for (int i = 0; i < 256; i++) sym[cnt[L[i]]++] = (uint8_t)i;
    for (int k = 0; k < 256; k++) { slen[k] = L[sym[k]]; H->lens[k] = L[k]; }
    H->code[sym[0]] = 0;
    for (int k = 0; k < 255; k++)
        H->code[sym[k + 1]] = (uint8_t)((int)(H->code[sym[k]] + 1) >> ((slen[k] - slen[k + 1]) & 0x1f));
    uint32_t pp = 0;
    for (int k = 255; k >= 0; k--) {
        uint8_t len = slen[k], s = sym[k], c = H->code[s];
        if (len < 9) {
            int n = 1 << (8 - len);
            uint32_t* e = &H->table[(uint32_t)c << (8 - len)];
            *e = pp; H->pool[pp] = 0; H->pool[pp + 1] = s; pp += 2;
            for (int i = 0; i < n; i++) e[i] = *e;
        } else {
            uint32_t hi = c >> (len - 8);
            int n = 1 << (len - 8);
            if ((c & (n - 1)) == 0) {
                H->table[hi] = pp;
                H->pool[pp++] = (uint8_t)(len - 8);
                int i = 0; const uint8_t* sp = &sym[k];
                while (i < n) {
                    int m = 1 << (len - slen[k + i]);
                    if (m) {
                        uint8_t v = *sp; pp += m;
                        do { H->pool[H->table[hi] + 1 + i] = v; i++; m--; } while (m);
                    }
                    sp++;
                }
            }
        }
    }
    return 1;
}

// 1.13c Fog+0x1C830
struct FogCpuInfo { uint32_t family; const char* vendor; const char* versionStr;
                    uint32_t eax, brand, mhz; };
void fog_cpu_banner_1c830(const FogCpuInfo* c, void (*log)(void*, const char* fmt, const void* a, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t), void* u)
{
    if (c->family == 3) { log(u, "Intel 386", nullptr, 0, 0, 0, 0, 0); return; }
    if (c->family == 4) { log(u, "Intel 486", nullptr, 0, 0, 0, 0, 0); return; }
    log(u, "Vendor:  %s", c->vendor, 0, 0, 0, 0, 0);
    log(u, "Version: %s (Type %X, Family %X, Model %X, Stepping %X, Brand %X)", c->versionStr,
        (c->eax >> 12) & 3, (c->eax >> 8) & 0xf, (c->eax >> 4) & 0xf, c->eax & 0xf, c->brand);
    log(u, "Speed: Approx. %d MHz", nullptr, c->mhz, 0, 0, 0, 0);
}

// 1.13c Fog+0x3279  (CRT __crtLCMapStringA; OS and codepage conversion are seams)
struct FogLcmapHost {
    void* user;
    int32_t  mode;                    // DAT_6ff80260: 0 unprobed, 1 wide API works, 2 ANSI only
    uint32_t defLcid, defAcp;         // DAT_6ff80500 / DAT_6ff80510
    int32_t  (*lcMapW)(void*, uint32_t lcid, uint32_t fl, const uint16_t* s, int n, uint16_t* d, int dn);
    int32_t  (*lcMapA)(void*, uint32_t lcid, uint32_t fl, const char* s, int n, char* d, int dn);
    int32_t  (*mbToWc)(void*, uint32_t cp, uint32_t fl, const char* s, int n, uint16_t* d, int dn);
    int32_t  (*wcToMb)(void*, uint32_t cp, uint32_t fl, const uint16_t* s, int n, char* d, int dn);
    uint32_t (*lastErr)(void*);
    uint32_t (*cpOfLcid)(void*, uint32_t lcid);                                 // Fog+0x586E0
    char*    (*convert)(void*, uint32_t from, uint32_t to, const char* s, uint32_t* n, char* d, int dn); // Fog+0x58727
};
#include <cstdlib>
uint32_t fog_lcmapstringa_3279(FogLcmapHost* h, uint32_t lcid, uint32_t flags, const char* src, int32_t srcLen,
                               char* dst, int32_t dstLen, uint32_t cp, int32_t err)
{
    if (h->mode == 0) {
        if (h->lcMapW(h->user, 0, 0x100, (const uint16_t*)u"", 1, nullptr, 0) == 0) {
            if (h->lastErr(h->user) == 0x78) h->mode = 2;
        } else h->mode = 1;
    }
    if (srcLen > 0) {
        int32_t i = 0;
        while (i < srcLen && src[i]) i++;
        srcLen = i;
    }
    if (h->mode != 2 && h->mode != 0) {
        if (h->mode != 1) return 0;
        uint32_t r = 0;
        if (cp == 0) cp = h->defAcp;
        int32_t n = h->mbToWc(h->user, cp, (err != 0) * 8 + 1, src, srcLen, nullptr, 0);
        if (n == 0) return 0;
        uint16_t* w = (uint16_t*)malloc((size_t)n * 2);
        if (!w) return 0;
        if (h->mbToWc(h->user, cp, 1, src, srcLen, w, n) != 0) {
            uint32_t m = (uint32_t)h->lcMapW(h->user, lcid, flags, w, n, nullptr, 0);
            r = m;
            if (m != 0) {
                if ((flags & 0x400) == 0) {
                    uint16_t* w2 = (uint16_t*)malloc((size_t)m * 2);
                    if (w2) {
                        if (h->lcMapW(h->user, lcid, flags, w, n, w2, (int32_t)m) != 0)
                            r = (uint32_t)h->wcToMb(h->user, cp, 0, w2, (int32_t)m, dst, dstLen);
                        free(w2);
                    }
                } else if (dstLen != 0 && (int32_t)m <= dstLen) {
                    h->lcMapW(h->user, lcid, flags, w, n, (uint16_t*)dst, dstLen);
                }
            }
        }
        free(w);
        return r;
    }
    if (lcid == 0) lcid = h->defLcid;
    if (cp == 0) cp = h->defAcp;
    uint32_t lcp = h->cpOfLcid(h->user, lcid);
    if (lcp == 0xffffffffu) return 0;
    if (lcp == cp) return (uint32_t)h->lcMapA(h->user, lcid, flags, src, srcLen, dst, dstLen);
    uint32_t n = (uint32_t)srcLen;
    char* in = h->convert(h->user, cp, lcp, src, &n, nullptr, 0);
    if (!in) return 0;
    uint32_t r = 0; char* out = nullptr;
    uint32_t m = (uint32_t)h->lcMapA(h->user, lcid, flags, in, (int32_t)n, nullptr, 0);
    if (m != 0) {
        char* buf = (char*)calloc(m, 1);
        if (buf) {
            m = (uint32_t)h->lcMapA(h->user, lcid, flags, in, (int32_t)n, buf, (int32_t)m);
            if (m != 0) {
                out = h->convert(h->user, lcp, cp, buf, &m, dst, dstLen);
                r = out ? m : 0;
            }
            free(buf);
        }
    }
    free(in);
    if (out && out != dst) free(out);
    return r;
}
