// Fog batch b55-fog-b. Win32 / sibling-function calls go through fog_bb_env seams.
// Structs carry only the fields each function touches (field names = retail offsets).
#include <stdint.h>
#include <string.h>

struct fog_bb_env {
    void (*cs_prep)(void* cs);          // Fog+0x11690 (lazy CS init)
    void (*cs_enter)(void* cs);
    void (*cs_leave)(void* cs);
    void (*cs_delete)(void* cs);
    void (*free_ptr)(void** p);         // Fog+0x1B0C0 (free *p)
    void (*reset_event)(void* h);
    void (*release253)(void* h);        // Fog ordinal 253 object release
    int  (*send6)(void* sock, void* a, void* b);   // Fog ordinal 6
    void (*sock_close707f0)(void* self, int idx);  // Fog+0x207F0
    void (*sha1_block)(uint32_t* state);           // Fog+0x1D2A0, block follows state
    void (*qpf)(uint64_t* out);
    uint32_t (*tick)(void);
    void (*err_dump5fc70)(int a);       // Fog+0x5FC70
    void (*log)(const char* fmt);       // Fog+0x190A0
    void (*q_kick202b0)(void* self);    // Fog+0x202B0
    void (*wait)(void* h, uint32_t ms);
};

struct fog_bb_node { uint32_t a, b, c; fog_bb_node* next; };

// 1.13c Fog+0x1D620
struct fog_bb_sha1ctx { uint32_t state[5]; uint32_t block[16]; };
void fog_bb_sha1_strided(const fog_bb_env* e, uint32_t* out, const uint8_t* in, uint32_t len, int stride)
{
    fog_bb_sha1ctx c;
    c.state[0] = 0x67452301; c.state[1] = 0xefcdab89; c.state[2] = 0x98badcfe;
    c.state[3] = 0x10325476; c.state[4] = 0xc3d2e1f0;
    uint32_t done = 0;
    if (len != 0) {
        do {
            uint32_t n = len - done; if (n > 0x40) n = 0x40;
            memcpy(c.block, in, n);
            in += stride;
            if (n < 0x40) memset((uint8_t*)c.block + n, 0, 0x40 - n);
            e->sha1_block(c.state);
            done += 0x40;
        } while (done < len);
    }
    memcpy(out, c.state, 20);
}

// 1.13c Fog+0xFCF0
struct fog_bb_errstate { int in_error; };
int fog_bb_fcf0(const fog_bb_env* e, const fog_bb_errstate* s, int p1)
{
    if (s->in_error != 0) {
        e->err_dump5fc70(p1);
        e->log("\n***** ENCOUNTERED ERROR DURING ERROR HANDLING!!!\n%s,%d\n%s\n*****");
        return 1;
    }
    return 0;
}

// 1.13c Fog+0xFBC0
struct fog_bb_perf { uint32_t ticks_per_ms, ticks_per_us; uint32_t acc[0x6000]; uint32_t start_tick; };
void fog_bb_perf_init(const fog_bb_env* e, fog_bb_perf* p)
{
    uint64_t f; e->qpf(&f);
    p->ticks_per_ms = (uint32_t)(f / 1000);
    p->ticks_per_us = (uint32_t)(f / 1000000);
    memset(p->acc, 0, sizeof p->acc);
    p->start_tick = e->tick();
}

// 1.13c Fog+0x1E5C0
struct fog_bb_sub { uint8_t pad0[0xc]; uint8_t cs[24]; int f24; int f2c; };
struct fog_bb_obj { fog_bb_sub* sub; int f124; };
void fog_bb_1e5c0(const fog_bb_env* e, fog_bb_obj* o)
{
    if (o && o->sub && o->sub->f24 != 0) {
        o->f124 = 1;
        e->cs_prep(o->sub->cs); e->cs_enter(o->sub->cs);
        o->sub->f2c = 1;
        e->cs_leave(o->sub->cs);
    }
}

// 1.13c Fog+0x16DF0
struct fog_bb_sock { void* sock; uint8_t cs[24]; };
bool fog_bb_16df0(const fog_bb_env* e, fog_bb_sock* s, void* a, void* b)
{
    e->cs_prep(s->cs); e->cs_enter(s->cs);
    int r = e->send6(s->sock, a, b);
    e->cs_leave(s->cs);
    return r != -1;
}

