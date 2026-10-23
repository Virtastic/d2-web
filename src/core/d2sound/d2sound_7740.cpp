#include <cstdint>
#include <cstring>
extern "C" { void* D2sound_alloc(int size, int line); void D2sound_free(void* p, int line); }
extern int32_t DAT_6f9c6584, DAT_6f9c6590; extern void* DAT_6f9c65cc; // host seams
int D2sound_7090(void* rec, uint32_t flags);
void D2sound_lock(); void D2sound_unlock(); // DAT_6f9c6530 critical section
struct SLink { void* rec; void* next; };
// 1.13c D2sound+0x7740 allocate a 0x60-byte sound record, init via 0x7090, link at the head of the global list.
void* D2sound_7740(uint32_t flags, int32_t arg) {
    DAT_6f9c6584 = arg;
    if (((flags & 2) && !DAT_6f9c6590) || ((flags & 2) && (flags & 1))) return nullptr;
    void* rec = D2sound_alloc(0x1d9, 0);
    std::memset(rec, 0, 0x60);
    if (!D2sound_7090(rec, flags)) { D2sound_free(rec, 0x1e0); return nullptr; }
    D2sound_lock();
    SLink* l = (SLink*)D2sound_alloc(0xd4, 0);
    l->rec = rec; l->next = DAT_6f9c65cc; DAT_6f9c65cc = l;
    D2sound_unlock();
    return rec;
}
