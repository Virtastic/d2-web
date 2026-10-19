// Prior art used: D2MOO has no D2Client module; local 1.13c cached bodies are
// the implementation source. reference/D2Ex2/libpng/zlib.h cross-checks the
// stream counters/checksum contract. reference/d2gs-1.13c/LibD2Server/
// d2warden/RC4.cpp cross-checks KSA only; retail's state layout differs.
#include "stream_algorithms_wave_a.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <utility>

// 1.13c D2Client+0x0DB00
// ECX=destination pitch, EAX=source pitch, then buffers/rows/width.
// Remainder bytes precede dword copies; preserve order for aliased buffers.
void D2Client_CopyPitchedRows(uint8_t* destination, const uint8_t* source,
                            std::ptrdiff_t destinationPitch, std::ptrdiff_t sourcePitch,
                            uint32_t rows, uint32_t width) {
    assert(rows != 0); // retail do/while contract: positive row count
    do {
        auto* dst = destination;
        const auto* src = source;
        for (uint32_t i = 0; i < (width & 3); ++i) *dst++ = *src++;
        for (uint32_t i = 0; i < (width >> 2); ++i) {
            uint32_t word;
            std::memcpy(&word, src, 4);
            std::memcpy(dst, &word, 4);
            src += 4;
            dst += 4;
        }
        if (--rows == 0) break;
        destination += destinationPitch;
        source += sourcePitch;
    } while (true);
}

// 1.13c D2Client+0x0EA40
// Window begin/end/read/write +28/+2c/+30/+34, checkfn/check +38/+3c;
// stream next_out/avail_out/total_out/adler +c/+10/+14/+30. Native views
// preserve each mutation without packing host pointers into x86 fields.
// Check runs before each copy, including empty spans. End wraps both cursors
// when applicable. Callback seed and buffer are the zlib checkfn register args.
int32_t D2Client_FlushInflateWindow(D2ClientInflateWindow& window,
                                   D2ClientInflateOutput& output, int32_t result) {
    uint8_t* read = window.read;
    uint8_t* nextOut = output.next;
    auto flush = [&](uint8_t* limit) {
        const uint32_t count = std::min(output.available,
            static_cast<uint32_t>(limit - read));
        if (count != 0 && result == -5) result = 0;
        output.available -= count;
        output.total += count;
        if (window.check) {
            window.checksum = window.check(window.checksum, read, count);
            output.checksum = window.checksum;
        }
        uint32_t offset = 0;
        for (; count - offset >= 4; offset += 4) {
            uint32_t word;
            std::memcpy(&word, read + offset, 4);
            std::memcpy(nextOut + offset, &word, 4);
        }
        for (; offset < count; ++offset) nextOut[offset] = read[offset];
        nextOut += count;
        read += count;
    };
    flush(window.write < window.read ? window.end : window.write);
    if (read == window.end) {
        read = window.begin;
        if (window.write == window.end) window.write = window.begin;
        flush(window.write);
    }
    output.next = nextOut;
    window.read = read;
    return result;
}

// 1.13c D2Client+0x0F280
// Adler-32, retail's 5552-byte reduction blocks. Null is the init query;
// a non-null empty buffer preserves even an unreduced incoming checksum.
uint32_t D2Client_StreamAdler32(uint32_t checksum, const uint8_t* bytes, uint32_t size) {
    if (!bytes) return 1;
    uint32_t low = checksum & 0xffff;
    uint32_t high = checksum >> 16;
    while (size != 0) {
        uint32_t count = std::min(size, uint32_t{5552});
        size -= count;
        while (count-- != 0) {
            low += *bytes++;
            high += low;
        }
        low %= 65521;
        high %= 65521;
    }
    return (high << 16) | low;
}

// 1.13c D2Client+0xC78C0
// Mask key index to a byte before access, including keys longer than 256.
// Retail returns the quotient of the final next-index division.
uint32_t D2Client_InitStreamCipher(uint8_t (&state)[258], const uint8_t* key, uint32_t keyLength) {
    assert(keyLength != 0);
    for (uint32_t i = 0; i < 256; ++i) state[i] = static_cast<uint8_t>(i);
    state[256] = state[257] = 0;
    uint8_t j = 0;
    uint32_t keyIndex = 0;
    uint32_t next = 0;
    for (uint32_t i = 0; i < 256; ++i) {
        keyIndex &= 255;
        j = static_cast<uint8_t>(j + key[keyIndex] + state[i]);
        std::swap(state[i], state[j]);
        next = keyIndex + 1;
        keyIndex = next % keyLength;
    }
    return next / keyLength;
}
