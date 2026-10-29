// Browser-facing whole-file reads from the layered retail MPQ VFS. The returned pointer is
// owned here and remains valid until the next read; the browser helper copies it immediately.
#include <emscripten.h>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>
#include "../core/tables/shared.h"

namespace {
std::vector<uint8_t> g_asset; std::string g_assetError;
std::vector<void*> g_parts;   // files open for a read in parts; a handle is its index + 1
void* partFile(int h) { return h > 0 && static_cast<size_t>(h) <= g_parts.size() ? g_parts[h - 1] : nullptr; }
}

extern "C" {
EMSCRIPTEN_KEEPALIVE int d2_asset_read(const char* name) {
    g_asset.clear(); g_assetError.clear();
    if (!name || !*name) { g_assetError = "invalid asset path"; return 0; }
    try {
        if (!d2::sharedVfs().read(name, g_asset)) {
            g_assetError = std::string("asset not found or unreadable: ") + name; return 0;
        }
    } catch (const std::exception& e) {
        g_asset.clear(); g_assetError = std::string("asset read failed: ") + e.what(); return 0;
    } catch (...) { g_asset.clear(); g_assetError = "asset read failed"; return 0; }
    return 1;
}
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_asset_ptr() { return g_asset.empty() ? nullptr : g_asset.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_asset_len() { return static_cast<uint32_t>(g_asset.size()); }
EMSCRIPTEN_KEEPALIVE const char* d2_asset_error() { return g_assetError.c_str(); }

// The same read in parts (open, size, part after part into d2_asset_ptr, close) so the browser can yield
// between them: StormLib decompresses a music track's sectors on the main thread, a 1.1 s freeze read whole.
// A handle is a plain number, so the browser never passes a wasm64 pointer.
EMSCRIPTEN_KEEPALIVE int d2_asset_open(const char* name) {
    g_assetError.clear(); void* file = nullptr;
    if (!name || !*name || !d2::sharedVfs().openFile(name, &file)) {
        g_assetError = std::string("asset not found or unreadable: ") + (name ? name : ""); return 0;
    }
    for (size_t i = 0; i < g_parts.size(); ++i) if (!g_parts[i]) { g_parts[i] = file; return static_cast<int>(i + 1); }
    g_parts.push_back(file); return static_cast<int>(g_parts.size());
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_asset_size(int h) { return d2::sharedVfs().fileSize(partFile(h)); }
// Reads exactly n bytes: StormLib reports a short read as a failure (ERROR_HANDLE_EOF).
EMSCRIPTEN_KEEPALIVE int d2_asset_part(int h, uint32_t n) {
    g_asset.resize(n); uint32_t got = 0;
    const bool ok = d2::sharedVfs().readFile(partFile(h), g_asset.data(), n, &got) && got == n;
    if (!ok) { g_asset.clear(); g_assetError = "asset part read failed"; }
    return ok;
}
EMSCRIPTEN_KEEPALIVE void d2_asset_close(int h) {
    if (void* file = partFile(h)) { d2::sharedVfs().closeFile(file); g_parts[h - 1] = nullptr; }
}
}
