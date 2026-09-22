#include "strtable.h"
#include <cstring>

namespace d2 {

static uint16_t rd16(const uint8_t* p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t* p) { return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24)); }

std::u16string utf8ToU16(const std::string& s) {
    std::u16string out;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = (unsigned char)s[i];
        if (c < 0x80) { out.push_back(c); i++; }
        else if ((c & 0xE0) == 0xC0 && i + 1 < s.size()) { out.push_back((char16_t)(((c & 0x1F) << 6) | (s[i + 1] & 0x3F))); i += 2; }
        else if ((c & 0xF0) == 0xE0 && i + 2 < s.size()) { out.push_back((char16_t)(((c & 0x0F) << 12) | ((s[i + 1] & 0x3F) << 6) | (s[i + 2] & 0x3F))); i += 3; }
        else { out.push_back(c); i++; }   // retail: invalid lead byte is taken as-is
    }
    return out;
}

bool StrTable::load(const uint8_t* d, size_t len) {
    // header: crc u16, numElements u16, hashTableSize u32, version u8, dataStart u32, maxTries u32, fileSize u32
    if (len < 21) return false;
    uint16_t numElements = rd16(d + 2);
    uint32_t hashSize = rd32(d + 4);
    maxTries_ = rd32(d + 13);
    size_t pos = 21 + (size_t)numElements * 2;              // skip the index -> slot table
    if (pos + (size_t)hashSize * 17 > len) return false;
    entries_.assign(hashSize, Entry{});
    for (uint32_t i = 0; i < hashSize; i++, pos += 17) {
        const uint8_t* e = d + pos;
        Entry& en = entries_[i];
        en.active = e[0] != 0;
        if (!en.active) continue;
        en.index = rd16(e + 1);
        uint32_t keyOff = rd32(e + 7), valOff = rd32(e + 11); uint16_t valLen = rd16(e + 15);
        if (keyOff >= len || valOff + valLen > len) { en.active = false; continue; }
        en.key.assign((const char*)d + keyOff, strnlen((const char*)d + keyOff, len - keyOff));
        std::string v((const char*)d + valOff, valLen ? valLen - 1 : 0);   // valLen counts the NUL
        en.value = utf8ToU16(v);
    }
    return true;
}

const StrTable::Entry* StrTable::lookup(const std::string& key) const {
    if (entries_.empty()) return nullptr;
    // ELF hash, as riiablo StringTBL.hash / retail STRTABLE_GetTblIndex
    uint32_t h = 0;
    for (unsigned char c : key) {
        h = (h << 4) + c;
        uint32_t t = h & 0xF0000000u;
        if (t) { h &= 0x0FFFFFFFu; h ^= t >> 24; }
    }
    uint32_t slot = h % (uint32_t)entries_.size();
    for (uint32_t tries = 0; tries < maxTries_; tries++) {
        const Entry& e = entries_[slot];
        if (!e.active) return nullptr;          // retail stops at the first empty slot
        if (e.key == key) return &e;
        slot = (slot + 1) % (uint32_t)entries_.size();
    }
    return nullptr;
}
const std::u16string* StrTable::find(const std::string& key) const { const Entry* e = lookup(key); return e ? &e->value : nullptr; }
int StrTable::findIndex(const std::string& key) const { const Entry* e = lookup(key); return e ? (int)e->index : -1; }
const std::u16string* StrTable::findByIndex(uint16_t index) const {
    for (const Entry& e : entries_) if (e.active && e.index == index) return &e.value;
    return nullptr;
}
const std::u16string* LangTables::byTblIndex(uint16_t index) const {
    if (index >= 20000) return expansion.findByIndex((uint16_t)(index - 20000));
    if (index >= 10000) return patch.findByIndex((uint16_t)(index - 10000));
    return classic.findByIndex(index);
}

const std::u16string* LangTables::find(const std::string& key) const {
    if (auto* v = patch.find(key)) return v;
    if (auto* v = expansion.find(key)) return v;
    return classic.find(key);
}

uint16_t LangTables::tblIndex(const std::string& key, const std::u16string** value) const {
    int i;
    if ((i = patch.findIndex(key)) >= 0)     { if (value) *value = patch.find(key);     return (uint16_t)(i + 10000); }
    if ((i = expansion.findIndex(key)) >= 0) { if (value) *value = expansion.find(key); return (uint16_t)(i + 20000); }
    if ((i = classic.findIndex(key)) >= 0)   { if (value) *value = classic.find(key);   return (uint16_t)i; }
    if (value) *value = nullptr;
    return 0;
}

} // namespace d2
