// 1.13c Fog+0x15c00: trigger the imagehlp stack walk for a thread. Retail reads the registry flag NoStackDump, then refuses (one log line each)
// when imagehlp is not loaded, when there is no trigger event, and logs a warning when there is no walk thread; otherwise it duplicates the
// thread handle, signals the walk thread, lifts the handle's priority to the maximum while it waits for the done event, then restores it.
// The host side (registry, log, kernel32 thread calls) comes in through FogStackEnv.
#include <cstdint>
struct FogStackGlobals { uint32_t imagehlp, thread, trigger, done, dup; };   // Fog+0x56E54 .. +0x56E64
struct FogStackEnv {
    uint32_t (*noStackDump)();                 // Ordinal 423 ("Diablo II", "NoStackDump", 0)
    void (*log)(const char* msg);              // FUN_190A0(&log, msg)
    uint32_t (*currentThread)();
    void (*dupHandle)(uint32_t thread, uint32_t* out);   // DuplicateHandle(self, thread, self, out, 0, 0, DUPLICATE_SAME_ACCESS)
    void (*setEvent)(uint32_t ev);
    int32_t (*getPriority)(uint32_t h);
    void (*setPriority)(uint32_t h, int32_t p);
    void (*wait)(uint32_t ev);
    void (*closeHandle)(uint32_t h);
};
void fog_stack_walk_15C00(uint32_t thread, FogStackGlobals* g, const FogStackEnv* e) {
    if (e->noStackDump()) { e->log("sDumpStack: Stack dump turned off in registry"); return; }
    if (!g->imagehlp) { e->log("sDumpStack: imagehlp.dll not loaded"); return; }
    if (!g->trigger) { e->log("No event to trigger stack walk thread"); return; }
    if (!g->thread) e->log("No stack walk thread");
    e->log("");
    e->log("ImageHelp Stack Crawl:");
    if (!thread) thread = e->currentThread();
    e->dupHandle(thread, &g->dup);
    e->setEvent(g->trigger);
    int32_t prio = e->getPriority(g->dup);
    if (prio == 0x7fffffff) prio = 0;
    e->setPriority(g->dup, -0xf);
    e->wait(g->done);
    e->setPriority(g->dup, prio);
    e->closeHandle(g->dup);
}
