// Prior art used: no D2MOO D2Client module. zlib's inflate_blocks state machine
// (reference/D2Ex2/libpng/zlib.h for status values) cross-checks the mode
// numbering; the cached 1.13c body supplies every transition, message and
// allocation/release order. Bit buffer, input and window are live native views.
#include "inflate_blocks.h"
#include "inflate_codes.h"
#include "inflate_trees.h"

namespace {
constexpr uint8_t kBorder[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
D2ClientInflateBlocksCallees g_callees;
}
static_assert(sizeof(void*) != 4 || sizeof(D2ClientInflateCodes) == 0x1c, "retail codes object is 0x1c bytes");

void D2Client_SetInflateBlocksCallees(const D2ClientInflateBlocksCallees* c) { g_callees = c ? *c : D2ClientInflateBlocksCallees{}; }

// The fixed trees (zlib's inflate_trees_fixed output, as retail stores it: 9/5 root bits, 512 + 32 entries).
uint32_t D2Client_FixedLiteralBits = 9;
uint32_t D2Client_FixedDistanceBits = 5;
D2ClientInflateHuft D2Client_FixedLiteralTree[512] = {
{96,7,{256}},{0,8,{80}},{0,8,{16}},{84,8,{115}},{82,7,{31}},{0,8,{112}},{0,8,{48}},{0,9,{192}},{80,7,{10}},{0,8,{96}},
{0,8,{32}},{0,9,{160}},{0,8,{0}},{0,8,{128}},{0,8,{64}},{0,9,{224}},{80,7,{6}},{0,8,{88}},{0,8,{24}},{0,9,{144}},
{83,7,{59}},{0,8,{120}},{0,8,{56}},{0,9,{208}},{81,7,{17}},{0,8,{104}},{0,8,{40}},{0,9,{176}},{0,8,{8}},{0,8,{136}},
{0,8,{72}},{0,9,{240}},{80,7,{4}},{0,8,{84}},{0,8,{20}},{85,8,{227}},{83,7,{43}},{0,8,{116}},{0,8,{52}},{0,9,{200}},
{81,7,{13}},{0,8,{100}},{0,8,{36}},{0,9,{168}},{0,8,{4}},{0,8,{132}},{0,8,{68}},{0,9,{232}},{80,7,{8}},{0,8,{92}},
{0,8,{28}},{0,9,{152}},{84,7,{83}},{0,8,{124}},{0,8,{60}},{0,9,{216}},{82,7,{23}},{0,8,{108}},{0,8,{44}},{0,9,{184}},
{0,8,{12}},{0,8,{140}},{0,8,{76}},{0,9,{248}},{80,7,{3}},{0,8,{82}},{0,8,{18}},{85,8,{163}},{83,7,{35}},{0,8,{114}},
{0,8,{50}},{0,9,{196}},{81,7,{11}},{0,8,{98}},{0,8,{34}},{0,9,{164}},{0,8,{2}},{0,8,{130}},{0,8,{66}},{0,9,{228}},
{80,7,{7}},{0,8,{90}},{0,8,{26}},{0,9,{148}},{84,7,{67}},{0,8,{122}},{0,8,{58}},{0,9,{212}},{82,7,{19}},{0,8,{106}},
{0,8,{42}},{0,9,{180}},{0,8,{10}},{0,8,{138}},{0,8,{74}},{0,9,{244}},{80,7,{5}},{0,8,{86}},{0,8,{22}},{192,8,{0}},
{83,7,{51}},{0,8,{118}},{0,8,{54}},{0,9,{204}},{81,7,{15}},{0,8,{102}},{0,8,{38}},{0,9,{172}},{0,8,{6}},{0,8,{134}},
{0,8,{70}},{0,9,{236}},{80,7,{9}},{0,8,{94}},{0,8,{30}},{0,9,{156}},{84,7,{99}},{0,8,{126}},{0,8,{62}},{0,9,{220}},
{82,7,{27}},{0,8,{110}},{0,8,{46}},{0,9,{188}},{0,8,{14}},{0,8,{142}},{0,8,{78}},{0,9,{252}},{96,7,{256}},{0,8,{81}},
{0,8,{17}},{85,8,{131}},{82,7,{31}},{0,8,{113}},{0,8,{49}},{0,9,{194}},{80,7,{10}},{0,8,{97}},{0,8,{33}},{0,9,{162}},
{0,8,{1}},{0,8,{129}},{0,8,{65}},{0,9,{226}},{80,7,{6}},{0,8,{89}},{0,8,{25}},{0,9,{146}},{83,7,{59}},{0,8,{121}},
{0,8,{57}},{0,9,{210}},{81,7,{17}},{0,8,{105}},{0,8,{41}},{0,9,{178}},{0,8,{9}},{0,8,{137}},{0,8,{73}},{0,9,{242}},
{80,7,{4}},{0,8,{85}},{0,8,{21}},{80,8,{258}},{83,7,{43}},{0,8,{117}},{0,8,{53}},{0,9,{202}},{81,7,{13}},{0,8,{101}},
{0,8,{37}},{0,9,{170}},{0,8,{5}},{0,8,{133}},{0,8,{69}},{0,9,{234}},{80,7,{8}},{0,8,{93}},{0,8,{29}},{0,9,{154}},
{84,7,{83}},{0,8,{125}},{0,8,{61}},{0,9,{218}},{82,7,{23}},{0,8,{109}},{0,8,{45}},{0,9,{186}},{0,8,{13}},{0,8,{141}},
{0,8,{77}},{0,9,{250}},{80,7,{3}},{0,8,{83}},{0,8,{19}},{85,8,{195}},{83,7,{35}},{0,8,{115}},{0,8,{51}},{0,9,{198}},
{81,7,{11}},{0,8,{99}},{0,8,{35}},{0,9,{166}},{0,8,{3}},{0,8,{131}},{0,8,{67}},{0,9,{230}},{80,7,{7}},{0,8,{91}},
{0,8,{27}},{0,9,{150}},{84,7,{67}},{0,8,{123}},{0,8,{59}},{0,9,{214}},{82,7,{19}},{0,8,{107}},{0,8,{43}},{0,9,{182}},
{0,8,{11}},{0,8,{139}},{0,8,{75}},{0,9,{246}},{80,7,{5}},{0,8,{87}},{0,8,{23}},{192,8,{0}},{83,7,{51}},{0,8,{119}},
{0,8,{55}},{0,9,{206}},{81,7,{15}},{0,8,{103}},{0,8,{39}},{0,9,{174}},{0,8,{7}},{0,8,{135}},{0,8,{71}},{0,9,{238}},
{80,7,{9}},{0,8,{95}},{0,8,{31}},{0,9,{158}},{84,7,{99}},{0,8,{127}},{0,8,{63}},{0,9,{222}},{82,7,{27}},{0,8,{111}},
{0,8,{47}},{0,9,{190}},{0,8,{15}},{0,8,{143}},{0,8,{79}},{0,9,{254}},{96,7,{256}},{0,8,{80}},{0,8,{16}},{84,8,{115}},
{82,7,{31}},{0,8,{112}},{0,8,{48}},{0,9,{193}},{80,7,{10}},{0,8,{96}},{0,8,{32}},{0,9,{161}},{0,8,{0}},{0,8,{128}},
{0,8,{64}},{0,9,{225}},{80,7,{6}},{0,8,{88}},{0,8,{24}},{0,9,{145}},{83,7,{59}},{0,8,{120}},{0,8,{56}},{0,9,{209}},
{81,7,{17}},{0,8,{104}},{0,8,{40}},{0,9,{177}},{0,8,{8}},{0,8,{136}},{0,8,{72}},{0,9,{241}},{80,7,{4}},{0,8,{84}},
{0,8,{20}},{85,8,{227}},{83,7,{43}},{0,8,{116}},{0,8,{52}},{0,9,{201}},{81,7,{13}},{0,8,{100}},{0,8,{36}},{0,9,{169}},
{0,8,{4}},{0,8,{132}},{0,8,{68}},{0,9,{233}},{80,7,{8}},{0,8,{92}},{0,8,{28}},{0,9,{153}},{84,7,{83}},{0,8,{124}},
{0,8,{60}},{0,9,{217}},{82,7,{23}},{0,8,{108}},{0,8,{44}},{0,9,{185}},{0,8,{12}},{0,8,{140}},{0,8,{76}},{0,9,{249}},
{80,7,{3}},{0,8,{82}},{0,8,{18}},{85,8,{163}},{83,7,{35}},{0,8,{114}},{0,8,{50}},{0,9,{197}},{81,7,{11}},{0,8,{98}},
{0,8,{34}},{0,9,{165}},{0,8,{2}},{0,8,{130}},{0,8,{66}},{0,9,{229}},{80,7,{7}},{0,8,{90}},{0,8,{26}},{0,9,{149}},
{84,7,{67}},{0,8,{122}},{0,8,{58}},{0,9,{213}},{82,7,{19}},{0,8,{106}},{0,8,{42}},{0,9,{181}},{0,8,{10}},{0,8,{138}},
{0,8,{74}},{0,9,{245}},{80,7,{5}},{0,8,{86}},{0,8,{22}},{192,8,{0}},{83,7,{51}},{0,8,{118}},{0,8,{54}},{0,9,{205}},
{81,7,{15}},{0,8,{102}},{0,8,{38}},{0,9,{173}},{0,8,{6}},{0,8,{134}},{0,8,{70}},{0,9,{237}},{80,7,{9}},{0,8,{94}},
{0,8,{30}},{0,9,{157}},{84,7,{99}},{0,8,{126}},{0,8,{62}},{0,9,{221}},{82,7,{27}},{0,8,{110}},{0,8,{46}},{0,9,{189}},
{0,8,{14}},{0,8,{142}},{0,8,{78}},{0,9,{253}},{96,7,{256}},{0,8,{81}},{0,8,{17}},{85,8,{131}},{82,7,{31}},{0,8,{113}},
{0,8,{49}},{0,9,{195}},{80,7,{10}},{0,8,{97}},{0,8,{33}},{0,9,{163}},{0,8,{1}},{0,8,{129}},{0,8,{65}},{0,9,{227}},
{80,7,{6}},{0,8,{89}},{0,8,{25}},{0,9,{147}},{83,7,{59}},{0,8,{121}},{0,8,{57}},{0,9,{211}},{81,7,{17}},{0,8,{105}},
{0,8,{41}},{0,9,{179}},{0,8,{9}},{0,8,{137}},{0,8,{73}},{0,9,{243}},{80,7,{4}},{0,8,{85}},{0,8,{21}},{80,8,{258}},
{83,7,{43}},{0,8,{117}},{0,8,{53}},{0,9,{203}},{81,7,{13}},{0,8,{101}},{0,8,{37}},{0,9,{171}},{0,8,{5}},{0,8,{133}},
{0,8,{69}},{0,9,{235}},{80,7,{8}},{0,8,{93}},{0,8,{29}},{0,9,{155}},{84,7,{83}},{0,8,{125}},{0,8,{61}},{0,9,{219}},
{82,7,{23}},{0,8,{109}},{0,8,{45}},{0,9,{187}},{0,8,{13}},{0,8,{141}},{0,8,{77}},{0,9,{251}},{80,7,{3}},{0,8,{83}},
{0,8,{19}},{85,8,{195}},{83,7,{35}},{0,8,{115}},{0,8,{51}},{0,9,{199}},{81,7,{11}},{0,8,{99}},{0,8,{35}},{0,9,{167}},
{0,8,{3}},{0,8,{131}},{0,8,{67}},{0,9,{231}},{80,7,{7}},{0,8,{91}},{0,8,{27}},{0,9,{151}},{84,7,{67}},{0,8,{123}},
{0,8,{59}},{0,9,{215}},{82,7,{19}},{0,8,{107}},{0,8,{43}},{0,9,{183}},{0,8,{11}},{0,8,{139}},{0,8,{75}},{0,9,{247}},
{80,7,{5}},{0,8,{87}},{0,8,{23}},{192,8,{0}},{83,7,{51}},{0,8,{119}},{0,8,{55}},{0,9,{207}},{81,7,{15}},{0,8,{103}},
{0,8,{39}},{0,9,{175}},{0,8,{7}},{0,8,{135}},{0,8,{71}},{0,9,{239}},{80,7,{9}},{0,8,{95}},{0,8,{31}},{0,9,{159}},
{84,7,{99}},{0,8,{127}},{0,8,{63}},{0,9,{223}},{82,7,{27}},{0,8,{111}},{0,8,{47}},{0,9,{191}},{0,8,{15}},{0,8,{143}},
{0,8,{79}},{0,9,{255}},
};
D2ClientInflateHuft D2Client_FixedDistanceTree[32] = {
{80,5,{1}},{87,5,{257}},{83,5,{17}},{91,5,{4097}},{81,5,{5}},{89,5,{1025}},{85,5,{65}},{93,5,{16385}},{80,5,{3}},
{88,5,{513}},{84,5,{33}},{92,5,{8193}},{82,5,{9}},{90,5,{2049}},{86,5,{129}},{192,5,{24577}},{80,5,{2}},{87,5,{385}},
{83,5,{25}},{91,5,{6145}},{81,5,{7}},{89,5,{1537}},{85,5,{97}},{93,5,{24577}},{80,5,{4}},{88,5,{769}},{84,5,{49}},
{92,5,{12289}},{82,5,{13}},{90,5,{3073}},{86,5,{193}},{192,5,{24577}},
};

// 1.13c D2Client+0x0F4C0
int32_t D2Client_InflateBlocks(D2ClientInflateBlocks& s, D2ClientInflateInput& in,
    D2ClientInflateOutput& out, const D2ClientInflateAllocator& al, int32_t r) {
    auto& w = s.window;
    auto mask = [](uint32_t n) { return (uint32_t{1} << n) - 1; };
    auto leave = [&](int32_t code) {
        return g_callees.flush ? g_callees.flush(w, out, code) : D2Client_FlushInflateWindow(w, out, code);
    };
    auto need = [&](uint32_t n) {
        while (s.bitCount < n) {
            if (!in.available) return false;
            --in.available; ++in.total; r = 0;
            s.bitBuffer |= uint32_t{*in.next++} << (s.bitCount & 31);
            s.bitCount += 8;
        }
        return true;
    };
    auto drop = [&](uint32_t n) { s.bitBuffer >>= n; s.bitCount -= n; };
    auto space = [&]() -> std::ptrdiff_t {
        return w.write < w.read ? w.read - w.write - 1 : w.end - w.write;
    };
    auto wrap = [&]() { if (w.write == w.end && w.read != w.begin) w.write = w.begin; };
    auto bad = [&](const char* msg) { s.mode = 9; in.message = msg; return leave(-3); };
    auto done = [&]() { s.mode = s.last ? 7 : 0; };
    for (;;) {
        switch (s.mode) {
        case 0: {
            if (!need(3)) return leave(r);
            s.last = s.bitBuffer & 1;
            const uint32_t type = (s.bitBuffer & 7) >> 1;
            if (type == 1) {
                // retail keeps the 3 header bits when the codes object cannot be had
                auto* codes = (g_callees.newCodes ? g_callees.newCodes : D2Client_NewInflateCodes)(al,
                    uint8_t(D2Client_FixedLiteralBits), uint8_t(D2Client_FixedDistanceBits),
                    D2Client_FixedLiteralTree, D2Client_FixedDistanceTree);
                s.sub.codes = codes;
                if (!codes) return leave(-4);
                drop(3);
                s.mode = 6;
                break;
            }
            drop(3);
            if (type == 0) {
                const uint32_t t = s.bitCount & 7;
                drop(t);
                s.mode = 1;
            } else if (type == 2) s.mode = 3;
            else { s.mode = 9; in.message = "invalid block type"; return leave(-3); }
            break;
        }
        case 1:
            if (!need(32)) return leave(r);
            if ((~s.bitBuffer >> 16) != (s.bitBuffer & 0xffff)) {
                s.mode = 9; in.message = "invalid stored block lengths"; return leave(-3);
            }
            s.sub.trees.table = s.bitBuffer & 0xffff;
            s.bitBuffer = 0; s.bitCount = 0;
            if (s.sub.trees.table) s.mode = 2; else done();
            break;
        case 2: {
            if (!in.available) return leave(r);
            if (!space()) {
                wrap();
                if (!space()) {
                    r = leave(r);
                    wrap();
                    if (!space()) return leave(r);
                }
            }
            r = 0;
            uint32_t n = s.sub.trees.table;
            if (n > in.available) n = in.available;
            if (n > space()) n = uint32_t(space());
            for (uint32_t i = 0; i < n; ++i) w.write[i] = in.next[i];
            w.write += n; in.next += n; in.available -= n; in.total += n;
            s.sub.trees.table -= n;
            if (!s.sub.trees.table) done();
            break;
        }
        case 3: {
            if (!need(14)) return leave(r);
            const uint32_t t = s.bitBuffer & 0x3fff;
            s.sub.trees.table = t;
            if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
                return bad("too many length or distance symbols");
            auto* lengths = static_cast<uint32_t*>(al.allocate(al.opaque, 258 + (t & 0x1f) + ((t >> 5) & 0x1f), 4));
            s.sub.trees.lengths = lengths;
            if (!lengths) return leave(-4);
            drop(14);
            s.sub.trees.index = 0;
            s.mode = 4;
            break;
        }
        case 4: {
            auto& tr = s.sub.trees;
            while (tr.index < (tr.table >> 10) + 4) {
                if (!need(3)) return leave(r);
                tr.lengths[kBorder[tr.index++]] = s.bitBuffer & 7;
                drop(3);
            }
            while (tr.index < 19) tr.lengths[kBorder[tr.index++]] = 0;
            tr.bits = 7;
            const int32_t e = (g_callees.bitTree ? g_callees.bitTree : D2Client_InflateBitTree)(tr.lengths, tr.bits,
                tr.tree, s.workspace, al, in.message);
            if (e) {
                if (e == -3) { al.release(al.opaque, tr.lengths); s.mode = 9; }
                return leave(e);
            }
            tr.index = 0;
            s.mode = 5;
            break;
        }
        case 5: {
            auto& tr = s.sub.trees;
            for (;;) {
                const uint32_t t = tr.table, total = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
                if (tr.index >= total) break;
                if (!need(tr.bits)) return leave(r);
                const auto& h = tr.tree[s.bitBuffer & mask(tr.bits)];
                const uint32_t v = h.value.base, hb = h.bits;
                if (v < 16) {
                    drop(hb);
                    tr.lengths[tr.index++] = v;
                    continue;
                }
                const uint32_t extra = v == 18 ? 7 : v - 14, rep0 = v == 18 ? 11 : 3;
                if (!need(hb + extra)) return leave(r);
                drop(hb);
                const uint32_t count = rep0 + (s.bitBuffer & mask(extra));
                drop(extra);
                if (total < count + tr.index || (v == 16 && tr.index == 0)) {
                    al.release(al.opaque, tr.lengths);
                    return bad("invalid bit length repeat");
                }
                const uint32_t fill = v == 16 ? tr.lengths[tr.index - 1] : 0;
                for (uint32_t i = 0; i < count; ++i) tr.lengths[tr.index++] = fill;
            }
            D2ClientInflateHuft *tl = nullptr, *td = nullptr;
            uint32_t bl = 9, bd = 6;
            uint32_t* const lengths = tr.lengths;
            const uint32_t t = tr.table;
            tr.tree = nullptr;
            const int32_t e = (g_callees.dynamicTrees ? g_callees.dynamicTrees : D2Client_InflateDynamicTrees)(
                (t & 0x1f) + 257, ((t >> 5) & 0x1f) + 1, lengths, bl, bd, tl, td, s.workspace, al, in.message);
            if (e) {
                if (e == -3) { al.release(al.opaque, lengths); s.mode = 9; }
                return leave(e);
            }
            // retail allocates the codes object here itself (0x1c bytes) and sets only these five fields
            auto* codes = static_cast<D2ClientInflateCodes*>(al.allocate(al.opaque, 1, sizeof(D2ClientInflateCodes)));
            if (!codes) return leave(-4);
            codes->literalBits = uint8_t(bl);
            codes->distanceBits = uint8_t(bd);
            codes->mode = 0;
            codes->literalTree = tl;
            codes->distanceTree = td;
            s.sub.codes = codes;
            al.release(al.opaque, lengths);
            s.mode = 6;
            break;
        }
        case 6: {
            r = (g_callees.codes ? g_callees.codes : D2Client_InflateCodes)(s, in, out, r);
            if (r != 1) return leave(r);
            r = 0;
            al.release(al.opaque, s.sub.codes);
            done();
            break;
        }
        case 7:
            r = leave(r);
            if (w.read != w.write) return leave(r);
            s.mode = 8;
            break;
        case 8:
            return leave(1);
        case 9:
            return leave(-3);
        default:
            return leave(-2);
        }
    }
}
