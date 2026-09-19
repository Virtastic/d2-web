#include "mpqvfs.h"
#include <StormLib.h>

namespace d2 {

MpqVfs::~MpqVfs() { for (void* h : archives_) SFileCloseArchive(h); }

bool MpqVfs::mount(const char* path) {
    HANDLE h = nullptr;
    if (!SFileOpenArchive(path, 0, MPQ_OPEN_READ_ONLY, &h)) return false;
    archives_.push_back(h);
    return true;
}

bool MpqVfs::has(const char* name) const {
    for (void* h : archives_) if (SFileHasFile(h, name)) return true;
    return false;
}

bool MpqVfs::read(const char* name, std::vector<uint8_t>& out) const {
    for (void* h : archives_) {
        HANDLE f;
        if (!SFileOpenFileEx(h, name, 0, &f)) continue;
        DWORD size = SFileGetFileSize(f, nullptr), got = 0;
        out.resize(size);
        bool ok = size == 0 || (SFileReadFile(f, out.data(), size, &got, nullptr) && got == size);
        SFileCloseFile(f);
        if (ok) return true;
        out.clear();
        return false;   // found but unreadable: do not fall through to an older layer's copy
    }
    return false;
}

} // namespace d2
