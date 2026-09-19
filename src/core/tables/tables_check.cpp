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

} // extern "C"
