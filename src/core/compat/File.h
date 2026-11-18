#pragma once
// Compat: Fog File.h. Only DATATBLS_CompileTxt's "-txt" branch touches files; it is never enabled.
#include <stdio.h>
inline int fopen_s(FILE** f, const char*, const char*) { *f = nullptr; return 1; }
inline size_t FileLockAndWrite(const void*, size_t, size_t, FILE*) { return 0; }
