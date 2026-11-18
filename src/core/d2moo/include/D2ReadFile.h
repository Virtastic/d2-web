// The callees of D2Common's whole-file read (D2Common+0x9900), in retail's order. The play path binds them to the
// MPQ VFS bridge (src/core/d2moo/src/Crt/ReadFile.cpp); an FN probe binds them to recorders.
#pragma once
#include <cstdint>

struct D2ReadFileEnv {
    int       (*open)(const char* name, uintptr_t* handleCell);     // ECX = name, EDX = the handle cell; 0 = not found
    void      (*openFailed)(const char* name);                      // the last-error read and the log line (format, name)
    uint32_t  (*size)(uintptr_t handle, uint32_t* scratch);         // ESI = handle, one stack word (a caller local)
    void*     (*alloc)(uint32_t bytes, int line);                   // ECX = bytes, EDX = the source file string, stack (line, 0)
    void      (*read)(uintptr_t handle, uint32_t size, void* buf);  // ESI = handle, EDI = size, one stack word (the buffer)
    void      (*fatal)(int line);                                   // does not return in retail
    void      (*close)(uintptr_t handle);                           // ECX = handle
};

// 1.13c D2Common+0x9900
void* d2common_read_file(const char* name, uint32_t* sizeOut, const D2ReadFileEnv* e);
