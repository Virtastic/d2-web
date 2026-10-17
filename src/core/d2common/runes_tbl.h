// Raw 32-bit-image rune-table record writer (1.13c D2Common+0x70FF0). The record array, its count and the
// allocation are supplied by the caller, so the shallow FN probe can pin them to its own blobs.
#pragma once
#include <cstdint>

struct RunesTblEnv {
    uint32_t* pCount;                                             // nRunesTxtRecordCount (in/out)
    uint8_t** pArray;                                             // pRunesTxt records pointer (in/out)
    uint32_t mode;                                                // DATATBLS_LoadFromBin != 0: a 4-byte count prefix precedes the records
    uint8_t* (*realloc)(uint8_t* old, uint32_t size, void* ctx);  // Fog 10047 (realloc-pool) stand-in
    void* ctx;
};

void D2Common_70FF0_AddOrChangeRunesTxtRecord(const RunesTblEnv& e, int32_t nRecordId, const uint8_t* rec);
