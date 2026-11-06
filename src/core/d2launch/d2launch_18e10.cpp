// 1.13c D2Launch+0x18E10  sub_18E10: the terms-of-service screen. Fetches tos-unicode_<lang>.txt through the BNET object, reads it
// (UTF-16LE, UTF-16BE or ANSI), wraps it into lines of at most 200 chars against the list width and adds every non-blank line, then
// builds the screen controls. Returns 1, or 0 on cancel / no BNET file. Every callee is a seam in the retail register layout (names
// give the thunk RVA or IAT slot; literals are image RVAs); g = the module range [0x25200, 0x25EC0).
#include <cstdint>
#include <cstring>

struct D2l18E10Seams {
    void (*f14a20)();
    void (*o9b26)(char* buf, uint32_t n);                                  // D2Lang 10004, stdcall: language code into buf
    void (*o9790)(char* dst, uint32_t srcRva, uint32_t max);               // Storm 501, stdcall
    void (*o975a)(char* dst, const char* src, uint32_t max);               // Storm 503, stdcall
    void (*o975aR)(char* dst, uint32_t srcRva, uint32_t max);              // Storm 503 with an image string
    void (*bnet50)(uint32_t bnet, const char* ecx, uint32_t edx);          // call [bnet + 0x50] (EAX = bnet)
    void (*o99a0)(uint32_t cbRva);                                         // D2Win 10014, stdcall: the modal wait, callback 0x148D0
    void (*o9922)(uint32_t* slot);                                         // D2Win 10050, stdcall
    uint32_t (*bnet68)(uint32_t bnet);                                     // call [bnet + 0x68] (ECX = bnet)
    void (*f12b60)();
    void (*f176c0)();
    uint32_t (*f12b10)(uint32_t eax);
    uint32_t (*o97e4)(const char* ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);   // Fog 10107
    uint32_t (*getFileSize)(uint32_t h, uint32_t hi);                      // kernel32 GetFileSize (IAT 0x1C3F0)
    uint8_t* (*o97de)(uint32_t ecx, uint32_t edxRva, uint32_t line, uint32_t z);       // Fog 10042 alloc
    void (*o9802)(uint32_t ecx, uint8_t* edx, uint32_t n, uint32_t* got, uint32_t z);  // Fog 10110 read
    void (*o9808)(uint32_t ecx);                                           // Fog 10109
    void (*o981a)(const char* ecx);                                        // Fog 10108
    void (*f41000)(char16_t* esi, uint32_t edi);                           // byte-swap ESI[0..EDI)
    void (*win2uni)(char16_t* ecx, const uint8_t* edx, uint32_t n);        // D2Lang Unicode::win2Unicode (IAT 0x1C038)
    void (*o97fc)(uint8_t* ecx, uint32_t edxRva, uint32_t line, uint32_t z);           // Fog 10043 free
    void (*uctor)(char16_t* ecx);                                          // D2Lang Unicode default ctor closure (IAT 0x1C020)
    int32_t (*o9880)(const char16_t* ecx);                                 // D2Win 10028: text width
    int32_t (*ustrlen)(const char16_t* ecx);                               // D2Lang Unicode::strlen (IAT 0x1C050)
    uint32_t (*isLB)(const char16_t* ecx, int32_t edx);                    // D2Lang Unicode::isLineBreak (IAT 0x1C02C)
    void (*o99be)(uint32_t ecx, const char16_t* edx, uint32_t s0, uint32_t s1, uint32_t s2);  // D2Win 10000: add a line
    void (*o99a6)(uint32_t ecx, uint32_t edx);                             // D2Win 10037
    uint32_t (*o9952)(uint32_t tplRva);                                    // D2Win 10098
    const char16_t* (*o9b32)(uint32_t ecx);                                // D2Lang 10003
    void (*f15090)(const char16_t* s, uint32_t fnRva, uint32_t one);
    uint32_t (*o9820)(uint32_t id);                                        // Fog 10265
    void (*o97f6)(uint32_t strRva, uint32_t msg);                          // Fog 10024
    void (*exit_)(int32_t code);
};

static constexpr uint32_t kBase = 0x25200, kSrc = 0x1E068;

