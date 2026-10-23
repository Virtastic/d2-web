// Prior art used: has no D2Client implementation to adopt. The
// D2Client pointer tables only name entry points; the decompile and
// provide the UI/string and renderer context. These bodies
// follow the cached 1.13c decompiles at build-wasm/ghidra-d2client/decomp/0x2C8C0.c
// through 0x2D3C0.c. Retail-only Ghidra output remains outside the repository.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <initializer_list>
#include <string>

namespace d2client_batch_2c8c0 {
using Address = uint32_t; // retail guest address; never a host pointer

struct Host {
    void* context;
    uint8_t* (*memory)(void*, Address);
    uintptr_t (*invoke)(void*, uint32_t, const uintptr_t*, size_t);

    template<class T> T read(Address at) const {
        T value{};
        std::memcpy(&value, memory(context, at), sizeof(value));
        return value;
    }
    template<class T> void write(Address at, T value) const {
        std::memcpy(memory(context, at), &value, sizeof(value));
    }
    uintptr_t call(uint32_t rva, std::initializer_list<uintptr_t> args = {}) const {
        return invoke(context, rva, args.begin(), args.size());
    }
    uintptr_t callThis(uint32_t rva, uintptr_t thisPointer,
                       std::initializer_list<uintptr_t> args = {}) const {
        uintptr_t encoded[16]{};
        encoded[0] = thisPointer;
        size_t n = 1;
        for (uintptr_t arg : args) {
            if (n == sizeof(encoded) / sizeof(encoded[0])) break;
            encoded[n++] = arg;
        }
        return invoke(context, rva, encoded, n);
    }
    uint32_t word(Address at) const { return read<uint32_t>(at); }
    void word(Address at, uint32_t value) const { write(at, value); }
};

constexpr Address kBase = 0x6FAB0000;
static uint32_t global(const Host& h, uint32_t rva) { return h.word(kBase + rva); }
static void setGlobal(const Host& h, uint32_t rva, uint32_t v) { h.word(kBase + rva, v); }
static uintptr_t out(const void* p) { return reinterpret_cast<uintptr_t>(p); }

static void appendRef(const Host& h, Address destination, uint32_t stringRef) {
    // D2Lang #10003 is the client import thunk at +0xD1EC.
    const Address source = static_cast<Address>(h.call(0xD1EC, {stringRef}));
    if (!source || !destination) return;
    Address end = destination;
    while (h.read<uint16_t>(end)) end += 2;
    for (Address p = source; p && h.read<uint16_t>(p); p += 2, end += 2)
        h.write<uint16_t>(end, h.read<uint16_t>(p));
    h.write<uint16_t>(end, 0);
}

static void appendCurrentRef(const Host& h, Address destination) {
    const Address source = static_cast<Address>(h.call(0xD1EC));
    if (!source || !destination) return;
    Address end = destination;
    while (h.read<uint16_t>(end)) end += 2;
    for (Address p = source; h.read<uint16_t>(p); p += 2, end += 2)
        h.write<uint16_t>(end, h.read<uint16_t>(p));
    h.write<uint16_t>(end, 0);
}

static void appendLocal(const Host& h, Address destination, const char16_t* source) {
    if (!destination || !source) return;
    Address end = destination;
    while (h.read<uint16_t>(end)) end += 2;
    for (size_t i = 0; source[i]; ++i, end += 2)
        h.write<uint16_t>(end, static_cast<uint16_t>(source[i]));
    h.write<uint16_t>(end, 0);
}

static void appendLocalFromGuest(const Host& h, char16_t* destination, size_t capacity,
                                 Address source) {
    size_t end = 0;
    while (end + 1 < capacity && destination[end]) ++end;
    for (size_t i = 0; source && end + 1 < capacity; ++i) {
        uint16_t ch = h.read<uint16_t>(source + static_cast<Address>(i * 2));
        if (!ch) break;
        destination[end++] = static_cast<char16_t>(ch);
    }
    destination[end] = 0;
}

static std::u16string formatNumber(int32_t lo, int32_t hi, bool plusSingle = false) {
    char ascii[48]{};
    if (lo == hi) {
        if (plusSingle && lo > 0) std::snprintf(ascii, sizeof(ascii), "+%d", lo);
        else std::snprintf(ascii, sizeof(ascii), "%d", lo);
    } else {
        std::snprintf(ascii, sizeof(ascii), "%d-%d", lo, hi);
    }
    std::u16string wide;
    size_t n = std::strlen(ascii);
    if (n >= 48) n = 47;
    wide.reserve(n);
    for (size_t i = 0; i < n; ++i) wide.push_back(static_cast<unsigned char>(ascii[i]));
    return wide;
}

// 1.13c D2Client+0x2C8C0 (144 B). The caller leaves the level descriptor in
// EBX. Scan SkillsTxt rows (stride 0x23C), compare the row's +0x0C byte to
// descriptor+4, then validate the matching entry through ordinals 10708/10306.
uint32_t checkSkillLevelEntry(const Host& h, Address levelDescriptor) {
    if (global(h, 0x1234A4)) return 1;
    const Address tables = h.word(kBase + 0xCE474); // sgptDataTables import cell
    if (!tables) return 0;
    const int32_t count = h.read<int32_t>(tables + 0xBA0);
    const Address rows = h.word(tables + 0xB98);
    const uint8_t level = levelDescriptor ? h.read<uint8_t>(levelDescriptor + 4) : 0xFFu;
    for (int32_t i = 0; i < count; ++i) {
        Address row = rows + static_cast<uint32_t>(i) * 0x23C;
        if (h.read<uint8_t>(row + 0x0C) != level) continue;
        if (h.call(0xC4EA, {0xFFFFFFFFu}) && h.call(0xC718)) {
            setGlobal(h, 0x1234A4, 1);
            return 1;
        }
    }
    return 0;
}

// 1.13c D2Client+0x2C950 (153 B). Resolve param_1 through D2Lang #10003,
// format the result into a zeroed 100-unit Unicode local, append both current
// localized suffixes, then show it through D2Win #10085 at ctx-0x34, 0, 1.
void appendDescriptorText(const Host& h, uint32_t stringRef) {
    char16_t formatted[200]{};
    const Address format = static_cast<Address>(h.call(0xD1EC, {stringRef}));
    appendLocalFromGuest(h, formatted, 100, format);
    appendLocalFromGuest(h, formatted, 100, static_cast<Address>(h.call(0xD1EC)));
    appendLocalFromGuest(h, formatted, 100, static_cast<Address>(h.call(0xD1EC)));
    h.callThis(0xD384, out(formatted), {global(h, 0xDBC4C) - 0x34, 0, 1});
}

// 1.13c D2Client+0x2C9F0 (144 B). 0x1506 is the rejected sentinel. Resolve
// param_2 through D2Lang #10003, format it into a zeroed 0x100-unit Unicode
// local, then concatenate that bounded local and one localized suffix to EBX.
uint32_t appendSingleValue(const Host& h, Address destination, int16_t selector, uint32_t value) {
    if (selector == 0x1506) return 0;
    char16_t formatted[256]{};
    const Address format = static_cast<Address>(h.call(0xD1EC, {value}));
    appendLocalFromGuest(h, formatted, 256, format);
    appendLocal(h, destination, formatted);
    appendCurrentRef(h, destination);
    return 1;
}

// 1.13c D2Client+0x2CA80 (212 B). BX != 0x1506 and param_2 != 0 gate the
// record. The inline formatter at +0xC7E0 writes a 32-unit local value; five
// localized separators precede it, then param_4 conditionally adds one more.
uint32_t appendRecordValue(const Host& h, Address destination, int16_t selector,
                           uint32_t enabled, Address record, int32_t optional) {
    if (selector == 0x1506 || !enabled) return 0;
    char16_t formatted[64]{};
    h.call(0x2C7E0, {record, out(formatted)});
    for (int i = 0; i < 5; ++i) appendCurrentRef(h, destination);
    appendLocal(h, destination, formatted);
    if (optional) appendCurrentRef(h, destination);
    appendCurrentRef(h, destination);
    return 1;
}

// 1.13c D2Client+0x2CB60 (158 B). Both sentinels reject the operation; on
// success the function appends the fixed localized label sequence to EDI.
uint32_t appendPairLabel(const Host& h, Address destination, int16_t left, int16_t right) {
    if (left == 0x1506 || right == 0x1506) return 0;
    for (int i = 0; i < 5; ++i) appendCurrentRef(h, destination);
    return 1;
}

// 1.13c D2Client+0x2CC00 (282 B). Requires a non-sentinel selector, non-null
// source record and positive amount. The amount is rendered in decimal between
// the localized value label and suffixes; the record formatter is +0xC7E0.
uint32_t appendCountedRecord(const Host& h, Address destination, int16_t selector,
                             uint32_t ignored, Address record, int32_t amount) {
    if (selector == 0x1506 || !record || amount <= 0) return 0;
    char16_t formatted[64]{};
    h.call(0x2C7E0, {record, out(formatted)});
    for (int i = 0; i < 3; ++i) appendCurrentRef(h, destination);
    appendLocal(h, destination, formatted);
    appendCurrentRef(h, destination);
    char ascii[24]{};
    std::snprintf(ascii, sizeof(ascii), "%d", amount);
    char16_t number[24]{};
    for (size_t i = 0; ascii[i] && i + 1 < 24; ++i) number[i] = static_cast<unsigned char>(ascii[i]);
    appendLocal(h, destination, number);
    for (int i = 0; i < 3; ++i) appendCurrentRef(h, destination);
    (void)ignored;
    return 1;
}

// 1.13c D2Client+0x2CD20 (227 B). Uses +0xC790 to build the param_3/param_4
// value. Either param_2 or param_3 must be nonzero; param_5 controls one extra
// localized suffix. EBX is the active destination string.
uint32_t appendRangeRecord(const Host& h, Address destination, int16_t selector,
                           int32_t low, int32_t high, int32_t unit, int32_t suffix) {
    if (selector == 0x1506 || (!low && !high)) return 0;
    char16_t formatted[64]{};
    h.call(0x2C790, {static_cast<uint32_t>(high), static_cast<uint32_t>(unit), out(formatted)});
    for (int i = 0; i < 3; ++i) appendCurrentRef(h, destination);
    appendLocal(h, destination, formatted);
    if (suffix) appendCurrentRef(h, destination);
    for (int i = 0; i < 3; ++i) appendCurrentRef(h, destination);
    return 1;
}

// 1.13c D2Client+0x2CF20 (214 B). Build a decimal point or inclusive range;
// (0,0) is omitted. The localized leading and trailing pieces stay data-driven.
uint32_t appendPlainRange(const Host& h, Address destination, int32_t low, int32_t high) {
    if (low == 0 && high == 0) return 0;
    std::u16string number = formatNumber(low, high);
    appendCurrentRef(h, destination);
    appendCurrentRef(h, destination);
    appendLocal(h, destination, number.c_str());
    appendCurrentRef(h, destination);
    appendCurrentRef(h, destination);
    return 1;
}

// 1.13c D2Client+0x2D000 (542 B). Scale both endpoints by the signed
// percentage in EBX, preserving the retail branch's divide-before-multiply
// optimization for large values. Then add the shared offset and append the
// signed single value or range. Return low word 1 when text was emitted.
uint64_t appendScaledRange(const Host& h, Address destination, int32_t low, int32_t high,
                           int32_t scale, int32_t offset, int32_t sharedOffset) {
    auto scaled = [scale](int32_t value) -> int32_t {
        if (scale < 0x10001) return static_cast<int32_t>((value * scale) / 100);
        const int32_t maskedScale = scale & static_cast<int32_t>(0xFFFFFFF0u);
        if (maskedScale < 0x640)
            return static_cast<int32_t>((int64_t(value) * scale) / 100);
        return static_cast<int32_t>((scale / 100) * value);
    };
    int32_t a = static_cast<int32_t>(static_cast<uint32_t>(low) + scaled(low) + offset);
    int32_t b = static_cast<int32_t>(static_cast<uint32_t>(high) + scaled(high) + offset);
    if (a == 0 && b == 0) return uint64_t(static_cast<uint32_t>(sharedOffset)) << 32;
    std::u16string number = formatNumber(a, b, true);
    appendCurrentRef(h, destination);
    appendLocal(h, destination, number.c_str());
    appendCurrentRef(h, destination);
    return (uint64_t(static_cast<uint32_t>(sharedOffset)) << 32) | 1u;
}

// 1.13c D2Client+0x2D220 (213 B). If AX is zero, +0xC870 decides whether the
// range is active. Append either one value or the low-high range and return 1.
uint32_t appendConditionalRange(const Host& h, Address destination, int16_t enabled,
                               int32_t low, int32_t high) {
    if (enabled == 0 && static_cast<int16_t>(h.call(0x2C870)) == 0) return 0;
    std::u16string number = formatNumber(low, high);
    appendCurrentRef(h, destination);
    appendLocal(h, destination, number.c_str());
    appendCurrentRef(h, destination);
    return 1;
}

// 1.13c D2Client+0x2D300 (183 B). Format param_1 as decimal, initialize the
// D2Win text state, then draw at (viewportWidth + viewportOrigin - 0x1E0 +
// param_3, param_1, 0). The local UTF-16 buffer is the decompile's this value.
void drawSmallValue(const Host& h, int32_t value, int32_t ignored, int32_t xOffset) {
    char16_t text[64]{};
    char ascii[32]{};
    std::snprintf(ascii, sizeof(ascii), "%d", value);
    for (size_t i = 0; ascii[i] && i + 1 < 64; ++i) text[i] = static_cast<unsigned char>(ascii[i]);
    h.call(0xD3A8);
    const int32_t x = static_cast<int32_t>(global(h, 0x11B9A4) + global(h, 0xDBC4C)) - 0x1E0 + xOffset;
    h.callThis(0xD35A, out(text));
    h.callThis(0xD37E, out(text), {static_cast<uint32_t>(x), static_cast<uint32_t>(value), 0});
    (void)ignored;
}

// 1.13c D2Client+0x2D3C0 (257 B). The first call initializes text state. On
// the first non-guarded draw, locale ordinal 10009 selects y=-6 for language
// ids below 6 or above 9, otherwise y=-7; that offset is cached in the global.
// Retail then draws param_6 at the two viewport-relative x anchors.
void drawVersionLabels(const Host& h, int32_t x1, uint32_t ignored, int32_t x3,
                       int32_t y, int32_t ignored2, uint32_t text) {
    h.call(0xD3A8);
    if (!h.read<uint8_t>(kBase + 0x1234A8)) {
        int32_t language = static_cast<int32_t>(h.call(0xD1E6));
        if (language < 6) setGlobal(h, 0xF9F70, uint32_t(-6));
        else {
            language = static_cast<int32_t>(h.call(0xD1E6));
            setGlobal(h, 0xF9F70, language > 9 ? uint32_t(-6) : uint32_t(-7));
        }
        h.write<uint8_t>(kBase + 0x1234A8, 1);
    }
    const int32_t baseX = static_cast<int32_t>(global(h, 0x11B9A4) + global(h, 0xDBC4C));
    h.callThis(0xD35A, text);
    h.callThis(0xD37E, text, {uint32_t(baseX + static_cast<int32_t>(global(h, 0xF9F70)) - 0x1E0 + y), uint32_t(y), 0});
    const int32_t secondX = static_cast<int32_t>(global(h, 0xF9F6C) + baseX);
    h.callThis(0xD35A, text);
    h.callThis(0xD37E, text, {uint32_t(secondX - 0x1E0 + x1), uint32_t(y), 0});
    (void)ignored;
    (void)x3;
    (void)ignored2;
}

// 1.13c D2Client+0x2D8E0 (201 B). ret 0x14; EDI = guest Unicode destination.
// Stack a1..a5 = key1, key2, key3, value, plus flag. value 0 -> return 0. Format
// "+%d" when plus != 0 and value >= 0, else "%d"; append key1 text, the number,
// key2 text, key3 text, then string 0xF9E (each via D2Lang #10003); return 1.
uint32_t appendSignedValue(const Host& h, Address edi, uint32_t key1, uint32_t key2,
                           uint32_t key3, int32_t value, uint32_t plus) {
    if (!value) return 0;
    char ascii[32]{};
    std::snprintf(ascii, sizeof(ascii), (plus && value >= 0) ? "+%d" : "%d", value);
    char16_t number[64]{};
    for (size_t i = 0; ascii[i]; ++i) number[i] = static_cast<unsigned char>(ascii[i]);
    appendRef(h, edi, key1);
    appendLocal(h, edi, number);
    appendRef(h, edi, key2);
    appendRef(h, edi, key3);
    appendRef(h, edi, 0xF9E);
    return 1;
}

} // namespace d2client_batch_2c8c0

