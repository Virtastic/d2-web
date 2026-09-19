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

D2BinFileStrc* FOG_CreateBinFile(void* pData, size_t dwSize) {
    if (!pData || dwSize < 4) return nullptr;
    D2BinFileStrc* f = (D2BinFileStrc*)calloc(1, sizeof(D2BinFileStrc));
    f->pDataBuffer = (uint8_t*)pData; f->pData = (uint8_t*)pData + 4;
    memcpy(&f->nRecordCount, pData, 4); f->nCellCount = (int32_t)(dwSize - 4);
    return f;
}
int  FOG_GetRecordCountFromBinFile(D2BinFileStrc* f) { return f ? f->nRecordCount : 0; }
void FOG_FreeBinFile(D2BinFileStrc* f) { free(f); }
void FOG_10207(D2BinFileStrc*, D2BinFieldStrc*, void*, int, size_t) { FOG_DisplayHalt("the .txt compile path is not ported (tables load from .bin)", __FILE__, __LINE__); }
