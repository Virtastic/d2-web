#pragma once
// The Storm intrusive-link rule shared by D2Client 0x1030, 0x10B0, 0x1150, 0x11F0, 0x2400, 0x2F70,
// 0x2FF0 and 0x30A0, written once over the link word type W. A link is two words: [0] the previous
// link's address (0 = unlinked), [1] the next object, or the complement of the list terminator when
// negative. W = uint32_t is retail's own layout (the FN vectors drive it on blob memory); W = uintptr_t
// is the play build's native layout. The word size scales every field offset; the rule does not change.
#include <cstdint>
#include <type_traits>

namespace d2c_link {
template <class W> inline W& at(W a) { return *reinterpret_cast<W*>(static_cast<uintptr_t>(a)); }
template <class W> inline bool neg(W v) { return static_cast<typename std::make_signed<W>::type>(v) < 0; }

// Unlink: the next link (the complemented terminator, or node + next - prev->next) takes the node's
// previous pointer, the previous link takes the node's next word, and both node words clear. The node
// words are reloaded after the first store, which may alias the node itself.
template <class W> inline void unlink(W node) {
    const W step = sizeof(W);
    W prev = at<W>(node);
    if (prev == 0) return;
    W next = at<W>(node + step);
    W target = neg<W>(next) ? static_cast<W>(~next) : static_cast<W>(node + (next - at<W>(prev + step)));
    at<W>(target) = prev;
    at<W>(at<W>(node) + step) = at<W>(node + step);
    at<W>(node) = 0;
    at<W>(node + step) = 0;
}

// Link `node` relative to `anchor` (0x10B0 / 0x2400 tail). mode != 1: before the anchor (the anchor's
// previous link now points on to `raw`); mode == 1: after it, the next link's back pointer found through
// the complemented terminator or the list's link offset `rel` (a negative offset is recomputed from the
// anchor's own previous link). `raw` is the caller's object word, stored verbatim.
template <class W> inline void link(W node, W anchor, W raw, int32_t mode, W listBase) {
    const W step = sizeof(W);
    if (mode != 1) {
        W p = at<W>(anchor);
        at<W>(node) = p;
        at<W>(node + step) = at<W>(p + step);
        at<W>(p + step) = raw;
        at<W>(anchor) = node;
        return;
    }
    at<W>(node) = anchor;
    at<W>(node + step) = at<W>(anchor + step);
    W next = at<W>(anchor + step);
    W rel = at<W>(listBase);
    if (neg<W>(next)) {
        at<W>(static_cast<W>(~next)) = node;
    } else {
        if (neg<W>(rel)) rel = static_cast<W>(anchor - at<W>(at<W>(anchor) + step));
        at<W>(static_cast<W>(next + rel)) = node;
    }
    at<W>(anchor + step) = raw;
}

// 0x10B0: ECX = anchor (0 = the terminator at base+1 word), EAX = list base, node argument (0 = the
// terminator). Returns the node.
template <class W> inline W splice10b0(W anchor, W base, W nodeArg, int32_t mode) {
    W node = nodeArg ? nodeArg : static_cast<W>(base + sizeof(W));
    unlink<W>(node);
    W a = anchor ? anchor : static_cast<W>(base + sizeof(W));
    link<W>(node, a, nodeArg, mode, base);
    return node;
}

// 0x2400: EAX = list base, EBX = node object; node and anchor objects are offset by the list's link
// offset base[0] (re-read at each use), 0 selecting the terminator. Returns the node link.
template <class W> inline W splice2400(W base, W nodeArg, uint32_t mode, W anchorArg) {
    W node = nodeArg ? static_cast<W>(at<W>(base) + nodeArg) : static_cast<W>(base + sizeof(W));
    unlink<W>(node);
    W anchor = anchorArg ? static_cast<W>(at<W>(base) + anchorArg) : static_cast<W>(base + sizeof(W));
    link<W>(node, anchor, nodeArg, static_cast<int32_t>(mode), base);
    return node;
}

// 0x30A0: EDI = list; while the terminator's next word is a positive object, unlink that object's link.
template <class W> inline void drain30a0(W list) {
    for (;;) {
        W first = at<W>(list + 2 * sizeof(W));
        if (first == 0 || neg<W>(first)) return;
        unlink<W>(static_cast<W>(at<W>(list) + first));
    }
}

// 0x11F0: EBX = list; the same drain without the link offset (the terminator's next word is the link).
template <class W> inline void drain11f0(W list) {
    for (;;) {
        W first = at<W>(list + 2 * sizeof(W));
        if (first == 0 || neg<W>(first)) return;
        unlink<W>(first);
    }
}

// 0x2F70: allocate size + three words, clear the link, and (mode != 0) splice it into `list` before the
// terminator. The allocator receives (bytes, ".?AUTMESSAGESOURCE@@", -2, line | 8).
template <class W, class Alloc> inline W alloc2f70(W list, int32_t size, uint32_t line, uint32_t mode, Alloc alloc) {
    W node = alloc(static_cast<uint32_t>(size) + 3u * static_cast<uint32_t>(sizeof(W)), line | 8u);
    if (node) { at<W>(node) = 0; at<W>(node + sizeof(W)) = 0; }
    if (mode) splice2400<W>(list, node, mode, 0);
    return node;
}
}  // namespace d2c_link
