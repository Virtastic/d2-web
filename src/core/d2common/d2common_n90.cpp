// D2Common CRT + chest-table glue, written from the cached 1.13c decompiles.
// Win32/CRT/Storm callees are reached through function-pointer seams (env structs).
#include <cstdint>

struct D2CommonFile { // VC7.1 FILE, 0x20 bytes, 32-bit layout
    uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag;
    int32_t file; int32_t charbuf; int32_t bufsiz; uint32_t tmpfname;
};

struct D2CommonOpenfileEnv {
    int32_t (*sopen)(const char*, uint32_t oflag, int32_t shflag, int32_t mode); // 0x7887
    uint32_t iob_flag_base; // DAT_6fdef960
    int32_t* open_streams;  // DAT_6fdef76c
};

// 1.13c D2COMMON+0x04354
D2CommonFile* d2common_openfile_4354(const char* name, const char* mode, int32_t shflag,
                                     D2CommonFile* f, const D2CommonOpenfileEnv* e) {
    uint32_t of, fl;
    bool ok = true, seenRS = false, seenCN = false;
    char c = *mode;
    if (c == 'a') { of = 0x109; fl = e->iob_flag_base | 2; }
    else if (c == 'r') { of = 0; fl = e->iob_flag_base | 1; }
    else if (c == 'w') { of = 0x301; fl = e->iob_flag_base | 2; }
    else return nullptr;
    for (;;) {
        c = *++mode;
        if (!c || !ok) break;
        switch (c) {
        case 'T': if (of & 0x1000) ok = false; else of |= 0x1000; break;
        case '+': if (of & 2) ok = false; else { of = (of & ~1u) | 2; fl = (fl & ~3u) | 0x80; } break;
        case 'D': if (of & 0x40) ok = false; else of |= 0x40; break;
        case 'R': if (seenRS) ok = false; else { seenRS = true; of |= 0x10; } break;
        case 'S': if (seenRS) ok = false; else { seenRS = true; of |= 0x20; } break;
        case 'b': if (of & 0xc000) ok = false; else of |= 0x8000; break;
        case 't': if (of & 0xc000) ok = false; else of |= 0x4000; break;
        case 'c': if (seenCN) ok = false; else { seenCN = true; fl |= 0x4000; } break;
        case 'n': if (seenCN) ok = false; else { seenCN = true; fl &= ~0x4000u; } break;
        default: ok = false; break;
        }
    }
    const int32_t fd = e->sopen(name, of, shflag, 0x1a4);
    if (fd < 0) return nullptr;
    ++*e->open_streams;
    f->flag = fl; f->cnt = 0; f->ptr = 0; f->base = 0; f->tmpfname = 0; f->file = fd;
    return f;
}

struct D2CommonGetstreamEnv {
    int32_t nstream;           // DAT_6fdf2840
    uint32_t** streams;        // DAT_6fdf1820, array of FILE* (entry +0xC = flag, +0x20 = CS)
    void (*lock)(int);                       // 0x2EF9
    void (*unlock_stream)();                 // 0x465A funclet
    void* (*malloc_)(uint32_t);              // 0x4C0D family
    void (*free_)(void*);
    int32_t (*init_cs)(void* cs, uint32_t spin); // ___crtInitCritSecAndSpinCount
    void (*enter_cs)(void*);
    int32_t (*mtinitlocknum)(int);           // 0x427F
    void (*lock_file2)(int, void*);          // 0x5F7F
    void (*unlock_file2)(int, void*);        // 0x5E4D
};

// 1.13c D2COMMON+0x04541
D2CommonFile* d2common_getstream_4541(const D2CommonGetstreamEnv* e) {
    D2CommonFile* found = nullptr;
    e->lock(1);
    for (int i = 0; i < e->nstream; ++i) {
        auto** slot = reinterpret_cast<D2CommonFile**>(&e->streams[i]);
        if (!*slot) {
            *slot = static_cast<D2CommonFile*>(e->malloc_(0x38));
            if (*slot) {
                if (!e->init_cs(reinterpret_cast<uint8_t*>(*slot) + 0x20, 4000)) { e->free_(*slot); *slot = nullptr; }
                else { e->enter_cs(reinterpret_cast<uint8_t*>(*slot) + 0x20); found = *slot; }
            }
            break;
        }
        if (((*slot)->flag & 0x83) == 0) {
            if (i > 2 && i < 0x14 && e->mtinitlocknum(i + 0x10) == 0) break;
            e->lock_file2(i, *slot);
            found = *slot;
            if ((found->flag & 0x83) == 0) break;
            e->unlock_file2(i, found);
            found = nullptr;
        }
    }
    if (found) { found->cnt = 0; found->flag = 0; found->base = 0; found->ptr = 0; found->tmpfname = 0; found->file = -1; }
    e->unlock_stream();
    return found;
}

// 1.13c D2COMMON+0x04D9C
void d2common_freebuf_4d9c(D2CommonFile* f, void (*free_)(uint32_t)) {
    if ((f->flag & 0x83) && (f->flag & 8)) {
        free_(f->base);
        f->flag = (f->flag & 0xfffff0000u) | (static_cast<uint16_t>(f->flag) & 0xfbf7);
        f->ptr = 0; f->base = 0; f->cnt = 0;
    }
}

// Unit-skill style list node helpers are owned by the 0x4F080.. family; reached by seam.
struct D2CommonSkillListEnv {
    uint32_t* (*find)(void* ctx, int a, int id, int m1);   // 0x4F080
    void (*refresh)(void* ctx, int a);                     // 0x52480
    uint32_t* (*create)(void* ctx, int a);                 // 0x52660
    void (*remove)(int a, int b);                          // 0x528B0
};

