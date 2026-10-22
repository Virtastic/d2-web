// Storm batch b65. Retail globals/callees/Win32 calls are explicit hook params; no SEH frames modelled.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <initializer_list>

namespace stormb65 {
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// 1.13c Storm+0x29DF0
// Hidden ESI = f (file record). Hooks: crit lock/unlock, read8 (sub_295B0 of 8 bytes), seek (SetFilePointer, method 1),
// memSize (*(f+0x114)+8).
struct SeekEnv {
    bool (*read8)(uint8_t* f, uint32_t out[2]);
    void (*lock)(); void (*unlock)();
    uint32_t (*seek)(uint32_t handle, int32_t dist, int method);
    uint32_t (*memSize)(uint8_t* f);
};
static uint32_t relSeek(const SeekEnv& e, uint8_t* f, int32_t dist, bool clampLow) {
    uint32_t r;
    e.lock();
    if (rd(f, 0x10c) == 0xffffffffu) {
        uint32_t pos = rd(f, 0x11c);
        if (clampLow) pos = (dist < 0 && pos < (uint32_t)-dist) ? 0 : pos + (uint32_t)dist;
        uint32_t last = e.memSize(f) - 1;
        if (pos > last) pos = last;
        wr(f, 0x11c, pos); wr(f, 0x138, 0); wr(f, 0x13c, 0);
        r = pos;
        e.unlock();
    } else { e.unlock(); r = e.seek(rd(f, 0x10c), dist, 1); }
    return r;
}
bool sub_29DF0(const SeekEnv& e, uint8_t* f, uint32_t tag, int32_t* out) {
    uint32_t h[2];
    bool ok = e.read8(f, h);
    while (true) {
        if (!ok) return false;
        if (h[0] == tag) break;
        if (relSeek(e, f, (int32_t)h[1], true) == 0xffffffffu) return false;
        ok = e.read8(f, h);
    }
    out[0] = (int32_t)h[1];
    uint32_t r = relSeek(e, f, 0, false);
    out[1] = (int32_t)r;
    return r != 0xffffffffu;
}

// 1.13c Storm+0x27EF0
// List node access via hooks (64-bit pointers cannot live in the 32-bit node words).
struct NodeEnv {
    void (*lock)(); void (*unlock)();
    void* (*head)();
    void* (*next)(void* n);            // node[1]
    uint32_t (*word)(void* n, int i);  // node[i]
    void (*recycle)(void* n);          // event pool alloc (sub_4290) then ev[2]=node[2]
    void (*unlink)(void* n);           // sub_4040
    void (*release)(void* n);          // sub_1B960 "AUREQUEST"
};
uint32_t sub_27EF0(const NodeEnv& e, int32_t a, int32_t b) {
    if (a == 0 && b == 0) return 0;
    e.lock();
    uint32_t found = 0;
    void* n = e.head();
    while (n) {
        if ((a != 0 && (int32_t)e.word(n, 9) == a) || (b != 0 && (int32_t)e.word(n, 10) == b)) {
            found = 1;
            if (e.word(n, 2) != 0) e.recycle(n);
            void* nx = e.next(n);
            e.unlink(n); e.release(n);
            n = nx;
        } else n = e.next(n);
    }
    e.unlock();
    return found;
}

// 1.13c Storm+0x25DC0
// Hidden ESI = o: u32 fields at 0x24,0x2c(out),0x30,0x40. iface = *(o+0x3c); call = vtable slot +0x10 (iface,&a,&b).
// Returns iface, or nullptr on failure (retail 0xffffffff).
void* sub_25DC0(uint8_t* o, void* iface, int (*call)(void*, uint32_t*, uint32_t*), uint32_t ifaceAddr32) {
    uint32_t a = 0, b = 0;
    if (call(iface, &a, &b) < 0) return nullptr;
    int32_t d = (int32_t)(rd(o, 0x30) % rd(o, 0x40) - ifaceAddr32);
    if (d < 1) d += (int32_t)rd(o, 0x40);
    uint32_t v = rd(o, 0x30) - (uint32_t)d;
    if (rd(o, 0x24) <= v) v = rd(o, 0x24);
    wr(o, 0x2c, v);
    return iface;
}

// 1.13c Storm+0x04040
// Releases two refcounted members (param[3] count at +0x50 dwords, param[0xc] count at +0x13), then base dtor.
struct RefEnv { void (*freeA)(); void (*freeB)(); void (*base)(); };
void sub_04040(const RefEnv& e, int32_t* refA, int32_t* refB) {
    if (refA && --*refA == 0) e.freeA();
    if (refB && --*refB == 0) e.freeB();
    e.base();
}

// 1.13c Storm+0x040D0
// Hidden EDI = link; alloc = sub_1C580(size, tag, -2, flags), tag "AUARCHIVEREC"; link = sub_4390.
uint32_t* sub_040D0(uint32_t* (*alloc)(uint32_t, int32_t, uint32_t), void (*link)(uint32_t*, int), int edi, uint32_t base, uint32_t flags) {
    uint32_t* p = alloc(base + 0x144, -2, flags | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (edi) link(p, edi);
    return p;
}

// 1.13c Storm+0x04290
// Same as 0x040D0 with size base+0xc, tag "AUEVENTREC".
uint32_t* sub_04290(uint32_t* (*alloc)(uint32_t, int32_t, uint32_t), void (*link)(uint32_t*, int), int edi, uint32_t base, uint32_t flags) {
    uint32_t* p = alloc(base + 0xc, -2, flags | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (edi) link(p, edi);
    return p;
}

// 1.13c Storm+0x06460
// strstr; one-char needle goes to sub_9E26 (strchr).
const char* sub_06460(const char* hay, const char* needle) {
    if (!*needle) return hay;
    if (!needle[1]) return std::strchr(hay, *needle);
    return std::strstr(hay, needle);
}

// 1.13c Storm+0x26750
// Hidden EAX = path (first 4 chars used as root). Hooks wrap GetDriveTypeA / GetVolumeInformationA / GetDiskFreeSpaceA.
struct DriveEnv {
    uint32_t (*driveType)(const char* root);
    bool (*volInfo)(const char* root, uint32_t* fsFlags, uint32_t* fsNameFirst4);
    bool (*freeSpace)(const char* root, uint32_t* secPerClus, uint32_t* bytesPerSec, uint32_t* freeClus, uint32_t* total);
};
uint32_t sub_26750(const DriveEnv& e, const char* path) {
    char root[8] = {0};
    std::strncpy(root, path, 3);
    uint32_t t = e.driveType(root), flags = 0, name0 = 0;
    if (!e.volInfo(root, &flags, &name0)) return 0;
    uint32_t spc = 0, bps = 0, fc = 0, tc = 0;
    if (!e.freeSpace(root, &spc, &bps, &fc, &tc)) return 0;
    uint32_t x = (flags & 4) ^ name0 ^ fc ^ bps ^ t;
    uint16_t w = (uint16_t)(x >> 16) ^ (uint16_t)x;
    return w == 0x1f00 || w == 0x805;
}

// 1.13c Storm+0x26BD0
// Hidden EAX = name. base = DAT_6fc43000 (initialised via sub_26860 by ensureBase); fullpath = _fullpath(out, in, 260).
void sub_26BD0(char* out, int32_t stripDir, const char* name, const char* (*ensureBase)(), void (*fullpath)(char*, const char*)) {
    const char* base = ensureBase();
    if (stripDir) { const char* s = std::strrchr(name, '\\'); if (s) name = s + 1; }
    char tmp[260];
    std::snprintf(tmp, sizeof tmp, "%s%s", base, name);
    fullpath(out, tmp);
}

// 1.13c Storm+0x26D70
// Hidden: ESI = pos (-1 = no seek), EBX = handle, EDI = bytesRead out. mode = DAT_6fc41a04 (0 fail, 1 retry cb, 2 report).
struct ReadEnv {
    void (*seek)(void* h, int32_t pos);
    bool (*read)(void* h, void* buf, uint32_t n, uint32_t* got);
    uint32_t (*lastError)();
    int mode;
    int (*retry)(uint32_t* ctx, uint32_t err, int attempt); // DAT_6fc4310c, may be null
    void (*report)(uint32_t err, uint32_t* ctx);            // sub_1F590(err, ctx, -4, 0, 0, 1)
};
bool sub_26D70(const ReadEnv& e, void* h, int32_t pos, int32_t* posOut, void* buf, uint32_t n, uint32_t* got, uint32_t* ctx) {
    int attempt = 0;
    for (;;) {
        if (pos != -1 && pos != *posOut) e.seek(h, pos);
        bool ok = e.read(h, buf, n, got);
        *posOut = (int32_t)(*got + (uint32_t)pos);
        if (ok) return *got == n;
        uint32_t err = e.lastError();
        if (e.mode == 0) return false;
        if (e.mode == 1) {
            if (!e.retry) return false;
            int r = e.retry(ctx, err, attempt++);
            if (r == 0) return false;
        } else if (e.mode == 2) e.report(err, ctx);
    }
}

// 1.13c Storm+0x29F60
// thiscall this; entries = param_1 base, stride 0x1c; count = param_2.
struct AttrEnv {
    bool (*open)(void* self, const char* name, void** rec);          // sub_18960
    uint32_t (*fileSize)(void* rec);
    void* (*alloc)(uint32_t n);
    bool (*read)(void* rec, void* buf, uint32_t n);                  // sub_195B0
    void (*free)(void* p);
    void (*close)(void* rec);                                        // sub_16E20
};
uint32_t sub_29F60(const AttrEnv& e, void* self, uint8_t* entries, uint32_t count) {
    void* rec;
    if (!e.open(self, "(attributes)", &rec)) return 0;
    uint32_t size = e.fileSize(rec), ok = 0;
    uint8_t* buf = (uint8_t*)e.alloc(size);
    if (e.read(rec, buf, size) && size > 7) {
        uint32_t flags = rd(buf, 4), want = 8;
        if (flags & 1) want = count * 4 + 8;
        if (flags & 2) want += count * 8;
        if (size == want) {
            const uint8_t* s = buf + 8;
            if ((flags & 1) && count) for (uint32_t i = 0; i < count; ++i, s += 4) wr(entries, 0x60 + i * 0x1c, rd(s, 0));
            if ((flags & 2) && count) for (uint32_t i = 0; i < count; ++i, s += 8) { wr(entries, 0x40 + i * 0x1c, rd(s, 0)); wr(entries, 0x44 + i * 0x1c, rd(s, 4)); }
            ok = 1;
        }
    }
    if (buf) e.free(buf);
    e.close(rec);
    return ok;
}

// 1.13c Storm+0x26ED0
// Hidden EAX = file record (errctx at +8). readAt = sub_26D70 with pos -1.
bool sub_26ED0(bool (*readAt)(int32_t* pos, void* buf, uint32_t n, uint32_t* ctx), uint8_t* rec, void* buf, uint32_t n, uint32_t* out) {
    int32_t pos = -1;
    readAt(&pos, buf, n, (uint32_t*)(rec + 8));
    if (out) *out = 0;
    return n == 0;
}
}
