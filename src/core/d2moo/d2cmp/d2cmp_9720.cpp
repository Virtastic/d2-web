// D2CMP #10003 (1.13c D2CMP+0x9720): load or find a DT1 tile library, transcribed from the disassembly. Raw 32-bit-layout byte offsets; the callees are injected.
// Retail convention: stdcall(outHash, name), ret 8. Nothing is kept in registers across the callees except as the seams note.
//   find   = 0x9690 (stack name; walks the list at [0x6FE88328], next at +0x10C, exact strcmp of the name at +0)
//   fsize  = 0xBAD0 (EAX name; the library file's size: nTiles * 0x60 + 0x114)
//   alloc  = Fog 10042 via 0x8D7E (ECX size, EDX file string, stack line, 0; returns the block)
//   open   = Storm 10102 via 0x8DB4 (ECX name, EDX &handle; returns nonzero on success)
//   openErr= GetLastError, then Fog 10029 via 0x8DBA (cdecl "Error opening file: %s", name)
//   readN  = 0x8EF0 (ESI handle, EDI count, stack buffer; ret 4)
//   close  = Storm 10103 via 0x8DC0 (ECX handle)
//   parse  = 0x15660 (EAX name, ECX file buffer, stack &data; ret 4; writes data)
//   hashOf = 0x15C70 (no arguments; returns the tile hash)
// Fatal paths (alloc failures 0x6C / 0x7C, a null handle 0x49) call _exit(-1) in retail and are not reproduced.
#include <cstdint>
#include <cstring>

struct D2Cmp9720Env {
    uint32_t* head;                                                 // [0x6FE88328], the library list head (a record address)
    uint32_t (*find)(const char* name);                             // 0x9690: the record address, or 0
    uint32_t (*fsize)(const char* name);                            // 0xBAD0
    uint8_t* (*alloc)(uint32_t size, uint32_t line);                // 0x8D7E
    uint32_t (*open)(const char* name, uint32_t* handle);           // 0x8DB4
    void (*openErr)(const char* name);                              // 0x8DBA (after GetLastError)
    void (*readN)(uint32_t handle, uint32_t count, uint8_t* buf);   // 0x8EF0
    void (*close)(uint32_t handle);                                 // 0x8DC0
    void (*parse)(const char* name, uint8_t* buf, uint32_t* data);  // 0x15660
    uint32_t (*hashOf)();                                           // 0x15C70
};
static inline uint32_t l9_rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline void l9_wr(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

extern "C" uint32_t d2cmp_9720(uint32_t* outHash, const char* name, const D2Cmp9720Env* e) {
    if (const uint32_t hit = e->find(name)) {
        const uint8_t* r = (const uint8_t*)(uintptr_t)hit;
        *outHash = l9_rd(r, 0x108);
        return l9_rd(r, 0x104);
    }
    const uint32_t size = e->fsize(name);
    uint8_t* buf = e->alloc(size, 0x6b);
    std::memset(buf, 0, size);
    uint32_t handle = 0, data = 0;
    if (!e->open(name, &handle)) e->openErr(name);
    e->readN(handle, size, buf);
    e->close(handle);
    e->parse(name, buf, &data);
    const uint32_t hash = e->hashOf();
    uint8_t* rec = e->alloc(0x110, 0x7b);
    std::memset(rec, 0, 0x110);
    l9_wr(rec, 0x10c, *e->head);
    *e->head = (uint32_t)(uintptr_t)rec;
    l9_wr(rec, 0x104, data);
    l9_wr(rec, 0x108, hash);
    std::strcpy((char*)rec, name);
    *outHash = hash;
    return data;
}
