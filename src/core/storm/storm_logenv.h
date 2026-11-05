#pragma once
// Storm log module host seam (retail words, not native pointers). Nodes are retail-layout dword arrays:
// {id, next, file, used, position, +14, +18, data[]}; a link or head is the node's retail 32-bit address,
// resolved by node(). Locks are the retail RVAs of the bucket CRITICAL_SECTIONs (0x52F60 + bucket*0x18).
#include <cstdint>
struct StormLogEnv {
    uint32_t* heads;      // 0x52FC0, four bucket heads
    uint32_t* nextId;     // 0x52F5C, last issued log id
    uint32_t* lastError;  // 0x55388, Storm's last-error word
    uint32_t* (*node)(uint32_t addr);
    void (*enter)(uint32_t lockRva);
    void (*leave)(uint32_t lockRva);
    uint32_t (*write)(uint32_t file, uint32_t buf, uint32_t len);   // WriteFile(file, buf, len, &local, 0)
    void (*flush)(uint32_t file);                                    // FlushFileBuffers
    void (*close)(uint32_t file);                                    // CloseHandle
    uint32_t (*create)(const char* name);                            // CreateFileA(name, GENERIC_WRITE, 3, 0, CREATE_ALWAYS, NORMAL, 0)
    void (*setLastError)(uint32_t code);                             // SetLastError
    uint32_t (*find)(uint32_t id, uint32_t* bucket, uint32_t create);   // 0x347B0 (EDI = id): node address, lock held on success
    void (*release)(uint32_t bucket, uint32_t nodeAddr);             // 0x346E0 (ESI = bucket, EDX = node), drops the bucket lock
    void (*purge)(uint32_t tagRva, uint32_t arg);                    // 0x1FB30("HSLOG" at module RVA tagRva, 0)
    void (*deleteLock)(uint32_t lockRva);                            // DeleteCriticalSection
    void (*flushAll)();                                              // 0x349E0 (ECX = scratch)
    void (*closeLog)(uint32_t id);                                   // 0x34BA0
};
static inline uint32_t stormLogLock(uint32_t bucket) { return 0x52F60u + bucket * 0x18u; }
