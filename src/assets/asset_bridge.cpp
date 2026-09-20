// Browser-facing whole-file reads from the layered retail MPQ VFS. The returned pointer is
// owned here and remains valid until the next read; the browser helper copies it immediately.
#include <emscripten.h>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>
#include "../core/tables/shared.h"

namespace { std::vector<uint8_t> g_asset; std::string g_assetError; }

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
}
