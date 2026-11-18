// 1.13c Storm+0x2160 -- TSList::LinkNode(EAX = list, EBX = node, [esp+4] = linktype, [esp+8] = existing), ret 8.
// Retail is 32-bit: the list is {linkoffset, term.link, term.tag} in dwords and every TSLink is {link, tag}
// (a negative tag is the complement of the previous slot's address, a non-negative one a byte offset).
// The link of `node` (node + linkoffset, or the list terminator when node is 0) is unlinked if live, then
// inserted next to `existing`'s link (or the terminator): linktype 1 inserts after it, anything else before.
// Retail stores the bare node pointer (not its link) as the forward word, exactly as here.
#include <cstdint>

namespace {
inline uint32_t* W(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
inline uint32_t A(const uint32_t* p) { return (uint32_t)(uintptr_t)p; }
}

extern "C" void Storm_LinkNode_2160(uint32_t* list, uint32_t node, uint32_t linktype, uint32_t existing) {
    uint32_t* f = node ? W(list[0] + node) : list + 1;
    if (f[0] != 0) {                                    // the shared unlink snippet
        uint32_t tag = f[1];
        uint32_t* slot = (int32_t)tag < 0 ? W(~tag) : W(tag + A(f) - W(f[0])[1]);
        *slot = f[0];
        W(f[0])[1] = f[1];
        f[0] = 0;
        f[1] = 0;
    }
    uint32_t* ef = existing ? W(list[0] + existing) : list + 1;
    if (linktype != 1) {
        uint32_t prev = ef[0];
        f[0] = prev;
        f[1] = W(prev)[1];
        W(prev)[1] = node;
        ef[0] = A(f);
        return;
    }
    f[0] = A(ef);
    f[1] = ef[1];
    uint32_t tag = ef[1];
    uint32_t off = list[0];
    if ((int32_t)tag < 0) {
        *W(~tag) = A(f);
    } else {
        if ((int32_t)off < 0) off = A(ef) - W(ef[0])[1];
        *W(tag + off) = A(f);
    }
    ef[1] = node;
}
