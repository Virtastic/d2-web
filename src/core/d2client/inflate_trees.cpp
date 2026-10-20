// Prior art used: D2MOO has no D2Client module. The existing native inflater
// owners and fast decoder define table-entry indexing; D2Ex2's zlib.h supplies
// the stream status vocabulary. The cached 1.13c bodies supply the algorithm,
// table limit, incomplete-tree rules and allocation/error-message ordering.
#include "inflate_trees.h"
#include <algorithm>

// 1.13c D2Client+0x0EBB0
int32_t D2Client_BuildInflateTree(const uint32_t* lengths, uint32_t count,
    uint32_t simple, const uint32_t* bases, const uint32_t* extra,
    D2ClientInflateHuft*& tree, uint32_t& bits, D2ClientInflateHuft* workspace,
    uint32_t& used, uint32_t* sorted) {
    uint32_t counts[16] = {}, offsets[16] = {}, prefixes[16] = {};
    D2ClientInflateHuft* tables[16] = {};
    for (uint32_t i = 0; i < count; ++i) ++counts[lengths[i]];
    if (counts[0] == count) { tree = nullptr; bits = 0; return 0; }
    uint32_t minimum = 1, maximum = 15;
    while (!counts[minimum]) ++minimum;
    while (!counts[maximum]) --maximum;
    const uint32_t root = bits = std::clamp(bits, minimum, maximum);
    int32_t remaining = 1 << minimum;
    for (uint32_t k = minimum; k < maximum; ++k) {
        remaining -= static_cast<int32_t>(counts[k]);
        if (remaining < 0) return -3;
        remaining *= 2;
    }
    remaining -= static_cast<int32_t>(counts[maximum]);
    if (remaining < 0) return -3;
    counts[maximum] += remaining;
    for (uint32_t k = 1; k < maximum; ++k) offsets[k + 1] = offsets[k] + counts[k];
    for (uint32_t i = 0; i < count; ++i)
        if (lengths[i]) sorted[offsets[lengths[i]]++] = i;
    const uint32_t realCount = offsets[maximum];
    uint32_t code = 0, symbol = 0, tableSize = 0;
    int32_t level = -1, consumed = -static_cast<int32_t>(root);
    D2ClientInflateHuft* table = nullptr;
    for (uint32_t length = minimum; length <= maximum; ++length) {
        uint32_t left = counts[length];
        while (left--) {
            while (static_cast<int32_t>(length) > consumed + static_cast<int32_t>(root)) {
                ++level;
                consumed += root;
                uint32_t width = length - consumed;
                const uint32_t limit = std::min(root, maximum - consumed);
                uint32_t slots = uint32_t{1} << width;
                if (slots > left + 1 && width < limit) {
                    slots -= left + 1;
                    uint32_t next = length;
                    while (++width < limit) {
                        slots *= 2;
                        if (slots <= counts[++next]) break;
                        slots -= counts[next];
                    }
                }
                tableSize = uint32_t{1} << width;
                if (used + tableSize > 1440) return -3;
                table = workspace + used;
                tables[level] = table;
                used += tableSize;
                if (!level) tree = table;
                else {
                    prefixes[level] = code;
                    const uint32_t index = code >> (consumed - root);
                    auto& link = tables[level - 1][index];
                    link.operation = static_cast<uint8_t>(width);
                    link.bits = static_cast<uint8_t>(root);
                    link.value.base = static_cast<uint32_t>(table - &link);
                }
            }
            D2ClientInflateHuft entry{};
            entry.bits = static_cast<uint8_t>(length - consumed);
            if (symbol == realCount) entry.operation = 192;
            else {
                const uint32_t value = sorted[symbol++];
                if (value < simple) {
                    entry.operation = value < 256 ? 0 : 96;
                    entry.value.base = value;
                } else {
                    entry.operation = static_cast<uint8_t>(extra[value - simple] + 80);
                    entry.value.base = bases[value - simple];
                }
            }
            const uint32_t stride = uint32_t{1} << (length - consumed);
            for (uint32_t index = code >> consumed; index < tableSize; index += stride)
                table[index] = entry;
            uint32_t bit = uint32_t{1} << (length - 1);
            while (code & bit) { code ^= bit; bit >>= 1; }
            code ^= bit;
            while ((code & ((uint32_t{1} << consumed) - 1)) != prefixes[level]) {
                --level;
                consumed -= root;
            }
        }
    }
    return remaining && maximum != 1 ? -5 : 0;
}

// 1.13c D2Client+0x0F0A0
int32_t D2Client_InflateDynamicTrees(uint32_t literals, uint32_t distances,
    const uint32_t* lengths, uint32_t& literalBits, uint32_t& distanceBits,
    D2ClientInflateHuft*& literalTree, D2ClientInflateHuft*& distanceTree,
    D2ClientInflateHuft* workspace, const D2ClientInflateAllocator& allocator, const char*& message) {
    auto* sorted = static_cast<uint32_t*>(allocator.allocate(allocator.opaque, 288, sizeof(uint32_t)));
    if (!sorted) return -4;
    uint32_t used = 0;
    int32_t result = D2Client_BuildInflateTree(lengths, literals, 257, lengthBase, lengthExtra,
        literalTree, literalBits, workspace, used, sorted);
    if (result || !literalBits) {
        if (result == -3) message = "oversubscribed literal/length tree";
        else if (result != -4) { message = "incomplete literal/length tree"; result = -3; }
    } else {
        result = D2Client_BuildInflateTree(lengths + literals, distances, 0, distanceBase, distanceExtra,
            distanceTree, distanceBits, workspace, used, sorted);
        if (result == -3) message = "oversubscribed distance tree";
        else if (result == -5) { message = "incomplete distance tree"; result = -3; }
        else if (result != -4 && (result || (!distanceBits && literals > 257))) {
            message = "empty distance tree with lengths"; result = -3;
        }
    }
    allocator.release(allocator.opaque, sorted);
    return result;
}

// 1.13c D2Client+0x0F1E0
int32_t D2Client_InflateBitTree(const uint32_t* lengths, uint32_t& bits,
    D2ClientInflateHuft*& tree, D2ClientInflateHuft* workspace,
    const D2ClientInflateAllocator& allocator, const char*& message) {
    auto* sorted = static_cast<uint32_t*>(allocator.allocate(allocator.opaque, 19, sizeof(uint32_t)));
    if (!sorted) return -4;
    uint32_t used = 0;
    int32_t result = D2Client_BuildInflateTree(lengths, 19, 19, nullptr, nullptr,
        tree, bits, workspace, used, sorted);
    if (result == -3) message = "oversubscribed dynamic bit lengths tree";
    else if (result == -5 || !bits) { message = "incomplete dynamic bit lengths tree"; result = -3; }
    allocator.release(allocator.opaque, sorted);
    return result;
}
