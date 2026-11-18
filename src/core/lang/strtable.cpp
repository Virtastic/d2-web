#include "strtable.h"
#include <cstring>

namespace d2 {

static uint16_t rd16(const uint8_t* p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t* p) { return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24)); }

extern "C" int d2lang_8c40(const uint8_t* src, int len, uint16_t* out);   // 1.13c D2Lang+0x8C40, one UTF-8 char

std::u16string utf8ToU16(const std::string& s) {
    std::u16string out;
    const int len = (int)s.size() + 1;
    for (size_t i = 0; i < s.size();) {
        uint16_t u = 0;
        int n = d2lang_8c40((const uint8_t*)s.data() + i, len - (int)i, &u);
        if (n < 1) { out.push_back((unsigned char)s[i]); i++; }   // invalid sequence: the byte is taken as-is
        else { out.push_back((char16_t)u); i += n; }
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
    raw_.assign(d, d + len);
    cache_.assign(hashSize, 0);
    for (uint32_t i = 0; i < hashSize; i++, pos += 17) {
        const uint8_t* e = d + pos;
        Entry& en = entries_[i];
        en.active = e[0] != 0;
        if (!en.active) continue;
        cache_[i] = i + 1;   // the decoded-string cache slot, as a handle into entries_ (our pointers are 64-bit)
        en.index = rd16(e + 1);
        uint32_t keyOff = rd32(e + 7), valOff = rd32(e + 11); uint16_t valLen = rd16(e + 15);
        if (keyOff >= len || valOff + valLen > len) continue;
        en.key.assign((const char*)d + keyOff, strnlen((const char*)d + keyOff, len - keyOff));
        std::string v((const char*)d + valOff, valLen ? valLen - 1 : 0);   // valLen counts the NUL
        en.value = utf8ToU16(v);
    }
    return true;
}

// 1.13c D2Lang+0x9360 (key to node index + cache slot) and +0x9050 (id to cache slot): the proven ports.
extern "C" uint32_t d2lang_9360_find(const uint8_t* tbl, const uint32_t* cache, const uint8_t* key, uint32_t* out);
extern "C" uint32_t d2lang_9050_lookup(const uint8_t* tbl, uint16_t id, const uint32_t* cache);

const StrTable::Entry* StrTable::lookup(const std::string& key, int* index) const {
    if (entries_.empty()) return nullptr;
    uint32_t out = 0;
    uint32_t r = d2lang_9360_find(raw_.data(), cache_.data(), (const uint8_t*)key.c_str(), &out);
    if (r == 0xffff) return nullptr;
    if (index) *index = (int)r;
    return &entries_[out - 1];
}
// 1.13c D2Lang+0x9050 reaches the node through the index-to-slot map. The callee's fatal exits (slot out of
// range, node not active, no cached string) are tested first here and read as a miss.
const std::u16string* StrTable::byId(uint16_t id) const {
    if (raw_.size() < 0x15) return nullptr;
    const uint8_t* t = raw_.data();
    uint32_t i = id;
    if (rd16(t + 2) <= id) i = 500;
    size_t mapAt = 0x15 + (size_t)i * 2;
    if (mapAt + 2 > raw_.size()) return nullptr;
    i = rd16(t + mapAt);
    if ((int32_t)i >= (int32_t)rd32(t + 4) || i >= cache_.size()) return nullptr;
    size_t nodeAt = 0x15 + (size_t)rd16(t + 2) * 2 + (size_t)i * 0x11;
    if (nodeAt >= rd32(t + 0x11) || nodeAt >= raw_.size() || t[nodeAt] != 1 || cache_[i] == 0) return nullptr;
    return &entries_[d2lang_9050_lookup(t, id, cache_.data()) - 1].value;
}
const std::u16string* StrTable::find(const std::string& key) const { const Entry* e = lookup(key); return e ? &e->value : nullptr; }
int StrTable::findIndex(const std::string& key) const { int i = -1; return lookup(key, &i) ? i : -1; }
const std::u16string* StrTable::findByIndex(uint16_t index) const { return byId(index); }
// 1.13c D2Lang+0x9450: a miss in the named window drops to the next table with the ORIGINAL index.
const std::u16string* LangTables::byTblIndex(uint16_t index, int* table, uint16_t* inTable) const {
    if (table) *table = index >= 20000 ? 2 : index >= 10000 ? 1 : 0;
    if (inTable) *inTable = (uint16_t)(index >= 20000 ? index - 20000 : index >= 10000 ? index - 10000 : index);
    const std::u16string* v;
    if (index >= 20000 && (v = expansion.findByIndex((uint16_t)(index - 20000)))) return v;
    if (index >= 10000 && (v = patch.findByIndex((uint16_t)(index - 10000)))) return v;
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
