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
