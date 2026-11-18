#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdint>

// D2Game's statically linked MSVC 2003 CRT: the rows with no the 1.10f source twin. Each callee that is not itself a
// game-tree function is a seam on the environment struct, in retail argument order.

extern "C" void d2game_unlock_3f76(int32_t lockNum);   // 0x3F76 __unlock

struct D2GameCrtCallocEnv
{
    void     (*lock)(int32_t locknum);                        // 0x40E4 __lock
    void     (*unlock)(int32_t locknum);                      // 0x411F __unlock
    int32_t* (*sbhAllocBlock)(uint32_t size);                 // 0x93FC ___sbh_alloc_block
    void     (*sub60DE)(void);                                // 0x60DE small-block heap bookkeeping
    void*    (*heapAlloc)(uint32_t bytes);                    // HeapAlloc(0x6FD3A20C, 8, bytes)
    int32_t  (*callnewh)(uint32_t size);                      // 0x9710 __callnewh
};

struct D2GameFreeflsEnv
{
    void     (*lock_)(int32_t);         // 0x402B __lock
    void     (*unlockD)(void);          // 0x3D36 __unlock(0xD)
    void     (*unlockC)(void);          // 0x3D42 __unlock(0xC)
    void     (*freetlocinfo)(uint32_t); // 0x398E ___freetlocinfo(locale)
    uint32_t default_mbc;               // 0x6FD39FD4 the CRT default mbc-info block
};

struct D2GameFreeptdEnv
{
    uint32_t tls_index;                                      // 0x6FD1F204, 0xFFFFFFFF = TLS_OUT_OF_INDEXES
    void*    (*tls_get_value)(uint32_t index);               // TlsGetValue
    int32_t  (*tls_set_value)(uint32_t index, void* value);  // TlsSetValue
    D2GameFreeflsEnv freefls;                                // 0x3C04's callees
    void (*freefls_fn)(void*);                               // replaces the 0x3C04 call when set (null in production)
};

struct D2GameGetStringTypeAEnv
{
    uint32_t default_locale;                       // DAT_6fd3003c
    uint32_t default_code_page;                    // DAT_6fd3004c
    uint32_t (*get_last_error)(void);              // GetLastError
    int32_t (*get_string_type_w)(uint32_t info_type, const uint16_t* src, int32_t cch, uint16_t* out);   // GetStringTypeW
    int32_t (*get_string_type_a)(uint32_t locale, uint32_t info_type, const char* src, int32_t cch, uint16_t* out); // GetStringTypeA
    uint32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src, int32_t cb, uint16_t* out, int32_t cch);
    int32_t (*locale_ansi_codepage)(uint32_t locale, const void* env); // D2Game+0x09824
    char* (*convert)(uint32_t from, uint32_t to, const char* src, uint32_t* pcb, char* out, int32_t outlen, const void* env); // D2Game+0x0986B
    void* (*malloc)(uint32_t bytes);
    void (*free)(void* p);
};

struct D2GameCrtWriteNolockEnv
{
    uint32_t* pioinfo;                                                                          // 0x6FD3A220 _pioinfo: block pointers (32-bit retail addresses)
    int32_t  (*lseeki64lk)(uint32_t fd, int32_t lo, int32_t hi, uint32_t origin);               // 0x871C
    int32_t  (*writeFile)(uint32_t handle, const void* buf, uint32_t n, uint32_t* written, uint32_t overlapped); // WriteFile
    uint32_t (*getLastError)(void);                                                             // GetLastError
    void     (*dosmaperr)(uint32_t error);                                                      // 0x4E77 __dosmaperr
    int32_t* (*errnoPtr)(void);                                                                 // 0x4E65
    uint32_t* (*doserrnoPtr)(void);                                                             // 0x4E6E
};

extern "C" void d2_fn_d2game_3c04(uint8_t* ptd, const D2GameFreeflsEnv* env);

//D2Game.0x56BD
int32_t D2GAME_CRT_WriteNolock_56BD(uint32_t fd, const char* buf, uint32_t count, const D2GameCrtWriteNolockEnv* env);

//D2Game.0x3B51
void D2GAME_CRT_Unlock12_3B51();
//D2Game.0x60DE
void D2GAME_CRT_Unlock4_60DE();
//D2Game.0x6018
void* D2GAME_CRT_Calloc_6018(uint32_t count, uint32_t size, const D2GameCrtCallocEnv* env);
//D2Game.0x3D4B
void D2GAME_CRT_Freeptd_3D4B(void* ptd, const D2GameFreeptdEnv* env);
//D2Game.0x75E8
int32_t D2GAME_CRT_GetStringTypeA_75E8(uint32_t info_type, const char* src, int32_t cch, uint16_t* out, uint32_t code_page, uint32_t locale, int32_t b_error, const D2GameGetStringTypeAEnv* env);

// MSVC 7.1 _iobuf as 32-bit retail words (flag +0x0C, file +0x10); host stdio owns the byte buffer.
struct D2GameCrtFile32
{
	uint32_t ptr;		//0x00
	int32_t cnt;		//0x04
	uint32_t base;		//0x08
	int32_t flag;		//0x0C
	int32_t file;		//0x10
	int32_t charbuf;	//0x14
	int32_t bufsiz;		//0x18
	uint32_t tmpfname;	//0x1C
};

struct D2GameCrtCommitSeams;

//D2Game.0x6FC25981
int32_t __cdecl D2GAME_CRT_FflushLk_6FC25981(D2GameCrtFile32* pRetailFile, FILE* pHostStream, const D2GameCrtCommitSeams* pCommitSeams);
//D2Game.0x6FC27460
size_t __cdecl D2GAME_CRT_Strlen_6FC27460(const char* szStr);
//D2Game.0x6FC282B0
uint64_t __stdcall D2GAME_CRT_Aulldvrm_6FC282B0(uint32_t nDividendLo, uint32_t nDividendHi, uint32_t nDivisorLo, uint32_t nDivisorHi);