// 1.13c Fog+0xD5E0
struct fog_bb_qs {
    fog_bb_node* head84c; uint8_t cs854[24];
    int count890; uint8_t cs894[24];
    fog_bb_node** buckets8ac; uint32_t nbuckets8b0;
};
void fog_bb_d5e0(const fog_bb_env* e, fog_bb_qs* q)
{
    e->cs_prep(q->cs854); e->cs_enter(q->cs854);
    fog_bb_node* n = q->head84c;
    while (n) { fog_bb_node* nx = n->next; void* t = n; e->free_ptr(&t); n = nx; }
    q->head84c = 0;
    e->cs_leave(q->cs854);
}

// 1.13c Fog+0xD710
void fog_bb_d710(const fog_bb_env* e, fog_bb_qs* q)
{
    e->cs_prep(q->cs894); e->cs_enter(q->cs894);
    for (uint32_t i = 0; i < q->nbuckets8b0; i++) {
        fog_bb_node* n = q->buckets8ac[i];
        while (n) { fog_bb_node* nx = n->next; void* t = n; e->free_ptr(&t); n = nx; }
        q->buckets8ac[i] = 0;
    }
    q->count890 = 0;
    e->cs_leave(q->cs894);
}

// 1.13c Fog+0x179B0 / 0x17B60 : packet queue
struct fog_bb_pkt { uint16_t len; uint16_t pad; fog_bb_pkt* next; };  // payload follows
struct fog_bb_pq { int overflow; uint8_t cs[24]; fog_bb_pkt* head; fog_bb_pkt* tail; };
uint16_t fog_bb_pq_pop(const fog_bb_env* e, fog_bb_pq* q, void* dst, uint32_t cap)
{
    e->cs_prep(q->cs); e->cs_enter(q->cs);
    fog_bb_pkt* p = q->head;
    uint16_t len = 0;
    if (p) {
        len = p->len;
        if ((uint16_t)cap < len) { q->overflow = 1; len = 0; }
        else memcpy(dst, p + 1, len);
        fog_bb_pkt* nx = p->next;
        void* t = p; e->free_ptr(&t);
        q->head = nx;
        if (!nx) q->tail = 0;
    }
    e->cs_leave(q->cs);
    return len;
}

void fog_bb_pq_destroy(const fog_bb_env* e, fog_bb_pq* q)
{
    e->cs_prep(q->cs); e->cs_enter(q->cs);
    while (q->head) { fog_bb_pkt* nx = q->head->next; void* t = q->head; e->free_ptr(&t); q->head = nx; }
    q->tail = 0; q->head = 0;
    e->cs_delete(q->cs);
}

// 1.13c Fog+0x20AF0
struct fog_bb_net { uint8_t pad[4]; int socks[4]; int f_c18; };
int fog_bb_20af0(const fog_bb_env* e, fog_bb_net* n)
{
    if (n->f_c18 == 0)
        for (int i = 0; i < 4; i++)
            if (n->socks[i] != -1) e->sock_close707f0(n, i);
    return 1;
}

// 1.13c Fog+0x1D740
struct fog_bb_ev { void* h; int a; int b; };
void fog_bb_1d740(const fog_bb_env* e, uintptr_t* o)
{
    if (((uint8_t)o[0x12] & 1) == 0) {
        fog_bb_ev* ev = (fog_bb_ev*)o[0xe];
        if (ev) { e->reset_event(ev->h); ev->a = 1; ev->b = 0; o[0xe] = 0; }
        if (o[0]) { e->release253((void*)o[0]); o[0] = 0; }
        o[0x12] |= 1;
        if (o[0xc]) {
            void* t = (void*)o[0xf]; e->free_ptr(&t);
            o[0xf] = o[0xc]; o[0x10] = o[0xd];
            o[7] = o[8] = o[9] = o[10] = o[0xb] = o[0xc] = o[0xd] = 0;
        }
    }
}

// 1.13c Fog+0xE9A0
struct fog_bb_srv { int mode; uint8_t cs[24]; int stop_flag; void* event; };
void fog_bb_e9a0(const fog_bb_env* e, fog_bb_srv* s)
{
    if (s->mode != 0) { e->q_kick202b0(s); return; }
    e->cs_prep(s->cs); e->cs_enter(s->cs);
    s->stop_flag = 1;
    e->cs_leave(s->cs);
    e->wait(s->event, 2000);
}
