// The host-seam table of the Storm SFile/SDraw entry points (storm_n42_sfile.cpp); a capture probe fills it to log what the retail hooks log.
#pragma once
#include <cstdint>
typedef uint32_t u32;
struct N42Host {
    void (*lock)(); void (*unlock)(); void (*setErr)(u32);
    void* (*alloc)(u32 n, const char* file, u32 line, u32 flags);
    void (*free_)(void* p, const char* file, u32 line);
    void (*strcpyN)(char* d, const char* s, u32 n); void (*strcatN)(char* d, const char* s, u32 n);
    u32 (*strlen_)(const char* s); const char* (*strstr_)(const char* s, const char* t); const char* (*strchr_)(const char* s, char c);
    u32 (*getAttr)(const char* p); void* (*createFile)(const char* p, u32 acc, u32 share, u32 disp, u32 flags);
    u32 (*setFilePtr)(void* h, int32_t off, u32 mode); u32 (*readFile)(void* h, void* buf, u32 n);
    void (*initArchives)(); void (*buildPath)(char* out, int mode); int (*findArchiveList)();
    void* (*newFile)(void* list, int a, int b); void (*delFile)(void* f); void (*destroyFile)(void* f);
    void (*hashInit)(void* tbl, void* mem, u32 n, void* name); u32 (*cryptSeed)(u32); void (*decrypt)(u32 key, void* d);
    void (*setCount)(u32); int (*findBlock)(void* arch, u32 hash); void (*postOpen)(void* f, void* tbl, int* cnt);
    void (*afterOpen)(); void (*regArchive)(); int (*fileOpenEx)(u32,u32*,u32,int*,int,int,int);
    const char* (*lastSep)(const char*, char);
    void (*drawLock)(); void (*palSnap)(); int (*lockSurf)(int,void*,int*,void**,int); void (*unlockSurf)(int,int);
    void (*blit)(void*,void*,int,u32,u32,u32,void*,int,int,u32); int (*savePcx)(const char*,void*,void*,u32,u32,u32);
    int (*saveDlg)(char* out260, void* hwnd); void (*surfSnap)();
};
