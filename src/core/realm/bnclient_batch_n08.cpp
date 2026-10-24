// BNClient batch n08: the BNCS thread/queue glue (rows 0x0D320 0x0E120 0x0F240 0x0F3C0 0x0FE00 0x0FEC0
// 0x12000 0x185E0). Transcribed from the cached decompile of retail 1.13c Bnclient.dll. Retail is x86-32 and
// reaches Win32, Storm and Fog through imports; here each is a function pointer of BnN08Host (the port's seam),
// and retail's DAT_ globals are the BnN08State the caller owns. Register-passed arguments (EAX/ESI/EBX) are
// ordinary parameters.
#include <cstdint>
#include <cstring>

struct BnN08Host {
    void (*sleep_ms)(uint32_t ms);
    uint32_t (*tick_ms)(void);
    void* (*alloc)(uint32_t size, int tag);             // Fog #10042 (queue node) / Storm #401
    void (*message)(uint32_t code);                      // Fog #10265 + #10024, then the caller's _exit(-1)
    void (*terminate)(int code);                         // _exit
    void* (*create_thread)(void* fn, void* arg);         // CreateThread / __beginthreadex
    int (*thread_running)(void* handle);                 // WaitForSingleObject(h,0) != 0
    void (*lock)(void);                                  // EnterCriticalSection(DAT_6ff3f1dc)
    void (*unlock)(void);
};

// DAT_6ff3f64c/50: head/tail of the singly linked packet queue; node +0x00 id, +0x04 data[0x120],
// +0x124 length, +0x128 next.
struct BnN08Node { uint32_t id; char data[0x120]; uint32_t length; uint32_t next_ptr_unused; };
struct BnN08State {
    uint8_t flag_f1f8; uint8_t done_f660; void* arg_f500; uint32_t arg_f504;
    void* queue_head; void* queue_tail;
    int32_t retry_cb_result;
};
static uint32_t& node_len(void* n) { return *(uint32_t*)((char*)n + 0x124); }
static void*& node_next(void* n) { return *(void**)((char*)n + 0x128); }

// shared tail of 0x0FE00 / 0x0FEC0: stamp, bounds-check, append under the lock.
static bool bn_n08_enqueue(BnN08State& s, const BnN08Host& h, void* n, uint32_t id, uint32_t len, uint32_t errcode) {
    *(uint32_t*)n = id; node_len(n) = len; node_next(n) = nullptr;
    if (len < 0x121) {
        h.lock();
        void* first = n;
        if (s.queue_head) { node_next(s.queue_tail) = n; first = s.queue_head; }
        s.queue_head = first; s.queue_tail = n;
        h.unlock();
        return true;
    }
    h.message(errcode);
    h.terminate(-1);
    return false;
}

// ponytail: `src` is the caller's string (retail reads it from the return-address slot); copies 0x120 bytes.
// 1.13c BNClient+0x0FE00 -- queue a NUL-terminated string (<= 0x120 bytes with the NUL) as node id 0x5f.
bool bn_queue_string(BnN08State& s, const BnN08Host& h, const char* src) {
    void* n = h.alloc(0x5f, 0);
    char* d = (char*)n + 4; memcpy(d, src, 0x120);
    uint32_t len = (uint32_t)strlen(d) + 1;
    return bn_n08_enqueue(s, h, n, 0x5f, len, 0x65);
}

// but keeps the stored length, which then fails the bound and takes the fatal tail (code 0x47).
// 1.13c BNClient+0x0FEC0 -- queue `len` raw bytes as node id `id` (EBX); len > 0x120 clamps the copy to 0x120
bool bn_queue_bytes(BnN08State& s, const BnN08Host& h, uint32_t len, const void* src, uint32_t id) {
    void* n = h.alloc(0x3a, 0);
    uint32_t c = len;
    if (len < 0x121) {
        if (len == 0) *((char*)n + 4) = 0;
    } else c = 0x120;
    if (c) memcpy((char*)n + 4, src, c);
    return bn_n08_enqueue(s, h, n, id, len, 0x47);
}

// finish or for done_f660 to be set, polling every 10 ms.
// 1.13c BNClient+0x0F240 -- start the worker (entry FUN_6ff2f0a0, `entry`) on {buf, p1} and wait for it to
void bn_start_worker(BnN08State& s, const BnN08Host& h, uint8_t* buf, uint32_t p1, void* entry) {
    s.flag_f1f8 = 1;
    h.sleep_ms(10);
    *buf = 0;
    s.arg_f504 = p1; s.arg_f500 = buf;
    void* th = h.create_thread(entry, &s.arg_f500);
    while (s.done_f660 == 0 && h.thread_running(th)) h.sleep_ms(10);
}

// copy = Ordinal_501 (SStrCopy), reg = Ordinal_422 (returns the override string or null).
struct BnN08GwHost {
    void (*load)(void); const char* (*nth)(void); void (*copy)(char* dst, const char* src);
    const char* (*reg)(void);
};
// 1.13c BNClient+0x0F3C0 -- ESI = out buffer. Seams: load = BNGatewayAccess::Load, nth = Nth(&gw, current),
void bn_pick_gateway_string(char* out, const BnN08GwHost& g) {
    *out = 0;
    g.load();
    g.copy(out, g.nth());
    if (const char* r = g.reg()) g.copy(out, r);
}

