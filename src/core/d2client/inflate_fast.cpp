// Prior art used: D2MOO has no D2Client inflater. D2Ex2/libpng/zlib.h
// cross-checks stream return/error conventions. The cached 1.13c body and
// its E350 caller establish the fast-path entry conditions and huft offsets.
#include "inflate_fast.h"
#include <algorithm>
#include <cassert>

// 1.13c D2Client+0x0DF60
int32_t D2Client_InflateFast(uint32_t literalBits, uint32_t distanceBits,
    const D2ClientInflateHuft* literalTree, const D2ClientInflateHuft* distanceTree,
    D2ClientInflateBlocks& blocks, D2ClientInflateInput& input) {
    const uint8_t* cursor = input.next;
    uint32_t available = input.available;
    uint32_t bits = blocks.bitCount, buffer = blocks.bitBuffer;
    auto& window = blocks.window;
    uint8_t* write = window.write;
    uint32_t space = static_cast<uint32_t>(write < window.read
        ? window.read - write - 1 : window.end - write);
    assert(available >= 10 && space >= 258);
    auto mask = [](uint32_t count) { return (uint32_t{1} << count) - 1; };
    auto fill = [&](uint32_t count) {
        while (bits < count) {
            --available;
            buffer |= uint32_t{*cursor++} << (bits & 31);
            bits += 8;
        }
    };
    auto consume = [&](uint32_t count) {
        buffer >>= count & 31;
        bits -= count;
    };
    auto finish = [&](int32_t result) {
        // Whole lookahead bytes return to the input; leave the accumulator
        // unchanged, exactly like retail (only bitCount marks valid bits).
        const uint32_t back = std::min(bits >> 3, input.available - available);
        cursor -= back;
        available += back;
        bits -= back * 8;
        blocks.bitCount = bits;
        blocks.bitBuffer = buffer;
        blocks.window.write = write;
        input.total += static_cast<uint32_t>(cursor - input.next);
        input.next = cursor;
        input.available = available;
        return result;
    };
    do {
        fill(20);
        const D2ClientInflateHuft* literal = literalTree + (buffer & mask(literalBits));
        consume(literal->bits);
        while (literal->operation != 0 && !(literal->operation & 16)) {
            const uint32_t operation = literal->operation;
            if (operation & 64) {
                if (operation & 32) return finish(1);
                input.message = "invalid literal/length code";
                return finish(-3);
            }
            literal += literal->value.base + (buffer & mask(operation));
            consume(literal->bits);
        }
        if (literal->operation == 0) {
            *write++ = static_cast<uint8_t>(literal->value.base);
            --space;
        } else {
            const uint32_t extra = literal->operation & 15;
            const uint32_t length = literal->value.base + (buffer & mask(extra));
            consume(extra);
            fill(15);
            const D2ClientInflateHuft* distance = distanceTree + (buffer & mask(distanceBits));
            consume(distance->bits);
            while (!(distance->operation & 16)) {
                const uint32_t operation = distance->operation;
                if (operation & 64) {
                    input.message = "invalid distance code";
                    return finish(-3);
                }
                distance += distance->value.base + (buffer & mask(operation));
                consume(distance->bits);
            }
            const uint32_t distanceExtra = distance->operation & 15;
            fill(distanceExtra);
            const uint32_t back = distance->value.base + (buffer & mask(distanceExtra));
            consume(distanceExtra);
            space -= length;
            // Integer offsets avoid forming a pointer before window.begin.
            // Retail wraps history repeatedly even for an oversized distance.
            const auto windowSize = window.end - window.begin;
            auto sourceOffset = (write - window.begin) - static_cast<std::ptrdiff_t>(back);
            while (sourceOffset < 0) sourceOffset += windowSize;
            uint8_t* source = window.begin + sourceOffset;
            for (uint32_t i = 0; i < length; ++i) {
                *write++ = *source++;
                if (source == window.end) source = window.begin;
            }
        }
    } while (space >= 258 && available >= 10);
    return finish(0);
}
