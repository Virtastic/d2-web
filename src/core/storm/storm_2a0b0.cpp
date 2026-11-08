// src/core/storm/storm_2a0b0.cpp -- 1.13c Storm+0x2A0B0, SFileOpenArchive(name, priority, flags, out) (stdcall, 4 words, ordinal 266).
// Prior art used: the decompile and the disassembly only. Opens an MPQ read-only, finds its header on a 0x200 boundary through 0x1000-byte
// windows, reads and decrypts the hash and block tables, links the archive record into the priority-ordered list and returns 1.
//
// Every callee, the kernel32 calls and the module words are seams, so a probe can record them. A callee that retail takes a module string
// as is given its RVA (the wrapper maps it back to the string): 0x46FA0 ":\", 0x46F9C "\\", 0x46F8C and 0x46F7C the two table-key names,
// 0x470F4 the SFILE.CPP tag, 0x46838 the "delete" tag. Record fields: +0/+4 list links, +8 name (0x104), +0x10C handle, +0x110 mode
// (3 CD, 1 flag 2, else 0), +0x114 priority, +0x120 block size, +0x124 block buffer, +0x12C header offset, +0x130 header copy (0x20),
// +0x134 block table, +0x138 hash table, +0x13C decrypt state, +0x140 = 1.
#include <cstdint>
#include <cstring>

struct Storm2A0B0Env {
    uint32_t* g45388;      // 0x55388 last error word
    uint32_t* g43120;      // 0x53120 archive system initialised
    uint32_t* g45984;      // 0x55984 list head word
    uint32_t* g45980;      // 0x55980 list anchor word (its address is the insert anchor when no node is lower)
    void (*setLastError)(uint32_t code);
    void (*enter)();       // EnterCriticalSection(0x55988)
    void (*leave)();
    void (*init)(uint32_t eax);                                         // 0x27FC0, EAX = 0x100001
    void (*copy)(uint32_t dst, uint32_t src, uint32_t n);               // 0x3B080 stdcall
    uint32_t (*getAttr)(uint32_t path);                                 // GetFileAttributesA
    uint32_t (*strstr)(uint32_t hay, uint32_t needleRva);               // 0x6460 cdecl
    void (*sub26BD0)(uint32_t name, uint32_t buf, uint32_t zero);       // 0x26BD0 EAX = name, stdcall (buf, 0)
    uint32_t (*sub26750)(uint32_t path);                                // 0x26750 EAX = path buffer, returns the CD word
    uint32_t (*createFile)(uint32_t path);                              // CreateFileA(path, 0x80000000, 1, 0, 3, 0, 0)
    uint32_t (*sub40D0)(uint32_t listRva, uint32_t a, uint32_t b);      // 0x40D0 stdcall: the new archive record
    uint32_t (*alloc)(uint32_t size, uint32_t tagRva, uint32_t line, uint32_t flags);   // 0x1C580
    void (*release)(uint32_t p, uint32_t tagRva, uint32_t line, uint32_t flags);        // 0x1B960
    void (*setFilePointer)(uint32_t h, uint32_t off);                   // SetFilePointer(h, off, 0, 0)
    void (*readFile)(uint32_t h, uint32_t buf, uint32_t n, uint32_t* got);   // ReadFile(h, buf, n, got, 0)
    void (*sub3E20)(uint32_t rec);                                      // 0x3E20 stdcall
    void (*sub26D70)(uint32_t state, uint32_t buf, uint32_t size, uint32_t name, uint32_t esi, uint32_t edi);   // 0x26D70, ESI = file offset, EDI = a local
    uint32_t (*sub25ED0)(uint32_t nameRva, uint32_t type);              // 0x25ED0 EAX = name, stdcall (type)
    void (*sub25F30)(uint32_t key, uint32_t size, uint32_t table);      // 0x25F30 ECX = key, EAX = size, EDX = table
    void (*sub25E50)(uint32_t table, uint32_t count);                   // 0x25E50 EAX = table, stdcall (count)
    void (*sub29560)(uint32_t mode);                                    // 0x29560 EAX = mode, no stack words
    void (*sub26CB0)(uint32_t rec);                                     // 0x26CB0 stdcall
    void (*sub29F60)(uint32_t rec, uint32_t table, uint32_t count);     // 0x29F60 ECX = rec, stdcall (table, count)
};

namespace {
inline uint32_t& W32(uint32_t base, uint32_t off) { return *(uint32_t*)(uintptr_t)(base + off); }
inline uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kTag = 0x470F4, kDelete = 0x46838;
}

