// Layered MPQ lookup, the way retail resolves a path: first archive in priority order that has
// the file wins. Retail 1.13c order (D2Launch/Fog): patch_d2, d2exp, d2xmusic, d2xtalk, d2xvideo,
// d2data, d2char, d2music, d2sfx, d2speech, d2video. Plain StormLib handles per archive -- no
// SFileOpenPatchArchive, whose patch semantics are WoW-era and not what D2 does.
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_set>

namespace d2 {

class MpqVfs {
public:
    MpqVfs() = default;
    ~MpqVfs();
    MpqVfs(const MpqVfs&) = delete;
    MpqVfs& operator=(const MpqVfs&) = delete;
    // Native-width StormLib handles, owned until closeFile or VFS destruction.
    bool openFile(const char* name, void** out);
    bool closeFile(void* file);
    uint32_t fileSize(void* file, uint32_t* high = nullptr) const;
    bool readFile(void* file, void* out, uint32_t size, uint32_t* read = nullptr);
    // Opens `path` (an emscripten FS path; typically a streamfs mount). False if StormLib refuses.
    // Order of mount() calls IS the priority order.
    bool mount(const char* path);
    bool has(const char* name) const;
    // Whole-file read. Empty vector + false on miss. `name` uses backslashes like retail.
    bool read(const char* name, std::vector<uint8_t>& out) const;
    bool fileName(void* file, char* out, uint32_t capacity) const;
    size_t mounted() const { return archives_.size(); }
private:
    std::unordered_set<void*> files_;
    std::vector<void*> archives_;   // HANDLEs, in priority order
};

} // namespace d2
