// 1.13c Storm+0x39030: SCODE emit. Normalises one operation's three operand words
// (a register letter in byte 0, a width in byte 1, flags in bytes 2-3), splits "a = b op a" into two steps when the
// destination is also a source, and appends a node {dst, src, op index, pass} to the list at this+0xC.
// Retail returns with EAX = the op index it stored. The list-node allocator (it takes a mode word in EDI, 2 here)
// is a seam. The op index is the offset of the op character in "=&|^+-~@", which sits at the module address kOpsVa;
// a character outside the set stores 0 - kOpsVa, as retail does.
#include <cstdint>

typedef uint32_t u32;
typedef uint8_t u8;

struct Storm39030Env {
    u32* (*alloc_node)(void* list, u32 a, u32 b, u32 mode);   // list-node alloc (stdcall 3, EDI = mode)
};

static const u32 kOpsVa = 0x6fc354ecu;

static inline u8 byteOf(u32 x, int i) { return (u8)(x >> (8 * i)); }
static inline void setWidth(u32& x, u8 v) { x = (x & 0xFFFF00FFu) | ((u32)v << 8); }

u32 Storm_39030(void* self, u32 pass, u32 dst, u32 src, u32 alt, u32 op, const Storm39030Env& env) {
    u32 keep, next;
    u8 altReg = (u8)alt;
    if (byteOf(dst, 1) == 0 && byteOf(src, 1) == 0 && byteOf(alt, 1) == 0) {
        if (pass == 1) goto widen_dst;
        goto check_split;
    }
    pass = 1;
    if (byteOf(dst, 1) == 0) goto widen_dst;
    goto widen_src;
widen_dst:
    if (byteOf(dst, 2) == 0 && byteOf(dst, 3) == 0) {
        if ((u8)dst == (u8)src && byteOf(src, 1) != 0) setWidth(dst, byteOf(src, 1));
        else {
            if ((u8)dst == (u8)alt) {
                setWidth(dst, byteOf(alt, 1));
                if (byteOf(alt, 1) != 0) goto widen_src;
            }
            setWidth(dst, 1);
        }
    }
widen_src:
    if (byteOf(src, 1) == 0 && byteOf(src, 2) == 0 && byteOf(src, 3) == 0) setWidth(src, 1);
    if ((u8)alt != 0) {
        if (byteOf(alt, 1) == 0 && byteOf(alt, 2) == 0 && byteOf(alt, 3) == 0) setWidth(alt, 1);
        goto check_split;
    }
    goto check_imm;
check_split:
    if (altReg == 0 || (u8)op == 0 || dst != alt || dst == src) goto check_imm;
    next = alt;
    goto swap;
check_imm:
    keep = src;
    if (((u8)src != 8 && (u8)src != 9) || altReg == 8 || altReg == 9) goto emit;
    next = alt;
swap:
    alt = (alt & 0xFFFFFF00u) | (u8)src;
    altReg = (u8)alt;
    keep = next;
    alt = src;
emit:
    src = keep;
    if (altReg != 0 && (u8)op != 0 && dst != src && dst != alt) {
        Storm_39030(self, pass, dst, keep, 0, 0, env);
        src = dst;
    }
    u32* n = env.alloc_node((u8*)self + 0xc, 0, 0, 2);
    n[2] = dst;
    n[3] = altReg == 0 ? src : alt;
    n[5] = pass;
    if ((u8)op == 0) { n[4] = 0; return 0; }
    static const char ops[] = "=&|^+-~@";
    u32 idx = 0u - kOpsVa;
    for (u32 i = 0; ops[i]; ++i) if (ops[i] == (char)op) { idx = i; break; }
    n[4] = idx;
    return idx;
}
