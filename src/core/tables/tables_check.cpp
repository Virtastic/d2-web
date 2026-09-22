// Retail-gated table checks (s02-tables). Mounts the retail MPQs in priority order, reads
// .txt tables through the VFS (the mod path), and hands the shipped .bin tables to the test runner
// for byte comparison against the retail captures -- the in-memory tables captured from the
// running 1.13c D2Common. Retail loads the .bin (u32 count + packed records) and then fixes
// some fields up in place; every byte that differs is either such a fixup (to be documented
// and masked) or a bug. Nothing is masked silently.
#include <emscripten.h>
#include <string>
#include <vector>
#include "txt.h"
#include "fixups.h"
#include "../lang/strtable.h"
#include "../../assets/mpqvfs.h"

static d2::MpqVfs g_vfs;
static d2::LangTables g_lang;
static bool g_langLoaded = false;

// string.tbl < expansionstring.tbl < patchstring.tbl from the VFS, once. Returns how many loaded.
static int loadLang() {
    if (g_langLoaded) return 3;
    int n = 0; std::vector<uint8_t> buf;
    const char* files[3] = { "data\\local\\lng\\ENG\\patchstring.tbl", "data\\local\\lng\\ENG\\expansionstring.tbl", "data\\local\\lng\\ENG\\string.tbl" };
    d2::StrTable* tabs[3] = { &g_lang.patch, &g_lang.expansion, &g_lang.classic };
    for (int i = 0; i < 3; i++) if (g_vfs.read(files[i], buf) && tabs[i]->load(buf.data(), buf.size())) n++;
    g_langLoaded = n == 3;
    return n;
}
static std::vector<uint8_t> g_bin;   // last .bin payload handed to JS (records only, count stripped)

// The mounted VFS and string tables, for the other retail-gated checks (drlg_check.cpp).
namespace d2 {
MpqVfs& sharedVfs() { return g_vfs; }
const LangTables& sharedLang() { loadLang(); return g_lang; }
}

extern "C" {

EMSCRIPTEN_KEEPALIVE int d2_vfs_mount(const char* path) { return g_vfs.mount(path) ? 1 : 0; }

// "<rows>:<cols>:<cell(0,col 0)>:<cell(last,col 0)>" or "ERR:<why>"
EMSCRIPTEN_KEEPALIVE const char* d2_txt_probe(const char* name) {
    static std::string out;
    std::vector<uint8_t> buf;
    if (!g_vfs.read(name, buf)) { out = "ERR:missing"; return out.c_str(); }
    d2::TxtTable t = d2::parseTxt(buf.data(), buf.size());
    if (t.rows.empty()) { out = "ERR:norows"; return out.c_str(); }
    out = std::to_string(t.rows.size()) + ":" + std::to_string(t.columns.size()) + ":" + t.cell(0, 0) + ":" + t.cell(t.rows.size() - 1, 0);
    return out.c_str();
}

// Loads data\global\excel\<name>.bin from the VFS. Returns the record count (0 on miss) and
// leaves the records in a buffer JS can read with d2_table_bin_ptr/len.
EMSCRIPTEN_KEEPALIVE int d2_table_bin_load(const char* name) {
    std::vector<uint8_t> buf;
    std::string path = std::string("data\\global\\excel\\") + name + ".bin";
    if (!g_vfs.read(path.c_str(), buf) || buf.size() < 4) { g_bin.clear(); return 0; }
    uint32_t count; memcpy(&count, buf.data(), 4);
    g_bin.assign(buf.begin() + 4, buf.end());
    if (count && g_bin.size() % count == 0) {
        loadLang();
        d2::applyTableFixups(name, g_bin.data(), count, g_bin.size() / count, g_lang, g_vfs);
    }
    return (int)count;
}
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_table_bin_ptr() { return g_bin.data(); }
// "<count loaded>:<value for key>" -- lets the test runner probe the string tables directly.
EMSCRIPTEN_KEEPALIVE const char* d2_lang_probe(const char* key) {
    static std::string out; int n = loadLang();
    const std::u16string* v = g_lang.find(key);
    out = std::to_string(n) + ":";
    if (v) for (char16_t c : *v) out.push_back(c < 0x80 ? (char)c : '?');
    else out += "<missing>";
    return out.c_str();
}
EMSCRIPTEN_KEEPALIVE int d2_table_bin_len() { return (int)g_bin.size(); }

// Retail UI text for the client. d2_lang_probe above is shaped for the test runner -- it prefixes the
// table count and flattens anything non-ASCII to '?' -- which is fine for a diff and useless for
// drawing. These return real UTF-8, so a name with an accent survives the trip.
static std::string u16ToUtf8(const std::u16string& in) {
    std::string out;
    for (size_t i = 0; i < in.size(); ++i) {
        char32_t c = in[i];
        // Surrogate pair, so characters outside the BMP are not silently cut in half.
        if (c >= 0xD800 && c <= 0xDBFF && i + 1 < in.size() && in[i + 1] >= 0xDC00 && in[i + 1] <= 0xDFFF) {
            c = 0x10000 + ((c - 0xD800) << 10) + (in[++i] - 0xDC00);
        }
        if (c < 0x80) { out.push_back((char)c); }
        else if (c < 0x800) {
            out.push_back((char)(0xC0 | (c >> 6)));
            out.push_back((char)(0x80 | (c & 0x3F)));
        } else if (c < 0x10000) {
            out.push_back((char)(0xE0 | (c >> 12)));
            out.push_back((char)(0x80 | ((c >> 6) & 0x3F)));
            out.push_back((char)(0x80 | (c & 0x3F)));
        } else {
            out.push_back((char)(0xF0 | (c >> 18)));
            out.push_back((char)(0x80 | ((c >> 12) & 0x3F)));
            out.push_back((char)(0x80 | ((c >> 6) & 0x3F)));
            out.push_back((char)(0x80 | (c & 0x3F)));
        }
    }
    return out;
}

// By reference key, the way D2LANG_GetStringByReferenceString resolves it (patch, then expansion,
// then classic). Empty string when the key is absent -- retail would render the key in angle
// brackets, which is a presentation choice the caller can make.
EMSCRIPTEN_KEEPALIVE const char* d2_lang_string(const char* key) {
    static std::string out; out.clear();
    if (!key) return out.c_str();
    loadLang();
    if (const std::u16string* v = g_lang.find(key)) out = u16ToUtf8(*v);
    return out.c_str();
}

// By .tbl index, which is what carries dialogue and most UI text in packets and table records
// (D2LANG_GetStringFromTblIndex: 10000+ patch, 20000+ expansion, otherwise classic).
EMSCRIPTEN_KEEPALIVE const char* d2_lang_string_by_index(int index) {
    static std::string out; out.clear();
    if (index < 0 || index > 0xFFFF) return out.c_str();
    loadLang();
    if (const std::u16string* v = g_lang.byTblIndex((uint16_t)index)) out = u16ToUtf8(*v);
    return out.c_str();
}

// The .tbl index for a key, so a caller can go key -> index -> string the way retail's tables do.
EMSCRIPTEN_KEEPALIVE int d2_lang_index(const char* key) {
    if (!key) return 0;
    loadLang();
    return (int)g_lang.tblIndex(key);
}

} // extern "C"
