// BNClient batch b67: IDEA block/key helpers, download recv loop, news table parse, EH try-range.
#include <cstdint>
#include <initializer_list>

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
void* Ordinal_401(uint32_t, const char*, int, int);
void Ordinal_403(void*, const char*, int, int);
int q_bnclient_batch_b67_Ordinal_16(void* sock, void* buf, int len, int flags);
void bnclient_33a00(char* esi, uint8_t* data, uint32_t size, void* ebx, uint32_t a, uint32_t b);
void bnclient_2d280(void);
int bnclient_34050(const char* name, uint32_t* ftime, int** data, uint32_t* size);
void bnclient_211c0(void);
void bnclient_21180(void* tbl);
void bnclient_inconsistency(void);
extern uint32_t bn_dl_abort;      // DAT_6ff3f614
extern uint32_t bn_dl_percent;    // DAT_6ff3f508
extern uint32_t bn_news_cap, bn_news_count, bn_news_stride_base, bn_news_flag, bn_news_i, bn_news_hdr;
extern const char* bn_news_name;  // PTR_s_BNNews_D2DV
}

static uint16_t idea_mul(uint32_t a, uint32_t b) {
    a &= 0xffff; b &= 0xffff;
    if (a == 0) return (uint16_t)(1 - b);
    if (b == 0) return (uint16_t)(1 - a);
    uint32_t p = a * b, hi = p >> 16, lo = p & 0xffff;
    return (uint16_t)(lo - hi + (lo < hi));
}

// 1.13c BNClient+0x0DB70
// register inputs: out=EAX (int* receiving buffer), esi/ebx forwarded to 0x13A00, mode=caller stack word
uint32_t sub_DB70(uint8_t** out, void* sock, uint8_t flags, uint32_t size, char* esi, void* ebx, int mode) {
    uint8_t* buf = (uint8_t*)Ordinal_401(size, "..\\Source\\D2BNClient\\BnDownload.cpp", 0x387, 8);
    uint32_t got = 0;
    *out = buf;
    while (got < size) {
        uint32_t n = size - got;
        if (n > 0x5b4) n = 0x5b4;
        int r = q_bnclient_batch_b67_Ordinal_16(sock, buf + got, (int)n, 0);
        if (r == -1 || r < 1 || bn_dl_abort != 0) {
            Ordinal_403(*out, "..\\Source\\D2BNClient\\BnDownload.cpp", 0x391, 0);
            *out = nullptr;
            return 0;
        }
        got += (uint32_t)r;
        if (mode == -0x7ffffffe) bn_dl_percent = got * 100 / size;
    }
    if (flags & 1) bnclient_33a00(esi, *out, size, ebx, 0x13c680, 0x278d00);
    if (flags & 2) bnclient_2d280();
    return 1;
}

// 1.13c BNClient+0x12390
// register inputs: in=EAX (4 words); ecx unused. key = 52 words.
void sub_12390(const uint16_t* in, const uint16_t* key, uint16_t* out) {
    uint32_t x0 = in[0], x1 = in[1], x2 = in[2], x3 = in[3];
    for (int r = 0; r < 8; ++r, key += 6) {
        uint32_t a = idea_mul(x0, key[0]);
        uint32_t b = (x1 + key[1]) & 0xffff;
        uint32_t c = (x2 + key[2]) & 0xffff;
        uint32_t d = idea_mul(x3, key[3]);
        uint32_t t1 = idea_mul(c ^ a, key[4]);
        uint32_t t2 = idea_mul(((d ^ b) + t1) & 0xffff, key[5]);
        x0 = a ^ t2;
        x1 = t2 ^ c;
        x3 = d ^ ((t1 + t2) & 0xffff);
        x2 = ((t1 + t2) & 0xffff) ^ b;
    }
    out[0] = idea_mul(x0, key[0]);
    out[1] = (uint16_t)(key[1] + x2);
    out[2] = (uint16_t)(key[2] + x1);
    out[3] = idea_mul(x3, key[3]);
}