// at DAT_6ff3ecb8; on success copy 0x14 bytes of DAT_6ff3ed40 to a local and call FUN_6ff2eb00(0x14,0x54).
// 1.13c BNClient+0x12000 -- __fastcall(param_1): r = FUN_6ff2b3a0(&DAT_6ff3f8e4, param_1); zero the 0x20 bytes
uint32_t bn_sub_12000(void* param_1, int (*f2b3a0)(void*, void*), void* f8e4, void* ecb8, const void* ed40,
                      void (*feb00)(const void*, uint32_t, uint32_t)) {
    int r = f2b3a0(f8e4, param_1);
    memset(ecb8, 0, 0x20);
    if (r) { uint8_t tmp[0x14]; memcpy(tmp, ed40, 0x14); feb00(tmp, 0x14, 0x54); return 1; }
    return 0;
}

// 1.13c BNClient+0x185E0 -- SEH unwind funclet: calls FUN_6ff21160.
void bn_unwind_185e0(void (*f21160)(void)) { f21160(); }

// Seams: stat = FUN_6ff34050(path,&ft,&buf,&len); cmp = CompareFileTime; run = FUN_6ff2e2e0; begin = __beginthreadex.
struct BnN08VerHost {
    int (*stat)(const char* path, uint32_t ft[2], void** buf, uint32_t* len);
    int (*cmp)(const uint32_t a[2], const uint32_t b[2]);
    void (*run)(const char* path, const char* extra, void* buf, uint32_t len);
    void (*free_buf)(void*);
    void* (*alloc)(uint32_t size);
    void (*begin)(void* block);                          // thread entry FUN_6ff2dc50 + CloseHandle
    void (*copy)(void* dst, const void* src, uint32_t n);
    void (*unlock_d280)(void);
    void (*inc_f66c)(void);
};
// 1.13c BNClient+0x0E120 -- spawn the version-check worker unless the file time already matches.
void bn_version_check(const BnN08VerHost& h, uint8_t flags, const uint32_t cur_ft[2], const char* path,
                      const char* extra) {
    if (!path || !*path) return;
    if (!extra) extra = "";
    uint32_t ft[2]; void* buf = nullptr; uint32_t len = 0;
    if (h.stat(path, ft, &buf, &len)) {
        if (h.cmp(cur_ft, ft) == 0) {
            if (flags & 2) h.unlock_d280();
            h.run(path, extra, buf, len);
            if (buf) h.free_buf(buf);
            return;
        }
        if (buf) h.free_buf(buf);
    }
    uint8_t* blk = (uint8_t*)h.alloc(0x460);
    if (!blk) return;
    memset(blk, 0, 0x460);
    *(uint16_t*)blk = (uint16_t)(strlen(path) + 0x21 + 1 - 1);
    blk[3] = 1;
    ((uint16_t*)blk)[2] = 0x3836; ((uint16_t*)blk)[3] = 0x4958; ((uint16_t*)blk)[4] = 0x4456; ((uint16_t*)blk)[5] = 0x4432;
    ((uint16_t*)blk)[6] = (uint16_t)ft[1]; ((uint16_t*)blk)[7] = (uint16_t)(ft[1] >> 16);
    ((uint16_t*)blk)[0xc] = (uint16_t)cur_ft[0]; ((uint16_t*)blk)[0xd] = (uint16_t)(cur_ft[0] >> 16);
    ((uint16_t*)blk)[0xe] = (uint16_t)cur_ft[1]; ((uint16_t*)blk)[0xf] = (uint16_t)(cur_ft[1] >> 16);
    ((uint16_t*)blk)[0x116] = (uint16_t)ft[0]; ((uint16_t*)blk)[0x117] = (uint16_t)(ft[0] >> 16);
    h.copy(blk + 0x20, path, 0x7fffffff);
    h.copy(blk + 0x124, extra, 0x104);
    h.inc_f66c();
    h.begin(blk);
}

// load it and call its CheckRevision(...) with the bnclient.dll / d2client.dll sibling paths. Returns 1 on
// timeout, 0 if the file never resolved, else CheckRevision's return. All loader work is the `cr` seam.
struct BnN08CheckRev { const char* (*pending_path)(void); int (*resolve)(const char* path); 
                       void* (*run)(uint32_t* o1, uint32_t* o2, uint8_t* o3); void (*cleanup)(const char* path); };
// 1.13c BNClient+0x0D320 -- poll (10 ms, <= 20001 ms) for the CheckRevision temp DLL path in DAT_6ff3f510, then
void* bn_check_revision(uint32_t* o1, uint32_t* o2, uint8_t* o3, const BnN08Host& h, const BnN08CheckRev& c) {
    if (o1) *o1 = 0;
    if (o2) *o2 = 0;
    if (o3) *o3 = 0;
    uint32_t t0 = h.tick_ms();
    do {
        const char* p = c.pending_path();
        if (p && *p) {
            if (!c.resolve(p)) return nullptr;
            void* r = c.run(o1, o2, o3);
            h.sleep_ms(0x32);
            c.cleanup(p);
            return r;
        }
        h.sleep_ms(10);
    } while (h.tick_ms() - t0 < 0x4e21);
    return (void*)1;
}
