// D2Lang string tables (.tbl): string.tbl < expansionstring.tbl < patchstring.tbl, looked up by
// reference key exactly as D2LANG_GetStringByReferenceString does -- the ELF-style hash, then
// linear probing up to HashMaxTries, patch first. 
//
// Prior art used: OpenDiablo2 d2tbl (layout), riiablo StringTBL.java (hash + probe order).
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace d2 {

class StrTable {
public:
    // Parses one .tbl. False on a malformed header.
    bool load(const uint8_t* data, size_t len);
    // Exact-key lookup with retail's probe. nullptr when absent (retail then falls back to the
    // next table, and finally to the key itself in angle brackets -- see LangTables::get).
    const std::u16string* find(const std::string& key) const;
    // The entry's own index (the .tbl's u16 at entry+1), or -1: what D2LANG_GetTblIndex returns
    // before the per-table offset (string 0+, patch 10000+, expansion 20000+).
    int findIndex(const std::string& key) const;
    // The entry carrying a given .tbl index (D2LANG_GetStringFromTblIndex's per-table half).
    const std::u16string* findByIndex(uint16_t index) const;
    size_t size() const { return entries_.size(); }
private:
    struct Entry { bool active; uint16_t index; std::string key; std::u16string value; };
    const Entry* lookup(const std::string& key) const;
    std::vector<Entry> entries_;   // hash-table slots, in file order
    uint32_t maxTries_ = 0;
};

// The three tables a retail client has open, in lookup priority order.
class LangTables {
public:
    StrTable patch, expansion, classic;
    // patch -> expansion -> classic; nullptr if none has it.
    const std::u16string* find(const std::string& key) const;
    // D2LANG_GetTblIndex: patch (10000+) > expansion (20000+) > string (0+); 0 when absent.
    uint16_t tblIndex(const std::string& key, const std::u16string** value = nullptr) const;
    // D2LANG_GetStringFromTblIndex: the inverse of tblIndex (10000+ patch, 20000+ expansion).
    // `table` (0 classic, 1 patch, 2 expansion) and `inTable` report the table an index dispatches
    // to and the index looked up inside it: 1.13c D2Lang+0x9450's whole content.
    const std::u16string* byTblIndex(uint16_t index, int* table = nullptr, uint16_t* inTable = nullptr) const;
};

// UTF-8 -> UTF-16 the way D2Lang's Unicode::utf8ToUnicode does for the 1-3 byte forms.
std::u16string utf8ToU16(const std::string& s);

} // namespace d2
