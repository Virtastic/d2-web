#include "output_3985.h"
#include <climits>
#include <cstring>

// Prior art used: none in D2MOO (it does not port the bundled MSVC2003 CRT formatter);
// body written from the cached 1.13c decompile. write_char/_write_multi_char/_write_string
// (+0x38F7/+0x392A/+0x394E) are folded in as the static helpers below.

namespace {
// Retail's class/state lookup (0x20..0x78); class = low nibble, next state = high nibble.
const uint8_t kTable[89] = {
    0x06,0x00,0x00,0x06,0x00,0x01,0x00,0x00,0x10,0x00,0x03,0x06,0x00,0x06,0x02,0x10,
    0x04,0x45,0x45,0x45,0x05,0x05,0x05,0x05,0x05,0x35,0x30,0x00,0x50,0x00,0x00,0x00,
    0x00,0x20,0x28,0x38,0x50,0x58,0x07,0x08,0x00,0x37,0x30,0x30,0x57,0x50,0x07,0x00,
    0x00,0x20,0x20,0x08,0x00,0x00,0x00,0x00,0x08,0x60,0x68,0x60,0x60,0x60,0x60,0x00,
    0x00,0x70,0x70,0x78,0x78,0x78,0x78,0x08,0x07,0x08,0x00,0x00,0x07,0x00,0x08,0x08,
    0x08,0x00,0x00,0x08,0x00,0x08,0x00,0x07,0x08};
const char kNullA[] = "(null)";
const char16_t kNullW[] = u"(null)";

struct Out {
    D2CommonStringStream2814* s;
    const D2CommonOutputCalls3985* c;
    int count;
    // 1.13c D2Common+0x038F7
    void ch(uint8_t b) { // _write_char
        if (!(s->flags & 0x40) || s->base) {
            uint32_t r;
            if (--s->remaining < 0) r = (uint32_t)c->flushByte((int)(int8_t)b, s);
            else { *s->cursor++ = (char)b; r = b; }
            if (r == 0xffffffffu) { count = -1; return; }
        }
        ++count;
    }
    // 1.13c D2Common+0x0392A
    void multi(uint8_t b, int n) { // _write_multi_char
        while (n > 0) { --n; ch(b); if (count == -1) return; }
    }
    // 1.13c D2Common+0x0394E
    void str(const char* p, int n) { // _write_string
        if (!(s->flags & 0x40) || s->base) {
            while (n > 0) { --n; ch((uint8_t)*p++); if (count == -1) return; }
        } else count += n;
    }
};
}

// Seam for the function replay of _write_string (retail regs: eax=&count, ecx=buf, edi=stream, [esp+4]=len).
extern "C" void D2Common_394E_WriteString(D2CommonStringStream2814* s, const D2CommonOutputCalls3985* c,
                                          int* count, const char* p, int n)
{
    Out o{s, c, *count};
    o.str(p, n);
    *count = o.count;
}