extern "C" uint32_t d2l_18e10(const D2l18E10Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - kBase)); };
    auto push = [&](uint32_t tpl) -> uint32_t {
        uint32_t v = s->o9952(tpl);
        uint32_t n = W(0x25E30);
        W(0x252B8 + n * 4) = v;
        W(0x25E30) = n + 1;
        return v;
    };
    char lang[0x70], name[0x80];
    char16_t L[200];
    W(0x25294) = 0;
    s->f14a20();
    s->o9b26(lang, 2);
    s->o9790(name, 0x1E098, 0x80);
    s->o975a(name, lang, 0x80);
    s->o975aR(name, 0x1E090, 0x80);
    s->bnet50(W(0x256C4), name, (uint32_t)strlen(name));
    s->o99a0(0x148D0);
    if (W(0x25294)) {
        for (int32_t i = 0; i < (int32_t)W(0x25E34); i++) s->o9922(&W(0x253C0 + (uint32_t)i * 4));
        W(0x25E34) = 0;
        return 0;
    }
    uint32_t list = 0, fh = 0xFFFFFFFFu;
    if (s->bnet68(W(0x256C4))) {
        s->f12b60();
        s->f176c0();
        s->f12b10(0x4D);
        list = s->f12b10(0x4E);
        fh = s->o97e4(name, 0x80000000u, 0, 0, 3, 0x8000000, 0);
    }
    if (fh == 0xFFFFFFFFu) { s->f15090(s->o9b32(0x14E3), 0x17DF0, 1); return 0; }
    uint32_t size = s->getFileSize(fh, 0);
    uint8_t* buf = s->o97de(size, kSrc, 0xC34, 0);
    uint32_t got = 0;
    s->o9802(fh, buf, size, &got, 0);
    s->o9808(fh);
    s->o981a(name);
    uint8_t* owned = buf;
    const char16_t* text = nullptr;
    int32_t n = 0;
    if (size == got && size > 2) {
        uint16_t bom; memcpy(&bom, buf, 2);
        if (bom == 0xFEFF) { n = (int32_t)((size - 2) >> 1); text = (const char16_t*)(buf + 2); }
        else if (bom == 0xFFFE) { text = (const char16_t*)(buf + 2); n = (int32_t)((size - 2) >> 1); s->f41000((char16_t*)(buf + 2), (uint32_t)n); }
        else {
            uint8_t* w = s->o97de(size * 2 + 1, kSrc, 0xC52, 0);
            s->win2uni((char16_t*)w, buf, size);
            s->o97fc(buf, kSrc, 0xC54, 0);
            owned = w; text = (const char16_t*)w; n = (int32_t)size;
        }
    }
    int32_t pos = 0, start = 0;
    while (pos < n) {
        for (int i = 0; i < 200; i++) s->uctor(&L[i]);
        memset(L, 0, sizeof L);
        int32_t k = 0;
        while (pos < n) {
            uint16_t ch = (uint16_t)text[pos];
            if (ch == 0xD || ch == 0xA) { L[k] = 0x20; k = 200; }
            else { L[k] = ch == 9 ? 0x20 : ch; k++; }
            pos++;
            if (s->o9880(L) > 0x21C) {
                int32_t m = s->ustrlen(L);
                if (m > 1) { const char16_t* p = text + start; do { if (s->isLB(p, m)) break; } while (--m > 1); }
                pos = m + start;
                L[m] = 0;
                if (m && L[m - 1] == 0x20) L[m - 1] = 0;
                break;
            }
            if (k >= 200) break;
        }
        int32_t ml = s->ustrlen(L);
        for (int32_t i = 0; i < ml; i++) if (L[i] != 0x20) { s->o99be(list, L, 0, 0, 0); break; }
        start = pos;
    }
    s->o97fc(owned, kSrc, 0xC9B, 0);
    s->o99a6(list, 0);
    // Retail tests the count before the first push and after each later one; at 0x40 it reports and exits, and an _exit that returns
    // falls into the last push.
    if ((int32_t)W(0x25E30) >= 0x40) goto fatal;
    push(0x21DF0);
    if ((int32_t)W(0x25E30) >= 0x40) goto fatal;
    push(0x21E20);
    if ((int32_t)W(0x25E30) >= 0x40) goto fatal;
    {
        uint32_t hb = push(0x21E50);
        s->o99be(hb, s->o9b32(0x143C), 0, 0, 0);
    }
    if ((int32_t)W(0x25E30) >= 0x40) goto fatal;
    push(0x21E80);
    if ((int32_t)W(0x25E30) < 0x40) goto last;
fatal:
    s->o97f6(0x1E256, s->o9820(0x624));
    s->exit_(-1);
last:
    push(0x21EB0);
    return 1;
}