namespace d2client_batch_2c8c0 {
// 1.13c D2Client+0x2D9B0 (183 B). ret 0x10; EBX = guest Unicode destination.
// Stack: key1, key2, value, plus. value 0 -> 0. Format "+%d" when plus and
// value >= 0 else "%d" into a 32-unit local; append key1 text, number, key2
// text, then string 0xF9E (each via D2Lang #10003); return 1.
uint32_t appendSignedValue2(const Host& h, Address ebx, uint32_t key1, uint32_t key2,
                            int32_t value, uint32_t plus) {
    if (!value) return 0;
    char ascii[32]{};
    std::snprintf(ascii, sizeof(ascii), (plus && value >= 0) ? "+%d" : "%d", value);
    char16_t number[32]{};
    for (size_t i = 0; ascii[i] && i < 31; ++i) number[i] = static_cast<unsigned char>(ascii[i]);
    appendRef(h, ebx, key1);
    appendLocal(h, ebx, number);
    appendRef(h, ebx, key2);
    appendRef(h, ebx, 0xF9E);
    return 1;
}
} // namespace d2client_batch_2c8c0

namespace d2client_batch_2c8c0 {
// 1.13c D2Client+0x2DA70 (177 B). ret 0x10; EBP = guest Unicode destination, EBX = value;
// stack: arg0 (unused), strA, strB, plus. value 0 -> 0. Else "+%d" (plus && value >= 0) or "%d",
// append it, then text(value==1 ? strA : strB), then text 0xF9E (each via D2Lang #10003); return 1.
uint32_t appendSignedValue3(const Host& h, Address ebp, int32_t ebx, uint32_t /*arg0*/,
                            uint32_t strA, uint32_t strB, uint32_t plus) {
    if (!ebx) return 0;
    char ascii[32]{};
    std::snprintf(ascii, sizeof(ascii), (plus && ebx >= 0) ? "+%d" : "%d", ebx);
    char16_t number[32]{};
    for (size_t i = 0; ascii[i] && i < 31; ++i) number[i] = static_cast<unsigned char>(ascii[i]);
    appendLocal(h, ebp, number);
    appendRef(h, ebp, ebx == 1 ? strA : strB);
    appendRef(h, ebp, 0xF9E);
    return 1;
}
// 1.13c D2Client+0x2DA70 (177 B). value==0 -> 0. Else format "+%d" (plus!=0 && value>=0) or "%d",
// append it to dest, then the localized string (value==1 ? strA : strB) and string 0xF9E; return 1.
uint32_t appendSignedValue(const Host& h, Address dest, int32_t value, uint32_t strA, uint32_t strB, uint32_t plus) {
    if (value == 0) return 0;
    char16_t text[32]{};
    char ascii[32];
    std::snprintf(ascii, sizeof ascii, (plus && value >= 0) ? "+%d" : "%d", value);
    for (size_t i = 0; ascii[i] && i < 31; ++i) text[i] = static_cast<unsigned char>(ascii[i]);
    appendLocal(h, dest, text);
    appendRef(h, dest, value == 1 ? strA : strB);
    appendRef(h, dest, 0xF9E);
    return 1;
}

} // namespace d2client_batch_2c8c0
