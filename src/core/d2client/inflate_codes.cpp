// Prior art used: D2MOO has no D2Client module. The zlib stream contract in
// reference/D2Ex2/libpng/zlib.h cross-checks status values; 1.13c establishes
// this resumable codes state machine and its circular-window transitions.
#include "inflate_codes.h"

// 1.13c D2Client+0x0E350
int32_t D2Client_InflateCodes(D2ClientInflateBlocks& blocks,
    D2ClientInflateInput& input, D2ClientInflateOutput& output, int32_t result) {
    auto& code = *blocks.sub.codes;
    auto& window = blocks.window;
    auto mask = [](uint32_t n) { return n == 32 ? ~uint32_t{0} : (uint32_t{1} << n) - 1; };
    auto fill = [&](uint32_t need) {
        while (blocks.bitCount < need) {
            if (!input.available) return false;
            --input.available;
            ++input.total;
            blocks.bitBuffer |= uint32_t{*input.next++} << (blocks.bitCount & 31);
            blocks.bitCount += 8;
            result = 0;
        }
        return true;
    };
    auto consume = [&](uint32_t n) {
        blocks.bitBuffer >>= n & 31;
        blocks.bitCount -= n;
    };
    auto flush = [&]() { return D2Client_FlushInflateWindow(window, output, result); };
    auto space = [&]() -> std::ptrdiff_t {
        return window.write < window.read ? window.read - window.write - 1 : window.end - window.write;
    };
    auto writable = [&]() {
        if (space()) return true;
        if (window.write == window.end && window.read != window.begin) window.write = window.begin;
        if (space()) return true;
        result = flush();
        if (window.write == window.end && window.read != window.begin) window.write = window.begin;
        return space() != 0;
    };
    for (;;) {
        switch (code.mode) {
        case 0:
            if (space() >= 258 && input.available >= 10) {
                result = D2Client_InflateFast(code.literalBits, code.distanceBits,
                    code.literalTree, code.distanceTree, blocks, input);
                if (result != 0) { code.mode = result == 1 ? 7 : 9; break; }
            }
            code.sub.code.tree = code.literalTree;
            code.sub.code.need = code.literalBits;
            code.mode = 1;
            break;
        case 1:
        case 3: {
            if (!fill(code.sub.code.need)) return flush();
            auto* entry = code.sub.code.tree + (blocks.bitBuffer & mask(code.sub.code.need));
            consume(entry->bits);
            const uint32_t operation = entry->operation;
            if (code.mode == 1 && operation == 0) {
                code.sub.literal = entry->value.base;
                code.mode = 6;
            } else if (operation & 16) {
                if (code.mode == 1) {
                    code.length = entry->value.base;
                    code.sub.copy.get = operation & 15;
                    code.mode = 2;
                } else {
                    code.sub.copy.get = operation & 15;
                    code.sub.copy.distance = entry->value.base;
                    code.mode = 4;
                }
            } else if (!(operation & 64)) {
                code.sub.code.tree = entry + entry->value.base;
                code.sub.code.need = operation;
            } else if (code.mode == 1 && (operation & 32)) {
                code.mode = 7;
            } else {
                input.message = code.mode == 1 ? "invalid literal/length code" : "invalid distance code";
                code.mode = 9;
                result = -3;
                return flush();
            }
            break;
        }
        case 2:
            if (!fill(code.sub.copy.get)) return flush();
            code.length += blocks.bitBuffer & mask(code.sub.copy.get);
            consume(code.sub.copy.get);
            code.sub.code.tree = code.distanceTree;
            code.sub.code.need = code.distanceBits;
            code.mode = 3;
            break;
        case 4:
            if (!fill(code.sub.copy.get)) return flush();
            code.sub.copy.distance += blocks.bitBuffer & mask(code.sub.copy.get);
            consume(code.sub.copy.get);
            code.mode = 5;
            break;
        case 5: {
            const auto size = window.end - window.begin;
            auto offset = (window.write - window.begin) - code.sub.copy.distance;
            while (offset < 0) offset += size;
            auto* source = window.begin + offset;
            while (code.length) {
                if (!writable()) return flush();
                *window.write++ = *source++;
                if (source == window.end) source = window.begin;
                --code.length;
                result = 0;
            }
            code.mode = 0;
            break;
        }
        case 6:
            if (!writable()) return flush();
            *window.write++ = static_cast<uint8_t>(code.sub.literal);
            result = 0;
            code.mode = 0;
            break;
        case 7:
            if (blocks.bitCount > 7) {
                blocks.bitCount -= 8;
                ++input.available;
                --input.next;
                --input.total;
            }
            result = flush();
            if (window.read != window.write) return flush();
            code.mode = 8;
            break;
        case 8: result = 1; return flush();
        case 9: result = -3; return flush();
        default: result = -2; return flush();
        }
    }
}
