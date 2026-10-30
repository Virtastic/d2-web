#include "mpqvfs.h"
#include <StormLib.h>
#include <cstring>

namespace d2 {

MpqVfs::~MpqVfs() {
    for (void* h : files_) SFileCloseFile(h);
    for (void* h : archives_) SFileCloseArchive(h);
}

bool MpqVfs::openFile(const char* name, void** out) {
    if (out) *out = nullptr;
    if (!out || !name || !*name) { SErrSetLastError(ERROR_INVALID_PARAMETER); return false; }
    // Storm #267 enables local-file lookup when its archive list is empty.
    if (archives_.empty()) {
        HANDLE file = nullptr;
        if (!SFileOpenFileEx(nullptr, name, SFILE_OPEN_LOCAL_FILE, &file)) return false;
        files_.insert(file);
        *out = file;
        return true;
    }
    uint32_t openError = ERROR_FILE_NOT_FOUND;
    for (void* archive : archives_) {
        HANDLE file = nullptr;
        if (!SFileOpenFileEx(archive, name, 0, &file)) {
            // Keep searching lower layers, but do not hide a corrupt/unsupported
            // entry behind file-not-found when no layer can open the stream.
            const auto error = SErrGetLastError();
            if (openError == ERROR_FILE_NOT_FOUND && error != ERROR_FILE_NOT_FOUND)
                openError = error;
            continue;
        }
        files_.insert(file);
        *out = file;
        return true;
    }
    SErrSetLastError(openError);
    return false;
}

bool MpqVfs::closeFile(void* file) {
    auto it = files_.find(file);
    if (it == files_.end()) { SErrSetLastError(ERROR_INVALID_HANDLE); return false; }
    bool ok = SFileCloseFile(file);
    if (ok) files_.erase(it);
    return ok;
}

uint32_t MpqVfs::fileSize(void* file, uint32_t* high) const {
    if (high) *high = 0;
    if (!files_.contains(file)) { SErrSetLastError(ERROR_INVALID_HANDLE); return 0xFFFFFFFF; }
    return SFileGetFileSize(file, high);
}

bool MpqVfs::readFile(void* file, void* out, uint32_t size, uint32_t* read) {
    if (read) *read = 0;
    if (!out) { SErrSetLastError(ERROR_INVALID_PARAMETER); return false; }
    if (!files_.contains(file)) { SErrSetLastError(ERROR_INVALID_HANDLE); return false; }
    return SFileReadFile(file, out, size, read, nullptr);
}

bool MpqVfs::fileName(void* file, char* out, uint32_t capacity) const {
    if (!out || !capacity) { SErrSetLastError(ERROR_INVALID_PARAMETER); return false; }
    out[0] = 0;
    if (!files_.contains(file)) { SErrSetLastError(ERROR_INVALID_HANDLE); return false; }
    // StormLib's native MAX_PATH is 1024 on Linux; retail callers own 260 bytes.
    char name[MAX_PATH]{};
    if (!SFileGetFileName(file, name)) return false;
    const size_t length = std::strlen(name);
    if (length >= capacity) { SErrSetLastError(ERROR_INSUFFICIENT_BUFFER); return false; }
    std::memcpy(out, name, length + 1);
    return true;
}

bool MpqVfs::mount(const char* path) {
    HANDLE h = nullptr;
    // No (listfile): every read here is by name, which is a hash lookup, and an opened file keeps the
    // name it was opened with (what retail Storm's SFileGetFileName answers). Parsing the eleven
    // listfiles cost ~1 s native, ~3 s in the browser, in the boot task that mounts the archives.
    if (!SFileOpenArchive(path, 0, MPQ_OPEN_READ_ONLY | MPQ_OPEN_NO_LISTFILE, &h)) return false;
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