// 1.13c D2Common+0x52BC0
void d2common_sub_52bc0(int32_t* p1, int p2, int p3, int p4, const D2CommonSkillListEnv* e) {
    if (p3 == 0) {
        if (p4) { e->remove(p2, 0); e->refresh(p1, p2); return; }
        uint32_t* n = e->find(p1, p2, -1, 0);
        if (!n) { e->create(p1, p2); n = e->find(p1, p2, -1, 0); if (!n) return; }
        n[10] = 0;
        e->refresh(p1, p2);
        return;
    }
    // list head at p1[0x2a]; walk node->next (+4), node[0] -> short id, node[0xd] == -1
    int32_t* node = nullptr;
    if (p1[0x2a]) {
        for (int32_t* q = *reinterpret_cast<int32_t**>(static_cast<intptr_t>(p1[0x2a]) + 4); q;
             q = reinterpret_cast<int32_t*>(static_cast<intptr_t>(q[1]))) {
            if (*reinterpret_cast<int16_t*>(static_cast<intptr_t>(q[0])) == p2 && q[0xd] == -1) { node = q; break; }
        }
    }
    if (node || (node = reinterpret_cast<int32_t*>(e->create(p1, p2)))) node[10] = p3;
    e->refresh(p1, p2);
}

struct D2CommonChestEnv {
    int32_t (*snprintf_)(char*, uint32_t, const char*, ...);       // Storm #578
    int32_t (*find_row)(int32_t tbl, const char* name, int);        // Storm #10217
    int32_t tbl;           // DAT_6fdf0890
    uint8_t* rows;         // DAT_6fdf0894
    const char* const* kind_a; // PTR_DAT_6fdeef94 (3 entries)
    const char* const* kind_b; // PTR_DAT_6fdeef88 (3 entries)
    uint8_t** out;         // DAT_6fdf089c, 3*5*3 slots
};

// 1.13c D2COMMON+0x53530
// 1.13c D2Common+0x53530
void d2common_sub_53530(const D2CommonChestEnv* e) {
    uint8_t** o = e->out;
    for (int a = 0; a < 3; ++a)
        for (int act = 1; act <= 5; ++act)
            for (int b = 0; b < 3; ++b) {
                char buf[256];
                e->snprintf_(buf, 0x100, "Act %d%s Chest %s", act, e->kind_a[a], e->kind_b[b]);
                const int32_t r = e->find_row(e->tbl, buf, 0);
                *o++ = r < 0 ? nullptr : e->rows + static_cast<intptr_t>(r) * 0x2c;
            }
}

struct D2CommonTreasureEnv {
    uint8_t* dt;                    // sgptDataTables: +0xbf8 levels rows (0xe4), +0xbfc count
    int32_t (*get_name)(int32_t tbl, uint32_t idx, char* out8);         // Storm #10214
    int32_t (*snprintf_)(char*, uint32_t, const char*, ...);
    const char* fmt;                // DAT_6fde26d0
    void* (*new_group)();           // 0x3780 (returns node, called once as pre-init too)
    void* (*new_entry)();           // 0x4020
    void (*add)(void* grp, void* ent, uint32_t weight, uint32_t flag);     // 0x3650
    uint32_t (*cond)(uint32_t a, uint32_t b);                              // 0x24500
    uint8_t* (*sel)(uint32_t);      // 0x1300: row lookup, or nullptr
    uint32_t nitems; uint8_t* items; // DAT_6fdefb94 / DAT_6fdefb98 (0x1a8 stride)
    int32_t tbl; uint16_t* counter; // DAT_6fdf0acc, DAT_6fdf1460
    void (*fatal)();                // missing-items-table abort path
};

// 1.13c D2COMMON+0x547D0
void d2common_sub_547d0(const D2CommonTreasureEnv* e) {
    e->new_group();
    int32_t n = *reinterpret_cast<int32_t*>(e->dt + 0xbfc);
    for (int32_t lv = 0; lv < n; ++lv) {
        uint8_t* row = *reinterpret_cast<uint8_t**>(e->dt + 0xbf8) + lv * 0xe4;
        if (!row) return;
        if (row[0x1d]) {
            ++*e->counter;
            for (int32_t t = 3; t < 0x61; t += 3) {
                char nm[8], buf[32];
                if (!e->get_name(e->tbl, lv, nm)) continue;
                e->snprintf_(buf, 0x20, e->fmt, nm, t);
                auto* g = static_cast<uint16_t*>(e->new_group());
                if (!g) continue;
                *reinterpret_cast<int32_t*>(g + 8) = 1;
                g[0] = 0; g[1] = static_cast<uint16_t>(t - 3);
                for (uint32_t i = 0; i < e->nitems; ++i) {
                    if (!e->items) { e->fatal(); return; }
                    uint8_t* it = e->items + i * 0x1a8;
                    if (it[0x12a] == 0 && it[0x133] && e->cond(i, lv) &&
                        (lv == 38 /* byte off 0x21d8 */ || e->cond(i, 0x26) == 0) &&
                        t - 3 < it[0xfd] && it[0xfd] <= t) {
                        uint8_t* s = e->sel(0);
                        if (!s) continue;
                        uint32_t w = s[0x1e] < 2 ? 1 : s[0x1e];
                        auto* ent = static_cast<uint8_t*>(e->new_entry());
                        *reinterpret_cast<int16_t*>(ent + 8) = static_cast<int16_t>(i);
                        const bool lt = *reinterpret_cast<uint16_t*>(it + 0xf6) < 100;
                        if (!lt) ent[12] |= 0x10;
                        e->add(g, ent, w, lt ? 0 : 1);
                    }
                }
            }
        }
    }
}
