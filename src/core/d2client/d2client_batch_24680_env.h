// The injected-callee table of the D2Client 0x24680 object-case family (shared with the FN probes).
#pragma once
#include <cstdint>

struct D2ClientBatch24680Env {
    void* (*resolve)(uint32_t address);
    const uint8_t* (*object_record)(uint32_t class_id); // Common #10688
    const uint8_t* (*object_data)(void* unit);         // Common #10092
    uint32_t (*is_object)(void* unit);                // Common #10934
    uint32_t (*refresh)(void* unit);                  // Common #10595
    uint32_t (*advance)(void* unit);                  // Client +237E0
    uint32_t (*set_mode)(void* unit, uint32_t mode);   // Common #11090
    uint32_t (*animate)(void* unit, uint32_t arg);     // Client +6A220
    uint32_t (*random_range)(void* seed, uint32_t limit, uint32_t base); // +2DC0 ECX/EAX/EDI
    uint32_t (*tick)();
    void (*detach)(void* unit);                       // +69700 EAX
    void (*remove10408)(void* unit);
    void (*remove11016)(void* unit);
    void (*destroy)(void* allocation);                // +A8630 ESI
    void (*free10043)(void* allocation, uint32_t file_rva, uint32_t line, uint32_t zero);
    uint8_t (*get_flags)(void* unit);                 // Common #10258
    void (*set_flags)(void* unit, uint8_t flags);     // Common #10111
    void (*sound)(uint32_t id, void* unit, uint32_t, uint32_t, uint32_t); // +88A70 EBX/stack
    void (*assert10265_10024)(uint32_t line);          // retail localized diagnostic and fatal exit
    int32_t object_type_count;                        // +F54E0
    bool remove_enabled;                             // +11C3B8
    void* const* object_buckets;                      // +10AA08, 128 native heads
    void* const* player_buckets;                      // +10A608, 128 native heads
    struct Dispatch {
        void (*initialize)(void* source, const uint8_t* data); // +D2F60, ECX/EDX
        void (*apply)(void* source, void* target, const uint8_t* data); // +D2F64
        uint32_t unused[2];
        uint32_t sound_id;                            // +D2F70
    } const* object_dispatch;
    // Jump-table targets belonging to other function. A required native callee,
    // not an optional fallback: dispatch must never silently discard its effects.
    uint8_t (*case_target[19])(void* unit);
};
