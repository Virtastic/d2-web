// Compat: D2MOO Archive.h -> our MPQ VFS. DrlgPreset.cpp reads DS1 files through this.
#pragma once
#include <Windows.h>
#include <D2BasicTypes.h>
#include <StormHandles.h>
struct D2ArchiveHandleStrc;
typedef D2ArchiveHandleStrc* HD2ARCHIVE;
// Native streams share the boot VFS owner with Fog file services.
BOOL ARCHIVE_OpenFile(HD2ARCHIVE hArchive, const char* szFilePath, HSFILE* phFile, BOOL bFileNotFoundLogSkipped);
// Native file error from the preceding operation.
uint32_t ARCHIVE_GetLastError();
void ARCHIVE_CloseFile(HD2ARCHIVE, HSFILE);
void* ARCHIVE_AllocateBufferAndReadFile(HD2ARCHIVE hArchive, const char* szFilePath, size_t* pdwBytesWritten, const char* szSrcPath, int nLine);
#define ARCHIVE_ALLOC_BUFFER_AND_READ_FILE_TO_IT(hArchive, szFilePath, pBytesWritten) ARCHIVE_AllocateBufferAndReadFile(hArchive, szFilePath, pBytesWritten, __FILE__, __LINE__)
