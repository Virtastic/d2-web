// 1.13c Storm+0x34460: register handler h for SEVT command cmd under key.
#include <cstdint>

struct Storm34460Env {
    uint32_t* (*f_15e0)(void* list, int32_t extra, uint32_t flags);  // 0x15E0 AUWNDREC ctor
    uint32_t (*f_36290)(uint32_t cmd, uint32_t key, uint32_t param_2, uint32_t param_4, uint32_t h);
    void (*set_last_error)(uint32_t e);
};

// 0x6FC458E8 list head (record: +4 next, +8 key); 0x6FC458E0 the list; 0x6FC45388 last error.
extern "C"
uint32_t Storm_Sub34460(uint32_t cmd, uint32_t param_2, uint32_t key, uint32_t h, uint32_t** head, void* list,
                        int32_t* lastError, const Storm34460Env* env) {
    if (h == 0) {
        *lastError = 0x57;
        env->set_last_error(0x57);
        return 0;
    }
    uint32_t* rec = *head;
    for (; rec != nullptr; rec = (uint32_t*)(uintptr_t)rec[1]) {
        if (rec[2] == key) return env->f_36290(cmd, key, param_2, 0, h);
    }
    env->f_15e0(list, 0, 0)[2] = key;
    return env->f_36290(cmd, key, param_2, 0, h);
}
