// Prior art used: no D2MOO D2Client module. zlib's inflate_blocks state machine
// (reference/D2Ex2/libpng/zlib.h for status values) cross-checks the mode
// numbering; the cached 1.13c body supplies every transition, message and
// allocation/release order. Bit buffer, input and window are live native views.
#include "inflate_blocks.h"
#include "inflate_codes.h"
#include "inflate_trees.h"

namespace {
constexpr uint8_t kBorder[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
struct FixedTrees {
    D2ClientInflateHuft work[1440];
    D2ClientInflateHuft *tl = nullptr, *td = nullptr;
    uint32_t bl = 9, bd = 5;
    FixedTrees() {
        uint32_t len[288], sorted[288], used = 0;
        for (uint32_t i = 0; i < 288; ++i) len[i] = i < 144 ? 8 : i < 256 ? 9 : i < 280 ? 7 : 8;
        D2Client_BuildInflateTree(len, 288, 257, lengthBase, lengthExtra, tl, bl, work, used, sorted);
        for (uint32_t i = 0; i < 30; ++i) len[i] = 5;
        D2Client_BuildInflateTree(len, 30, 0, distanceBase, distanceExtra, td, bd, work, used, sorted);
    }
};
}

// 1.13c D2Client+0x0F4C0
int32_t D2Client_InflateBlocks(D2ClientInflateBlocks& s, D2ClientInflateInput& in,
    D2ClientInflateOutput& out, const D2ClientInflateAllocator& al, int32_t r) {
    auto& w = s.window;
    auto mask = [](uint32_t n) { return (uint32_t{1} << n) - 1; };
    auto leave = [&](int32_t code) { return D2Client_FlushInflateWindow(w, out, code); };
    auto need = [&](uint32_t n) {
        while (s.bitCount < n) {
            if (!in.available) return false;
            --in.available; ++in.total; r = 0;
            s.bitBuffer |= uint32_t{*in.next++} << (s.bitCount & 31);
            s.bitCount += 8;
        }
        return true;
    };
    auto drop = [&](uint32_t n) { s.bitBuffer >>= n; s.bitCount -= n; };
    auto space = [&]() -> std::ptrdiff_t {
        return w.write < w.read ? w.read - w.write - 1 : w.end - w.write;
    };
    auto wrap = [&]() { if (w.write == w.end && w.read != w.begin) w.write = w.begin; };
    auto bad = [&](const char* msg) { s.mode = 9; in.message = msg; return leave(-3); };
    auto done = [&]() { s.mode = s.last ? 7 : 0; };
    for (;;) {
        switch (s.mode) {
        case 0: {
            if (!need(3)) return leave(r);
            s.last = s.bitBuffer & 1;
            const uint32_t type = (s.bitBuffer & 7) >> 1;
            drop(3);
            if (type == 0) {
                const uint32_t t = s.bitCount & 7;
                drop(t);
                s.mode = 1;
            } else if (type == 1) {
                static const FixedTrees fixed;
                auto* codes = D2Client_NewInflateCodes(al, uint8_t(fixed.bl), uint8_t(fixed.bd), fixed.tl, fixed.td);
                if (!codes) return leave(-4);
                s.sub.codes = codes;
                s.mode = 6;
            } else if (type == 2) s.mode = 3;
            else { s.mode = 9; in.message = "invalid block type"; return leave(-3); }
            break;
        }
        case 1:
            if (!need(32)) return leave(r);
            if ((~s.bitBuffer >> 16) != (s.bitBuffer & 0xffff)) {
                s.mode = 9; in.message = "invalid stored block lengths"; return leave(-3);
            }
            s.sub.trees.table = s.bitBuffer & 0xffff;
            s.bitBuffer = 0; s.bitCount = 0;
            if (s.sub.trees.table) s.mode = 2; else done();
            break;
        case 2: {
            if (!in.available) return leave(r);
            if (!space()) {
                wrap();
                if (!space()) {
                    r = leave(r);
                    wrap();
                    if (!space()) return leave(r);
                }
            }
            r = 0;
            uint32_t n = s.sub.trees.table;
            if (n > in.available) n = in.available;
            if (n > space()) n = uint32_t(space());
            for (uint32_t i = 0; i < n; ++i) w.write[i] = in.next[i];
            w.write += n; in.next += n; in.available -= n; in.total += n;
            s.sub.trees.table -= n;
            if (!s.sub.trees.table) done();
            break;
        }
        case 3: {
            if (!need(14)) return leave(r);
            const uint32_t t = s.bitBuffer & 0x3fff;
            if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29) {
                s.sub.trees.table = t;
                return bad("too many length or distance symbols");
            }
            auto* lengths = static_cast<uint32_t*>(al.allocate(al.opaque, 258 + (t & 0x1f) + ((t >> 5) & 0x1f), 4));
            if (!lengths) return leave(-4);
            s.sub.trees.table = t;
            s.sub.trees.lengths = lengths;
            drop(14);
            s.sub.trees.index = 0;
            s.mode = 4;
            break;
        }
        case 4: {
            auto& tr = s.sub.trees;
            while (tr.index < (tr.table >> 10) + 4) {
                if (!need(3)) return leave(r);
                tr.lengths[kBorder[tr.index++]] = s.bitBuffer & 7;
                drop(3);
            }
            while (tr.index < 19) tr.lengths[kBorder[tr.index++]] = 0;
            tr.bits = 7;
            const int32_t e = D2Client_InflateBitTree(tr.lengths, tr.bits, tr.tree, s.workspace, al, in.message);
            if (e) {
                if (e == -3) { al.release(al.opaque, tr.lengths); s.mode = 9; }
                return leave(e);
            }
            tr.index = 0;
            s.mode = 5;
            break;
        }
        case 5: {
            auto& tr = s.sub.trees;
            for (;;) {
                const uint32_t t = tr.table, total = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
                if (tr.index >= total) break;
                if (!need(tr.bits)) return leave(r);
                const auto& h = tr.tree[s.bitBuffer & mask(tr.bits)];
                const uint32_t v = h.value.base, hb = h.bits;
                if (v < 16) {
                    drop(hb);
                    tr.lengths[tr.index++] = v;
                    continue;
                }
                const uint32_t extra = v == 18 ? 7 : v - 14, rep0 = v == 18 ? 11 : 3;
                if (!need(hb + extra)) return leave(r);
                drop(hb);
                const uint32_t count = rep0 + (s.bitBuffer & mask(extra));
                drop(extra);
                if (total < count + tr.index || (v == 16 && tr.index == 0)) {
                    al.release(al.opaque, tr.lengths);
                    return bad("invalid bit length repeat");
                }
                const uint32_t fill = v == 16 ? tr.lengths[tr.index - 1] : 0;
                for (uint32_t i = 0; i < count; ++i) tr.lengths[tr.index++] = fill;
            }
            D2ClientInflateHuft *tl = nullptr, *td = nullptr;
            uint32_t bl = 9, bd = 6;
            uint32_t* const lengths = tr.lengths;
            const uint32_t t = tr.table;
            tr.tree = nullptr;
            const int32_t e = D2Client_InflateDynamicTrees((t & 0x1f) + 257, ((t >> 5) & 0x1f) + 1,
                lengths, bl, bd, tl, td, s.workspace, al, in.message);
            if (e) {
                if (e == -3) { al.release(al.opaque, lengths); s.mode = 9; }
                return leave(e);
            }
            auto* codes = D2Client_NewInflateCodes(al, uint8_t(bl), uint8_t(bd), tl, td);
            if (!codes) return leave(-4);
            s.sub.codes = codes;
            al.release(al.opaque, lengths);
            s.mode = 6;
            break;
        }
        case 6: {
            r = D2Client_InflateCodes(s, in, out, r);
            if (r != 1) return leave(r);
            r = 0;
            al.release(al.opaque, s.sub.codes);
            done();
            break;
        }
        case 7:
            r = leave(r);
            if (w.read != w.write) return leave(r);
            s.mode = 8;
            break;
        case 8:
            return leave(1);
        case 9:
            return leave(-3);
        default:
            return leave(-2);
        }
    }
}
