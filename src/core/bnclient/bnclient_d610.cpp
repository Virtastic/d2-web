// BNClient 0xD610 (stdcall, one argument: the status out-dword; ret 4): PatchDriver. Takes the game directory from
// GetModuleFileNameA, reads the "Patch\Patches" multi-string from the Storm registry (0x8054) and, for each archive in it,
// opens it (0x8096), rejects a size of 1..4 (0x80A8), reads its "Prepatch.lst" (0x80C6/0x80AE/0x8018/0x80B4) and runs each
// line: "delete <f>" deletes dir\f, "extract <f>" extracts f to dir\f (0xCE60), "execute <f>" appends dir\f to the launch
// list. Then it saves the Launcher, SrcData and DstData registry strings (0x808A), defaults the launch list to
// dir\bnupdate.exe, saves the gateways (0x15030) and launches (0xCDD0). Status: 0 nothing to do, -1 a failure, 2 launched.
#include <cstdint>
#include <cstdio>
#include <cstring>

struct BnD610Seams {
    void (*fatal)();                                                                   // 0x80E4/0x80DE/_exit (NULL status)
    uint32_t (*filename)(uint32_t hm, char* out, uint32_t n);                          // GetModuleFileNameA
    char* (*chr)(const char* s, int c);                                                // 0x809C SStrChr
    uint32_t (*regload)(uint32_t key, uint32_t name, uint32_t flags, char* out, uint32_t n, uint32_t got);  // 0x8054
    uint32_t (*open)(const char* name, uint32_t a, uint32_t b, uint32_t* h);           // 0x8096
    uint32_t (*count)(uint32_t h, uint32_t* out);                                      // 0x80A8
    uint32_t (*fopen)(uint32_t h, uint32_t name, uint32_t flags, uint32_t* file);      // 0x80C6
    uint32_t (*fsize)(uint32_t file, uint32_t high);                                   // 0x80AE
    uint8_t* (*alloc)(uint32_t n, uint32_t src, uint32_t line, uint32_t flags);        // 0x8018
    uint32_t (*read)(uint32_t file, uint8_t* buf, uint32_t n, uint32_t got, uint32_t ovl);  // 0x80B4
    void (*free)(uint8_t* p, uint32_t src, uint32_t line, uint32_t flags);             // 0x801E
    void (*fclose)(uint32_t file);                                                     // 0x80BA
    void (*close)(uint32_t h);                                                         // 0x80C0
    void (*del)(const char* path);                                                     // DeleteFileA
    void (*extract)(const char* name, uint32_t h, const char* path);                   // 0xCE60 (ECX name, EDX h; stack path)
    void (*regsave)(uint32_t key, uint32_t name, uint32_t flags, const char* s);       // 0x808A
    uint32_t (*copy)(char* d, const char* s, uint32_t max);                            // 0x804E SStrCopy
    uint32_t (*len)(const char* s);                                                    // 0x8042 SStrLen
    int32_t (*nicmp)(const char* a, const char* b, uint32_t n);                        // 0x165A _strnicmp
    void (*save)(uint32_t a);                                                          // 0x15030 (stdcall 1)
    uint32_t (*launch)(const char* list, const char* dir);                             // 0xCDD0 (EAX list, EBX dir)
    uint32_t k_patch, k_patches, k_prepatch, k_src, k_launcher, k_srcdata, k_dstdata, k_save;
};

// 1.13c BNClient+0x0D610
extern "C" uint32_t bnclient_patch_driver_d610(int32_t* status, const BnD610Seams* s) {
    if (status == nullptr) { s->fatal(); return 0; }
    char dir[0x104];
    memset(dir, 0, sizeof dir);
    s->filename(0, dir, 0x104);
    dir[0x103] = 0;
    char* e = dir;
    while (*e != 0) {
        if (s->chr(e + 1, '\\') == nullptr) break;
        e = s->chr(e + 1, '\\');
    }
    *e = 0;
    char list[0x400];
    memset(list, 0, sizeof list);
    if (s->regload(s->k_patch, s->k_patches, 2, list, 0x3FE, 0) == 0 || list[0] == 0) { *status = 0; return 0; }
    char out[0x400];
    memset(out, 0, sizeof out);
    char* cursor = out;
    const char* name = list;
    do {
        uint32_t h, cnt, file;
        if (s->open(name, 0, 0, &h) == 0) { s->del(name); *status = -1; return 0; }
        s->count(h, &cnt);
        if (cnt != 0 && cnt < 5) { s->close(h); s->del(name); *status = -1; return 0; }
        if (s->fopen(h, s->k_prepatch, 0, &file) != 0) {
            const uint32_t size = s->fsize(file, 0);
            uint8_t* buf = s->alloc(size, s->k_src, 0x4F6, 0);
            if (s->read(file, buf, size, 0, 0) != 0 && size != 0) {
                const uint8_t* p = buf;
                do {
                    char line[0x100];
                    memset(line, 0, sizeof line);
                    if ((uint32_t)(p - buf) < size) {
                        uint32_t k = (uint32_t)(p - buf);
                        for (int i = 0; i < 0xFF; ++i) {
                            const uint8_t c = *p;
                            if (c == 0 || c == '\r' || c == '\n') break;
                            line[i] = (char)c;
                            line[i + 1] = 0;
                            ++p;
                            if (++k >= size) break;
                        }
                    }
                    if ((uint32_t)(p - buf) < size) {
                        uint32_t k = (uint32_t)(p - buf);
                        while (*p == '\r' || *p == '\n') { ++p; if (++k >= size) break; }
                    }
                    char path[0x104], arg[0x104];
                    memset(path, 0, sizeof path);
                    memset(arg, 0, sizeof arg);
                    if (s->chr(line, ' ') != nullptr) {
                        const char* q = s->chr(line, ' ');
                        do ++q; while (*q == ' ');
                        s->copy(arg, q, 0x104);
                        sprintf(path, "%s\\%s", dir, q);
                    }
                    if (s->nicmp(line, "delete ", 7) == 0) s->del(path);
                    else if (s->nicmp(line, "extract ", 8) == 0) s->extract(arg, h, path);
                    else if (s->nicmp(line, "execute ", 8) == 0) cursor += s->copy(cursor, path, 0x7FFFFFFF) + 1;
                } while ((uint32_t)(p - buf) < size);
            }
            s->free(buf, s->k_src, 0x52B, 0);
            s->fclose(file);
        }
        s->close(h);
        name += s->len(name) + 1;
    } while (*name != 0);
    char b[0x388];
    sprintf(b, "%s\\Game.exe", dir);
    s->regsave(s->k_patch, s->k_launcher, 2, b);
    sprintf(b, "%s\\%s;%s\\%s;%s\\%s", dir, "d2exp.mpq", dir, "d2data.mpq", dir, "d2sfx.mpq");
    s->regsave(s->k_patch, s->k_srcdata, 2, b);
    sprintf(b, "%s\\%s", dir, "patch_d2.mpq");
    s->regsave(s->k_patch, s->k_dstdata, 2, b);
    if (cursor == out) {
        sprintf(cursor, "%s\\bnupdate.exe", dir);
        cursor += s->len(cursor) + 1;
    }
    *cursor = 0;
    s->save(s->k_save);
    if (s->launch(out, dir) == 0) { *status = -1; return 0; }
    *status = 2;
    return 1;
}
