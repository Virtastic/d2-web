#include <cstdint>

// 1.13c D2Common+0x075A0 __tsopen_lk(shflag[ECX], &inited, &fh, path, oflag, pmode): MSVC CRT open
// body. Maps oflag access/share/create bits to CreateFileA args, allocates a CRT handle slot,
// opens, classifies via GetFileType (char=0x40, pipe=0x08), records the osfile flag byte, and for
// text-mode read/write with O_APPEND-like 0x80 set probes the trailing ^Z (truncate it).
// Env seam: Win32 imports, CRT helpers and globals are owner-supplied.
struct D2TsOpenEnv {
    uint32_t fmode;            // DAT_6fdefa44 (_fmode)
    uint32_t umaskByte;        // DAT_6fdef5a0 (low byte used)
    uint32_t osplatform;       // DAT_6fdef5a4
    uint8_t** osfile;          // DAT_6fdf1720 pointer table; entries are 0x24-byte records, flags at +4
    void (*setErrno)(int);     // FUN_6fd544bc()->*p = v
    void (*setDosErrno)(uint32_t); // FUN_6fd544c5()->*p = v
    uint32_t (*getDosErrno)(); // *FUN_6fd544c5()
    uint32_t (*allocOsfhnd)(); // __alloc_osfhnd (0x7B5D)
    void (*setOsfhnd)(uint32_t fh, intptr_t h);   // __set_osfhnd (0x795F-ish)
    uint32_t (*lseekLk)(uint32_t fh, int32_t off, int whence); // __lseek_lk
    int (*readLk)(uint32_t fh, char* buf, uint32_t n);         // FUN_6fd58f13
    int (*chsizeLk)(uint32_t fh, uint32_t size);               // FUN_6fd58dce
    void (*closeLk)(uint32_t fh);                              // __close_lk
    void (*dosmaperr)(uint32_t err);                           // __dosmaperr
    intptr_t (*createFileA)(const char*, uint32_t access, uint32_t share, void* sa,
                            uint32_t disp, uint32_t flags, intptr_t tmpl);
    uint32_t (*getFileType)(intptr_t h);
    void (*closeHandle)(intptr_t h);
    uint32_t (*getLastError)();
};

uint32_t d2common_tsopen_lk_75a0(D2TsOpenEnv& e, uint32_t shflag, uint32_t* inited, uint32_t* fhOut,
                                 const char* path, uint32_t oflag, uint8_t pmode)
{
    struct { uint32_t len; uint32_t sd; uint32_t inherit; } sa = { 0xc, 0, (uint32_t)(((int8_t)oflag) >= 0) };
    static_assert(sizeof(sa) == 12, "retail SECURITY_ATTRIBUTES is 3 x 32-bit");
    uint8_t fl = sa.inherit ? 0 : 0x10;
    uint32_t access, share, disp, attr;
    auto bad = [&]() { e.setErrno(0x16); e.setDosErrno(0); return 0xffffffffu; };

    if (!(oflag & 0x8000) && ((oflag & 0x4000) || e.fmode != 0x8000)) fl |= 0x80;
    switch (oflag & 3) {
        case 0: access = 0x80000000; break;
        case 1: access = 0x40000000; break;
        case 2: access = 0xc0000000; break;
        default: return bad();
    }
    switch (shflag) {
        case 0x10: share = 0; break;
        case 0x20: share = 1; break;
        case 0x30: share = 2; break;
        case 0x40: share = 3; break;
        default: return bad();
    }
    switch (oflag & 0x700) {
        case 0x000: case 0x400: disp = 3; break;
        case 0x100: disp = 4; break;
        case 0x300: disp = 2; break;
        case 0x200: case 0x600: disp = 5; break;
        case 0x500: case 0x700: disp = 1; break;
        default: return bad();
    }
    attr = 0x80;
    if ((oflag & 0x100) && !((int8_t)(~(uint8_t)e.umaskByte & pmode) < 0)) attr = 1;
    if (oflag & 0x40) {
        access = (access & 0xff000000u) | 0x10000;
        attr |= 0x4000000;
        if (e.osplatform == 2) share |= 4;
    }
    if (oflag & 0x1000) attr |= 0x100;
    if (oflag & 0x20) attr |= 0x8000000;
    else if (oflag & 0x10) attr |= 0x10000000;

    uint32_t fh = e.allocOsfhnd();
    if (fh == 0xffffffffu) { e.setErrno(0x18); e.setDosErrno(0); return 0xffffffffu; }
    *inited = 1;
    *fhOut = fh;
    intptr_t h = e.createFileA(path, access, share, &sa, disp, attr, 0);
    if (h != (intptr_t)-1) {
        uint32_t ty = e.getFileType(h);
        if (ty != 0) {
            if (ty == 2) fl |= 0x40; else if (ty == 3) fl |= 8;
            e.setOsfhnd(fh, h);
            uint8_t* rec = e.osfile[(int32_t)fh >> 5] + (fh & 0x1f) * 0x24 + 4;
            uint8_t b = fl | 1;
            fl &= 0x48;
            *rec = b;
            if (fl == 0 && (int8_t)b < 0 && (oflag & 2)) {
                uint32_t pos = e.lseekLk(fh, -1, 2);
                bool ok;
                if (pos == 0xffffffffu) {
                    ok = e.getDosErrno() == 0x83;
                } else {
                    char c = 0;
                    ok = (e.readLk(fh, &c, 1) == 0 && c == 0x1a && e.chsizeLk(fh, pos) == -1) ? false : true;
                    if (ok) ok = e.lseekLk(fh, 0, 0) != 0xffffffffu;
                }
                if (!ok) { e.closeLk(fh); return 0xffffffffu; }
            }
            if (fl == 0 && (oflag & 8)) *rec |= 0x20;
            return fh;
        }
        e.closeHandle(h);
    }
    e.dosmaperr(e.getLastError());
    return 0xffffffffu;
}
