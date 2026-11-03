// Fog's client-memory allocation on retail's register convention (ordinal 10042): ECX = size, EDX = the source file,
// stack: line, one unused word (ret 8). The pool allocator takes EAX = size, ECX = 0 (no pool) and the file and line;
// a null answer reports error 2 (file, line, the out-of-memory text) through the fatal-error path. Answers the block.
#include <cstdint>

struct Fog1CD10Env {
    uint32_t (*poolAlloc)(uint32_t size, uint32_t pool, uint32_t file, uint32_t line);
    void     (*error)(uint32_t code, uint32_t file, uint32_t line, uint32_t text);
    uint32_t oomText;   // the module string the error is reported with
};

uint32_t Fog_AllocClientMemory32(uint32_t size, uint32_t file, uint32_t line, const Fog1CD10Env& env)
{
    const uint32_t p = env.poolAlloc(size, 0, file, line);
    if (!p) env.error(2, file, line, env.oomText);
    return p;
}
