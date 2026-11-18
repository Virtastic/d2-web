// 1.13c Storm+0x27560 (#251) -- verify the archive signature through advapi32.
// Retail records keep 32-bit addresses; every host call (lookup, LoadLibraryA, the nine advapi32 entries, the mapping and the
// BLIZZARDKEY resource) is a seam in Storm27560Env, so the wasm target needs no advapi32.
#include <cstdint>
#include <cstring>

struct Storm27560Env {
    uint32_t (*find)(void* archive, uint32_t name_rva, uint32_t locale);                // 0x26AC0
    void (*set_last_error)(uint32_t code);                                              // SetLastError + DAT_6FC45388
    uint32_t (*load_library)(const char* name);
    uint32_t (*get_proc)(uint32_t module, const char* name);
    uint32_t (*free_library)(uint32_t module);
    uint32_t* (*mem_alloc)(uint32_t size, const char* file, uint32_t line, uint32_t flags);  // 0x1C580
    void (*mem_free)(void* p, const char* file, uint32_t line);                          // 0x1B960
    uint32_t (*crypt)(uint32_t fn, uint32_t n, const uint32_t* a);                       // call the resolved advapi32 word with n args
    uint32_t (*create_mapping)(uint32_t file, uint32_t prot);
    uint32_t (*map_view)(uint32_t mapping, uint32_t access);
    uint32_t (*unmap_view)(uint32_t view);
    uint32_t (*close_handle)(uint32_t h);
    uint32_t (*find_resource)(const char* name, const char* type);
    uint32_t (*load_resource)(uint32_t res);
    uint32_t (*lock_resource)(uint32_t g);
    uint32_t (*sizeof_resource)(uint32_t res);
    uint32_t (*free_resource)(uint32_t g);
};

namespace {
const char kSfile[] = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";
const char kProv[] = "Microsoft Base Cryptographic Provider v1.0";
const char kCont[] = "Blizzard_Storm";
uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
uint32_t Lo(const void* p) { return (uint32_t)(uintptr_t)p; }
const uint8_t* N(uint32_t a) { return (const uint8_t*)(uintptr_t)a; }
}

extern "C" uint32_t Storm_Sub27560(uint8_t* ar, uint32_t* out, const Storm27560Env& e) {
    static const char* const kFn[9] = { "CryptAcquireContextA", "CryptCreateHash", "CryptDestroyHash", "CryptDestroyKey", "CryptHashData",
                                        "CryptImportKey", "CryptReleaseContext", "CryptSignHashA", "CryptVerifySignatureA" };
    if (out) *out = 0;
    uint32_t idx = e.find(ar, 0, 0);
    if (idx == 0xffffffffu) {
        if (out) *out = 1;
        e.set_last_error(0x4dc);
        return 0;
    }
    uint32_t* tbl = nullptr;
    uint32_t hash = 0, key = 0, ctx = 0, file = 0, view = 0, result = 0;
    const uint32_t mod = e.load_library("advapi32.dll");
    do {
        if (!mod) break;
        tbl = e.mem_alloc(0x24, kSfile, 0xb51, 0);
        bool ok = true;
        for (int i = 0; i < 9 && ok; i++) { tbl[i] = e.get_proc(mod, kFn[i]); ok = tbl[i] != 0; }
        if (!ok) break;
        { const uint32_t a[5] = { Lo(&ctx), Lo(kCont), Lo(kProv), 1, 0 };
          if (e.crypt(tbl[0], 5, a) == 0) { const uint32_t b[5] = { Lo(&ctx), Lo(kCont), Lo(kProv), 1, 8 }; if (e.crypt(tbl[0], 5, b) == 0) break; } }
        file = e.create_mapping(rd(ar, 0x10c), 0x8000002);
        if (!file) break;
        view = e.map_view(file, 4);
        if (!view) break;
        idx = e.find(ar, 0, 0);
        if (idx == 0xffffffffu) break;
        const uint8_t* ent = N(rd(ar, 0x134)) + rd(N(rd(ar, 0x138)), 0xc + idx * 0x10) * 0x1c;
        const uint32_t pos = rd(ent, 0), size = rd(ent, 8), base = rd(ar, 0x12c);
        if ((rd(ent, 0xc) & 0x1ff00) == 0 && size > 8 && pos != 0) {
            const uint32_t type = rd(N(view), pos);
            if (type != 0) { if (out) *out = 3; break; }
            const uint32_t res = e.find_resource("BLIZZARDKEY", "#256");
            const uint32_t g = e.load_resource(res);
            const uint32_t lk = e.lock_resource(g);
            if (lk) { const uint32_t a[6] = { ctx, lk, e.sizeof_resource(res), 0, 0, Lo(&key) }; e.crypt(tbl[5], 6, a); }
            e.free_resource(g);
            if (key == 0) break;
            { const uint32_t a[5] = { ctx, 0x8003, 0, 0, Lo(&hash) }; if (e.crypt(tbl[1], 5, a) == 0) break; }
            { const uint32_t a[4] = { hash, base + view, pos - base, 0 }; if (e.crypt(tbl[4], 4, a) == 0) break; }
            uint32_t* buf = e.mem_alloc(size, kSfile, 0xba7, 8);
            const uint32_t a[4] = { hash, Lo(buf), size, 0 };
            const uint32_t hr = e.crypt(tbl[4], 4, a);
            e.mem_free(buf, kSfile, 0xbac);
            if (hr == 0) break;
            const uint32_t total = rd(N(rd(ar, 0x130)), 8), after = pos + size;
            if (after < total + base) {
                const uint32_t b[4] = { hash, after + view, ((total - pos) - size) + base, 0 };
                if (e.crypt(tbl[4], 4, b) == 0) break;
            }
            const uint32_t v[6] = { hash, pos + 8 + view, size - 8, key, 0, 0 };
            if (e.crypt(tbl[8], 6, v) != 0) { result = 1; if (out) *out = 5; break; }  // status table 0x35CE4[0] = 5
        }
        if (out) *out = 2;
    } while (false);
    if (hash) { const uint32_t a[1] = { hash }; e.crypt(tbl[2], 1, a); }
    if (key) { const uint32_t a[1] = { key }; e.crypt(tbl[3], 1, a); }
    if (view) e.unmap_view(view);
    if (file) e.close_handle(file);
    if (ctx) {
        const uint32_t a[2] = { ctx, 0 }; e.crypt(tbl[6], 2, a);
        const uint32_t b[5] = { Lo(&hash), Lo(kCont), Lo(kProv), 1, 0x10 }; e.crypt(tbl[0], 5, b);
    }
    if (tbl) e.mem_free(tbl, kSfile, 0xbd5);
    if (mod) e.free_library(mod);
    if (!result) e.set_last_error(0x4dc);
    return result;
}