// 1.13c D2Common+0x03985
extern "C" int D2Common_3985_Output(D2CommonStringStream2814* stream, const char* fmt,
                                    const D2CommonOutputArgs3985* a,
                                    const D2CommonOutputCalls3985* calls)
{
    Out o{stream, calls, 0};
    char buf[0x210];
    char* const bufEnd = buf + 0x1ff; // retail local_11: digit writer start
    char prefix[2] = {0, 0};
    int state = 0, prec = -1, width = 0, prefixLen = 0, wide = 0, noOut = 0, hexAdd = 0;
    uint32_t flags = 0, caps = 0;
    size_t len = 0;
    const void* text = nullptr;
    void* heap = nullptr;
    unsigned char c = (unsigned char)*fmt;

    for (;;) {
        if (c == 0 || o.count < 0) return o.count;
        const char* next = fmt + 1;
        unsigned cls = (c < 0x20 || c > 0x78) ? 0 : (kTable[c - 0x20] & 0xf);
        state = (int)(int8_t)kTable[cls * 8 + state] >> 4;
        bool literal = state == 0;
        switch (state) {
        case 0: break;
        case 1: prec = -1; caps = 0; noOut = 0; width = 0; prefixLen = 0; flags = 0; wide = 0; break;
        case 2:
            if (c == 0x20) flags |= 2; else if (c == 0x23) flags |= 0x80;
            else if (c == 0x2b) flags |= 1; else if (c == 0x2d) flags |= 4;
            else if (c == 0x30) flags |= 8;
            break;
        case 3:
            if (c == '*') { width = (int)a->u32(a->ctx); if (width < 0) { flags |= 4; width = -width; } }
            else width = (int)(int8_t)c - '0' + width * 10;
            break;
        case 4: prec = 0; break;
        case 5:
            if (c == '*') { prec = (int)a->u32(a->ctx); if (prec < 0) prec = -1; }
            else prec = (int)(int8_t)c - '0' + prec * 10;
            break;
        case 6:
            if (c == 'I') {
                unsigned char n1 = (unsigned char)fmt[1];
                if (n1 == '6' && fmt[2] == '4') { next = fmt + 3; flags |= 0x8000; }
                else if (n1 == '3' && fmt[2] == '2') { next = fmt + 3; flags &= 0xffff7fff; }
                else if (n1 != 'd' && n1 != 'i' && n1 != 'o' && n1 != 'u' && n1 != 'x' && n1 != 'X') {
                    state = 0; literal = true;
                }
            } else if (c == 'h') flags |= 0x20;
            else if (c == 'l') flags |= 0x10;
            else if (c == 'w') flags |= 0x800;
            break;
        case 7: break;
        default: break;
        }
        if (literal) {
            wide = 0;
            if (calls->isLeadByte(c)) { o.ch(c); next = fmt + 2; o.ch((uint8_t)fmt[1]); }
            else o.ch(c);
        } else if (state == 7) {
            enum { kDone, kInt, kStr, kChar, kFloat, kN, kPad } kind = kPad;
            unsigned radix = 10;
            switch (c) {
            case 'X': hexAdd = 7; radix = 16; kind = kInt; break;
            case 'x': hexAdd = 0x27; radix = 16; kind = kInt; break;
            case 'p': prec = 8; hexAdd = 7; radix = 16; kind = kInt; break;
            case 'o': radix = 8; if (flags & 0x80) flags |= 0x200; kind = kInt; break;
            case 'u': kind = kInt; break;
            case 'd': case 'i': flags |= 0x40; kind = kInt; break;
            case 'C': if (!(flags & 0x830)) flags |= 0x800; kind = kChar; break;
            case 'c': kind = kChar; break;
            case 'S': if (!(flags & 0x830)) flags |= 0x800; kind = kStr; break;
            case 's': kind = kStr; break;
            case 'E': case 'G': caps = 1; c += 0x20; kind = kFloat; break;
            case 'e': case 'f': case 'g': kind = kFloat; break;
            case 'n': kind = kN; break;
            case 'Z': kind = kDone; break; // handled below
            default: break;
            }
            if (c == 'Z') {
                const auto* z = (const D2CommonCountedString3985*)a->ptr(a->ctx);
                if (!z || !z->buffer) { text = kNullA; len = std::strlen(kNullA); }
                else {
                    text = z->buffer; len = (size_t)z->length;
                    if (flags & 0x800) { len = (size_t)((int)len / 2); wide = 1; } else wide = 0;
                }
            } else if (kind == kChar) {
                if (!(flags & 0x810)) { buf[0] = (char)a->u32(a->ctx); len = 1; }
                else {
                    len = (size_t)(int)calls->wctomb(buf, (char16_t)a->u32(a->ctx));
                    if ((int)len < 0) noOut = 1;
                }
                text = buf;
            } else if (kind == kStr) {
                int max = prec == -1 ? INT_MAX : prec;
                text = a->ptr(a->ctx);
                if (!(flags & 0x810)) {
                    if (!text) text = kNullA;
                    const char* p = (const char*)text;
                    for (; max != 0 && (--max, *p != 0); ++p) {}
                    len = (size_t)(p - (const char*)text);
                } else {
                    if (!text) text = kNullW;
                    wide = 1;
                    const char16_t* p = (const char16_t*)text;
                    for (; max != 0 && (--max, *p != 0); ++p) {}
                    len = (size_t)(p - (const char16_t*)text);
                }
            } else if (kind == kFloat) {
                flags |= 0x40;
                char* b = buf;
                if (prec < 0) prec = 6;
                else if (prec == 0) { if (c == 'g') prec = 1; }
                else {
                    if (prec > 0x200) prec = 0x200;
                    if (prec > 0xa3) {
                        heap = calls->alloc((size_t)prec + 0x15d);
                        if (heap) b = (char*)heap; else prec = 0xa3;
                    }
                }
                uint64_t d = a->u64(a->ctx);
                calls->cfltcvt(&d, b, (int)(int8_t)c, prec, (int)caps);
                if ((flags & 0x80) && prec == 0) calls->forcdecpt(b);
                if (c == 'g' && !(flags & 0x80)) calls->cropzeros(b);
                if (*b == '-') { flags |= 0x100; ++b; }
                text = b; len = std::strlen(b);
            } else if (kind == kN) {
                void* p = const_cast<void*>(a->ptr(a->ctx));
                if (!(flags & 0x20)) *(int32_t*)p = o.count; else *(int16_t*)p = (int16_t)o.count;
                noOut = 1;
            } else if (kind == kInt) {
                if (c == 'X' || c == 'x' || c == 'p') {
                    if (flags & 0x80) { prefix[1] = (char)(hexAdd + 'Q'); prefix[0] = '0'; prefixLen = 2; }
                }
                uint32_t lo, hi;
                if (!(flags & 0x8000)) {
                    uint32_t v = a->u32(a->ctx);
                    if (!(flags & 0x20)) { lo = v; hi = !(flags & 0x40) ? 0 : (uint32_t)((int32_t)lo >> 31); }
                    else { lo = !(flags & 0x40) ? (uint32_t)(uint16_t)v : (uint32_t)(int32_t)(int16_t)v;
                           hi = (uint32_t)((int32_t)lo >> 31); }
                    if (!(flags & 0x20) && !(flags & 0x40)) hi = 0;
                } else { uint64_t v = a->u64(a->ctx); lo = (uint32_t)v; hi = (uint32_t)(v >> 32); }
                uint64_t val = ((uint64_t)hi << 32) | lo;
                if ((flags & 0x40) && (int32_t)hi < 0) {
                    val = (uint64_t)0 - val; flags |= 0x100;
                    if (!(flags & 0x8000)) val &= 0xffffffffu; // retail drops the high word here
                }
                if (!(flags & 0x8000)) val &= 0xffffffffu;
                if (prec < 0) prec = 1; else { flags &= 0xfffffff7; if (prec > 0x200) prec = 0x200; }
                if (val == 0) prefixLen = 0;
                char* p = bufEnd;
                while (!(prec < 1 && val == 0)) {
                    --prec;
                    unsigned dg = (unsigned)(val % radix); val /= radix;
                    int ch = (int)dg + '0';
                    if (ch > '9') ch += hexAdd;
                    *p-- = (char)ch;
                }
                len = (size_t)(bufEnd - p); text = p + 1;
                if ((flags & 0x200) && (*(const char*)text != '0' || len == 0)) { *p = '0'; ++len; text = p; }
            }
            if (!noOut) {
                if (flags & 0x40) {
                    if (flags & 0x100) { prefix[0] = '-'; prefixLen = 1; }
                    else if (flags & 1) { prefix[0] = '+'; prefixLen = 1; }
                    else if (flags & 2) { prefix[0] = ' '; prefixLen = 1; }
                }
                int pad = (int)((unsigned)width - (unsigned)prefixLen - (unsigned)len);
                if (!(flags & 0xc)) o.multi(' ', pad);
                o.str(prefix, prefixLen);
                if ((flags & 8) && !(flags & 4)) o.multi('0', pad);
                if (!wide || (int)len < 1) o.str((const char*)text, (int)len);
                else {
                    size_t left = len; const char16_t* w = (const char16_t*)text;
                    do {
                        --left;
                        int n = calls->wctomb(buf + 0x200, *w++);
                        if (n < 1) break;
                        o.str(buf + 0x200, n);
                    } while (left != 0);
                }
                if (flags & 4) o.multi(' ', pad);
            }
            if (heap) { calls->release(heap); heap = nullptr; }
        }
        c = (unsigned char)*next;
        fmt = next;
    }
}