// 1.13c BNClient+0x122E0 (inlined helper of 0x125E0): multiplicative inverse mod 0x10001
static uint16_t idea_inv(uint32_t x) {
    if (x < 2) return (uint16_t)x;
    uint32_t t1 = 0x10001 / x, y = 0x10001 % x, t0 = 1;
    if (y == 1) return (uint16_t)(1 - t1);
    for (;;) {
        uint32_t q = x / y;
        x = x % y;
        t0 += q * t1;
        if (x == 1) return (uint16_t)t0;
        uint32_t q2 = y / x;
        y = y % x;
        t1 += q2 * t0;
        if (y == 1) return (uint16_t)(1 - t1);
    }
}

// 1.13c BNClient+0x125E0
// register input: key=EAX (52-word encrypt schedule); out=stack param (52 words, decrypt schedule)
void sub_125E0(const uint16_t* key, uint16_t* out) {
    uint16_t t[52];
    t[48] = idea_inv(key[0]);
    t[49] = (uint16_t)-key[1];
    t[50] = (uint16_t)-key[2];
    t[51] = idea_inv(key[3]);
    const uint16_t* p = key + 4;
    for (int j = 0; j < 7; ++j, p += 6) {
        uint16_t* o = t + 42 - 6 * j;
        o[5] = p[1];
        o[4] = p[0];
        o[3] = idea_inv(p[5]);
        o[2] = (uint16_t)-p[4];
        o[1] = (uint16_t)-p[3];
        o[0] = idea_inv(p[2]);
    }
    t[5] = p[1];
    t[4] = p[0];
    t[3] = idea_inv(p[5]);
    t[2] = (uint16_t)-p[4];
    t[1] = (uint16_t)-p[3];
    t[0] = idea_inv(p[2]);
    for (int i = 0; i < 52; ++i) out[i] = t[i];
}

// 1.13c BNClient+0x12A70
uint32_t sub_12A70(void) {
    uint32_t ok = 0, ft[2];
    int* data = nullptr;
    uint32_t sz;
    if (bnclient_34050(bn_news_name, ft, &data, &sz)) {
        bn_news_hdr = (uint32_t)data[0];
        uint32_t n = (uint32_t)data[1];
        const uint8_t* p = (const uint8_t*)(data + 2);
        if (n < 0x101) {
            bn_news_i = 0;
            if (bn_news_cap < n) bnclient_211c0();
            bn_news_flag = 0;
            uint32_t prev = 0, k = 0;
            bool bad = false;
            while (k < n) {
                if (bn_news_cap < bn_news_i + 1) bnclient_211c0();
                uint8_t* e = (uint8_t*)(uintptr_t)(bn_news_i * 0x408 + bn_news_stride_base);
                bn_news_i++;
                *(uint32_t*)e = 0;
                *(uint32_t*)(e + 4) = *(const uint32_t*)p;
                const char* s = (const char*)p + 4;
                uint32_t len = 0;
                while (s[len]) ++len;
                if (len > 0x3ff) { bad = true; break; }
                for (uint32_t i = 0; i <= len; ++i) e[8 + i] = (uint8_t)s[i];
                uint32_t cur = *(uint32_t*)(e + 4);
                p += 4 + len + 1;
                if (prev > cur) break;
                ++k; prev = cur;
            }
            (void)bad;
            ok = (k == n);
        }
    }
    if (!ok) { bn_news_cap = bn_news_i = bn_news_flag = bn_news_hdr = 0; }
    if (data) Ordinal_403(data, "..\\Source\\D2BNClient\\BNNews.cpp", 299, 0);
    return ok;
}

struct TryBlk { int32_t tryLow, tryHigh, catchHigh, nCatches; uint32_t handlers; };
struct FuncInfoB { uint32_t magic; int32_t maxState; uint32_t unwind; uint32_t nTry; uint32_t tryMap; };

// 1.13c BNClient+0x17780
const TryBlk* sub_17780(const FuncInfoB* fi, int state, int cur, uint32_t* lo, uint32_t* hi) {
    uint32_t n = fi->nTry;
    const TryBlk* map = (const TryBlk*)(uintptr_t)fi->tryMap;
    uint32_t i = n, a = n, b = n;
    while (state >= 0) {
        uint32_t keep = a;
        if (i == 0xffffffffu) bnclient_inconsistency();
        --i;
        if ((map[i].tryHigh < cur && cur <= map[i].catchHigh) || i == 0xffffffffu) {
            --state; a = i; b = keep;
        } else a = keep;
    }
    ++i;
    *lo = i; *hi = b;
    if (fi->nTry < b || b < i) bnclient_inconsistency();
    return map + i;
}