extern "C" uint32_t Storm13c_2A0B0(const Storm2A0B0Env* e, uint32_t name, uint32_t prio, uint32_t flags, uint32_t out) {
    uint32_t ok = 0xFFFFFFFFu;
    if (out == 0) ok = 0; else W32(out, 0) = 0;
    if (name == 0 || *(const uint8_t*)(uintptr_t)name == 0) ok = 0;   // retail reads the first byte unconditionally
    if (ok == 0) {
        *e->g45388 = 0x57;
        e->setLastError(0x57);
        return 0;
    }
    e->enter();
    if (*e->g43120 == 0) e->init(0x100001);
    uint8_t path[0x104];
    std::memset(path, 0, sizeof path);
    e->copy(A(path), name, 0x104);
    path[0x103] = 0;
    const uint32_t attr = e->getAttr(A(path));
    if (!(~(attr >> 4) & 1)) {   // a directory (or a failed query): the path is resolved against the install directory
        if (*(const uint8_t*)(uintptr_t)name == 0x5C || e->strstr(name, 0x46FA0) != 0 || e->strstr(name, 0x46F9C) != 0)
            e->copy(A(path), name, 0x104);
        else
            e->sub26BD0(name, A(path), 0);
    }
    const uint32_t cd = e->sub26750(A(path));
    const uint32_t flag0 = flags & 1;
    if (flag0 && cd == 0) {
        *e->g45388 = 0xF;
        e->setLastError(0xF);
        e->leave();
        return 0;
    }
    const uint32_t h = e->createFile(A(path));
    if (h == 0xFFFFFFFFu) {
        e->leave();
        return 0;
    }
    const uint32_t rec = e->sub40D0(0x5597C, 0, 0);
    e->copy(rec + 8, A(path), 0x104);
    W32(rec, 0x114) = prio;
    W32(rec, 0x10C) = h;
    W32(rec, 0x140) = 1;
    if (cd != 0) W32(rec, 0x110) = 3;
    else if (flags & 2) W32(rec, 0x110) = 1;
    else W32(rec, 0x110) = 0;
    const uint32_t hdr = e->alloc(0x20, kTag, 0x1024, 0);
    W32(rec, 0x130) = hdr;
    W32(rec, 0x12C) = 0;
    const uint32_t win = e->alloc(0x1000, kTag, 0x1029, 0);
    uint32_t base = 0, len = 0;
    for (;;) {
        const uint32_t off = W32(rec, 0x12C);
        if (!(off < len + base)) {
            const uint32_t cand = off;
            e->setFilePointer(h, off);
            uint32_t got = 0;
            e->readFile(h, win, 0x1000, &got);
            if (got < 0x20) {
                e->sub3E20(rec);
                e->release(rec, kDelete, 0xFFFFFFFFu, 0);
                *e->g45388 = 0x8510006C;
                e->setLastError(0x8510006C);
                e->leave();
                e->release(win, kTag, 0x1044, 0);
                return 0;
            }
            base = cand;
            len = got;
        }
        std::memcpy((void*)(uintptr_t)hdr, (const void*)(uintptr_t)(win + (W32(rec, 0x12C) - base)), 0x20);
        if (W32(hdr, 0) == 0x1A51504Du && W32(hdr, 4) >= 0x20) break;   // unsigned compare (jae)
        W32(rec, 0x12C) += 0x200;
    }
    e->release(win, kTag, 0x106B, 0);
    const uint32_t blk = 0x200u << (*(const uint8_t*)(uintptr_t)(hdr + 0xE) & 31);
    W32(rec, 0x120) = blk;
    W32(rec, 0x124) = e->alloc(blk, kTag, 0x1070, 0);
    W32(rec, 0x138) = e->alloc(W32(hdr, 0x18) << 4, kTag, 0x1079, 0);
    uint32_t edi = 0;   // retail's EDI is the address of a local; the address is all the callee is handed
    e->sub26D70(rec + 0x13C, W32(rec, 0x138), W32(hdr, 0x18) << 4, rec + 8, W32(hdr, 0x10) + W32(rec, 0x12C), A(&edi));
    const uint32_t k1 = e->sub25ED0(0x46F8C, 3);
    e->sub25F30(k1, W32(hdr, 0x18) << 4, W32(rec, 0x138));
    W32(rec, 0x134) = e->alloc(W32(hdr, 0x1C) * 0x1C, kTag, 0x1092, 0);
    e->sub26D70(rec + 0x13C, W32(rec, 0x134), W32(hdr, 0x1C) << 4, rec + 8, W32(hdr, 0x14) + W32(rec, 0x12C), A(&edi));
    const uint32_t k2 = e->sub25ED0(0x46F7C, 3);
    e->sub25F30(k2, W32(hdr, 0x1C) << 4, W32(rec, 0x134));
    e->sub25E50(W32(rec, 0x134), W32(hdr, 0x1C));
    if (W32(rec, 0x12C) != 0) {
        for (uint32_t i = 0; i < W32(hdr, 0x1C); ++i) {
            const uint32_t at = W32(rec, 0x134) + i * 0x1C;
            if (W32(at, 0) != 0) W32(at, 0) += W32(rec, 0x12C);
        }
    }
    // the priority-ordered insert: the first node whose priority is not above ours, else the anchor word
    uint32_t anchor = 0;
    if ((int32_t)*e->g45984 > 0) {
        uint32_t node = *e->g45984;
        for (;;) {
            if ((int32_t)W32(node, 0x114) <= (int32_t)prio) { anchor = node; break; }
            node = W32(node, 4);
            if ((int32_t)node <= 0) break;
        }
    }
    const uint32_t link = W32(rec, 0);
    if (link != 0) {
        const uint32_t tag = W32(rec, 4);
        uint32_t prev;
        if ((int32_t)tag < 0) prev = ~tag;
        else prev = tag + (rec - W32(link, 4));
        W32(prev, 0) = link;
        W32(W32(rec, 0), 4) = W32(rec, 4);
        W32(rec, 0) = 0;
        W32(rec, 4) = 0;
    }
    if (anchor == 0) anchor = A(e->g45980);
    const uint32_t head = W32(anchor, 0);
    W32(rec, 0) = head;
    W32(rec, 4) = W32(head, 4);
    W32(head, 4) = rec;
    W32(anchor, 0) = rec;
    const uint32_t mode = W32(rec, 0x110);
    if (mode != 0) {
        e->sub29560(mode);
    } else if (flag0 != 0 && cd == 0) {
        e->sub26CB0(rec);
        *e->g45388 = 0xF;
        e->setLastError(0xF);
        e->leave();
        return 0;
    }
    W32(out, 0) = rec;
    e->leave();
    e->sub29F60(rec, W32(rec, 0x134), W32(W32(rec, 0x130), 0x1C));
    return 1;
}
