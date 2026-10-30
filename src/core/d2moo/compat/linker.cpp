// Compat: Fog's excel "linkers" and the .bin side of DATATBLS_CompileTxt.
//
// A linker maps a table's key (a 4-char code, or a row name) to its row index; the D2MOO loaders
// build one per table (FOG_10215 / FOG_10216 append in row order) and every code-to-index fixup
// queries it (FOG_GetLinkIndex / FOG_GetRowFromTxt). First match wins, like a linear scan would.
// Names compare case-insensitively (Blizzard's txt references are not case-consistent); codes
// compare exactly (item codes are case-sensitive: "amu" vs "AMU" do not collide in retail data).
//
// DATATBLS_CompileTxt is vendored (DataTbls.cpp) and, with bCompileTxt off and LoadFromBin on,
// reads data\global\excel\<name>.bin through ARCHIVE_ALLOC_BUFFER_AND_READ_FILE_TO_IT -- i.e. our
// VFS -- so the only Fog pieces it needs are the D2BinFileStrc accessors below.
#include <D2Common.h>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

namespace {
struct Linker {
    std::vector<std::string> names;          // index -> key (code as 4 chars, or name)
    std::unordered_map<std::string, int> byName;   // lower-cased name -> first index
    std::unordered_map<uint32_t, int> byCode;      // code -> first index
};
std::string lower(const char* s) { std::string r(s ? s : ""); for (char& c : r) c = (char)tolower((unsigned char)c); return r; }
}

void* FOG_AllocLinker(const char*, int) { return new Linker(); }
void  FOG_FreeLinker(void* p) { delete (Linker*)p; }

int FOG_10215(void* p, int dwCode) {
    Linker* l = (Linker*)p; int idx = (int)l->names.size();
    char c[5] = {}; memcpy(c, &dwCode, 4); l->names.push_back(c);
    l->byCode.emplace((uint32_t)dwCode, idx);
    l->byName.emplace(lower(c), idx);
    return idx;
}
int FOG_10216_AddRecordToLinkingTable(void* p, const char* szName) {
    Linker* l = (Linker*)p; int idx = (int)l->names.size();
    l->names.push_back(szName ? szName : "");
    l->byName.emplace(lower(szName), idx);
    if (szName && strlen(szName) <= 4) { uint32_t code = 0; memcpy(&code, szName, strlen(szName)); l->byCode.emplace(code, idx); }
    return idx;
}
int FOG_GetLinkIndex(void* p, uint32_t dwCode, BOOL) {
    if (!p || !dwCode) return -1;
    Linker* l = (Linker*)p; auto it = l->byCode.find(dwCode); return it == l->byCode.end() ? -1 : it->second;
}
int FOG_GetRowFromTxt(void* p, char* szText, int) {
    if (!p || !szText || !*szText) return -1;
    Linker* l = (Linker*)p; auto it = l->byName.find(lower(szText)); return it == l->byName.end() ? -1 : it->second;
}
char* FOG_10255(void* p, int nId, int) {
    Linker* l = (Linker*)p;
    if (!l || nId < 0 || nId >= (int)l->names.size()) { static char none[1] = {}; return none; }
    return (char*)l->names[nId].c_str();
}
int FOG_GetStringFromLinkIndex(void* p, int nIndex, char* szString) {
    const char* s = FOG_10255(p, nIndex, 0); strcpy(szString, s); return (int)strlen(s);
}

// A table is either a compiled .bin (4-byte record count, then the records) or the tab-separated .txt
// it was compiled from (header row, then one row per record). A patch that ships the text without the .bin
// gets it compiled here; FOG_10207 compiles it against the field table.
namespace {
struct TextTable { std::vector<std::string> head; std::vector<std::vector<std::string>> rows; };
const int kTextTable = -1;   // D2BinFileStrc::nCellCount marks a parsed text table; pDataBuffer holds the TextTable

bool looksLikeText(const uint8_t* p, size_t n) {
    // A compiled .bin is a record count and packed fields; a text table is mostly printable with a tab-separated header.
    size_t m = n < 512 ? n : 512, ok = 0; bool tab = false;
    for (size_t i = 0; i < m; i++) { uint8_t c = p[i]; if (c == '\t') tab = true; if (c >= 0x20 || c == '\t' || c == '\r' || c == '\n') ok++; }
    return tab && ok * 10 >= m * 9;
}
std::vector<std::string> splitCells(const char* b, const char* e) {
    std::vector<std::string> c; const char* s = b;
    for (const char* p = b; p <= e; p++) if (p == e || *p == '\t') { c.emplace_back(s, p - s); s = p + 1; }
    return c;
}
TextTable* parseText(const uint8_t* d, size_t n) {
    TextTable* t = new TextTable();
    const char* p = (const char*)d; const char* end = p + n; bool first = true;
    while (p < end) {
        const char* nl = (const char*)memchr(p, '\n', end - p); const char* le = nl ? nl : end;
        const char* ce = le; if (ce > p && ce[-1] == '\r') ce--;
        if (ce > p) {
            auto cells = splitCells(p, ce);
            if (first) { for (auto& h : cells) h = lower(h.c_str()); t->head = std::move(cells); first = false; }
            else if (lower(cells[0].c_str()) != "expansion") t->rows.push_back(std::move(cells));
        }
        p = nl ? nl + 1 : end;
    }
    return t;
}
int toInt(const std::string& s) { return s.empty() ? 0 : (int)strtol(s.c_str(), nullptr, 10); }
uint32_t packCode(const std::string& s) { char c[4] = {' ', ' ', ' ', ' '}; for (size_t i = 0; i < 4 && i < s.size(); i++) c[i] = s[i]; uint32_t v; memcpy(&v, c, 4); return v; }
typedef void (*FieldFn)(char*, void*, int, int, int, int);
}

D2BinFileStrc* FOG_CreateBinFile(void* pData, size_t dwSize) {
    if (!pData || dwSize < 4) return nullptr;
    D2BinFileStrc* f = (D2BinFileStrc*)calloc(1, sizeof(D2BinFileStrc));
    if (looksLikeText((const uint8_t*)pData, dwSize)) {
        TextTable* t = parseText((const uint8_t*)pData, dwSize);
        f->pDataBuffer = (uint8_t*)t; f->nRecordCount = (int32_t)t->rows.size(); f->nCellCount = kTextTable;
        return f;
    }
    f->pDataBuffer = (uint8_t*)pData; f->pData = (uint8_t*)pData + 4;
    memcpy(&f->nRecordCount, pData, 4); f->nCellCount = (int32_t)(dwSize - 4);
    return f;
}
int  FOG_GetRecordCountFromBinFile(D2BinFileStrc* f) { return f ? f->nRecordCount : 0; }
void FOG_FreeBinFile(D2BinFileStrc* f) { if (f && f->nCellCount == kTextTable) delete (TextTable*)f->pDataBuffer; free(f); }

void FOG_10207(D2BinFileStrc* f, D2BinFieldStrc* tbl, void* dest, int nRecords, size_t recSize) {
    if (!f || f->nCellCount != kTextTable) { char m[96]; snprintf(m, sizeof m, "FOG_10207: not a text table (f=%p cells=%d)", (void*)f, f ? f->nCellCount : 0); FOG_DisplayHalt(m, __FILE__, __LINE__); return; }
    TextTable* t = (TextTable*)f->pDataBuffer;
    std::unordered_map<std::string, int> col;
    for (size_t i = 0; i < t->head.size(); i++) col.emplace(t->head[i], (int)i);
    std::unordered_map<std::string, int> local;
    for (int r = 0; r < nRecords; r++) {
        uint8_t* rec = (uint8_t*)dest + (size_t)r * recSize;
        const auto& row = t->rows[r];
        for (D2BinFieldStrc* fd = tbl; fd->nFieldType != TXTFIELD_NONE; fd++) {
            auto it = col.find(lower(fd->szFieldName));
            std::string cell = it != col.end() && (size_t)it->second < row.size() ? row[it->second] : std::string();
            uint8_t* at = rec + fd->nFieldOffset;
            void* link = fd->pLinkField;
            void* linker = link ? *(void**)link : nullptr;   // pLinkField of a NAME/CODE field is the address of the table's linker pointer
            auto lookup = [&](bool code) -> int {
                if (cell.empty()) return 0;   // a blank cell leaves the zeroed record alone, as retail does
                if (!linker) return -1;
                if (((Linker*)linker)->names.empty()) { auto m = local.find(lower(cell.c_str())); return m == local.end() ? -1 : m->second; }   // the table's own linker is built after the compile: names seen so far in the key column
                return code ? FOG_GetLinkIndex(linker, packCode(cell), 0) : FOG_GetRowFromTxt(linker, (char*)cell.c_str(), 0);
            };
            switch (fd->nFieldType) {
            case TXTFIELD_ASCII: { size_t n = std::min<size_t>(cell.size(), fd->nFieldLength ? fd->nFieldLength - 1 : cell.size()); memcpy(at, cell.data(), n); at[n] = 0; break; }
            case TXTFIELD_DWORD: case TXTFIELD_DWORD2: { int32_t v = toInt(cell); memcpy(at, &v, 4); break; }
            case TXTFIELD_WORD: { int16_t v = (int16_t)toInt(cell); memcpy(at, &v, 2); break; }
            case TXTFIELD_BYTE: case TXTFIELD_BYTE2: case TXTFIELD_UNKNOWN2: *at = (uint8_t)toInt(cell); break;
            case TXTFIELD_RAW: case TXTFIELD_ASCIITOCODE: { uint32_t v = cell.empty() ? 0 : packCode(cell); memcpy(at, &v, 4); break; }
            case TXTFIELD_UNKNOWN3: { int32_t v = lookup(true); memcpy(at, &v, 4); break; }
            case TXTFIELD_CODETOBYTE: *at = (uint8_t)lookup(true); break;
            case TXTFIELD_CODETOWORD: { int16_t v = (int16_t)lookup(true); memcpy(at, &v, 2); break; }
            case TXTFIELD_NAMETOINDEX: case TXTFIELD_NAMETOINDEX2: if (linker && ((Linker*)linker)->names.empty() && !cell.empty()) { local.emplace(lower(cell.c_str()), r); int32_t v = r; memcpy(at, &v, 4); break; } [[fallthrough]];
            case TXTFIELD_NAMETODWORD: { int32_t v = lookup(false); memcpy(at, &v, 4); break; }
            case TXTFIELD_NAMETOWORD: case TXTFIELD_NAMETOWORD2: { int16_t v = (int16_t)lookup(false); memcpy(at, &v, 2); break; }
            case TXTFIELD_KEYTOWORD: { uint16_t v = cell.empty() ? 0 : ((uint16_t(*)(char*))fd->pLinkField.p)((char*)cell.c_str()); memcpy(at, &v, 2); break; }
            case TXTFIELD_CUSTOMLINK: case TXTFIELD_CALCTODWORD:
                ((FieldFn)fd->pLinkField.p)(cell.empty() ? nullptr : (char*)cell.c_str(), rec, fd->nFieldOffset, 0, r, it != col.end() ? it->second : -1);
                break;
            case TXTFIELD_BIT: if (toInt(cell)) { uint32_t v; memcpy(&v, at, 4); v |= 1u << fd->nFieldLength; memcpy(at, &v, 4); } break;
            default: break;
            }
        }
    }
}
