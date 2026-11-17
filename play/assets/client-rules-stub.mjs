// A stand-in for the engine module in unit tests: the same rules the C++ exports run
// (stats_cell_bcea0.h, party_row_76790.h), so a panel test can bind it without a wasm build.
import { bindClientRules } from './client-rules.mjs';

export function bindStubRules() {
  const heap = new Int32Array(256);
  bindClientRules({
    HEAP32: heap,
    _malloc: () => 0,
    _free: () => {},
    ccall(name, _ret, _types, a) {
      if (name === 'd2_client_cell_colour') return a[0] > a[1] ? 3 : a[0] < a[1] ? 1 : a[2];
      if (name === 'd2_client_party_row_mode') {
        const [share, pt, mine, hostile] = a;
        return hostile ? 1 : share > 1 && pt !== 0xffff ? (mine === pt ? 2 : 8) : 0;
      }
      if (name === 'd2_client_quest_tabs') {   // quest_rules_1ef10.h tabShown
        const [sel, exp, mask] = a; let out = 0;
        for (let i = 0; i < 5; i++) if (!(!exp && i > 3) && (i === sel || i === 0 || (mask >> i) & 1)) out |= 1 << i;
        return out;
      }
      if (name === 'd2_client_quest_icon') {   // quest_rules_1ef10.h iconFor
        const [mode, prev, anim, idx] = a;
        if (mode === 0) return anim >= 0x19 ? 0x18 : anim;
        if (mode === 1) return prev ? (1 << 16) | idx : 0x18;
        return mode === 2 ? 0x1a : prev ? 0x19 : 0;
      }
      if (name === 'd2_client_resist_cell') {
        let [v, max, col, adj] = a; v += adj;
        const cap = Math.min(0x5f, max + 0x4b);
        if (v <= -100) v = -100;
        if (v >= cap) { v = cap; if (col !== 3) { heap[Number(a[4]) >> 2] = v; return 4; } }
        if (v < 0) col = 1;
        heap[Number(a[4]) >> 2] = v;
        return col;
      }
      if (name === 'd2_client_exp_bar_pixels') {   // hud_bar_rules.h expBarPixels
        let [num, den] = a;
        if (!den) return 0;
        if (num >= 0x226b902) { num >>>= 7; den >>>= 7; }
        if (!den) return 0;
        const bar = Number((BigInt(num) * 0x77n & 0xffffffffn) / BigInt(den));
        return bar > 0x77 ? 0 : bar;
      }
      if (name === 'd2_client_orb_fill') {   // hud_bar_rules.h orbFill
        let [cur, mx, kind, alive] = a;
        if (!mx) return 0;
        if (kind === 1 && cur > mx) cur = mx;
        let fill = Math.trunc(Number(BigInt.asIntN(32, BigInt(cur >>> 0) * 80n)) / mx);
        if (kind === 0 && alive && fill > 0 && fill <= 2) fill = 2;
        return fill;
      }
      if (name === 'd2_client_stamina_bar') {   // hud_bar_rules.h staminaBar
        let [cur, mx, over] = a, slot = 1;
        if (mx + 5 < cur || over) { slot = 2; mx = cur; }
        if (mx <= 0) return 0;
        const w = Number(BigInt.asIntN(32, BigInt(cur >>> 0) * 0x66n)) / mx | 0;
        if (w < 0x19) slot = 0;
        return (slot << 16) | w;
      }
      if (name === 'd2_client_draw_hud_buttons') {   // d2client_28320.cpp / d2client_26880.cpp at 800x600
        const [mx, my, running, runHot, arrowOpen, arrowHot, cellRun, cellMenu, out] = a;
        const runIn = mx >= 255 && mx <= 272 && my >= 572 && my <= 592, menuIn = mx >= 392 && mx <= 405 && my >= 561 && my <= 587;
        const recs = [[0xd096, cellRun, (running ? 2 : 0) + (runHot && runIn ? 1 : 0), 255, 590, -1, 5, 0],
          [0xd096, cellMenu, (arrowOpen ? 2 : 0) + (arrowHot && menuIn ? 1 : 0), 392, 584, -1, 5, 0]];
        recs.forEach((r, i) => r.forEach((v, j) => { heap[(out >> 2) + i * 8 + j] = v; }));
        return recs.length;
      }
      if (name === 'd2_client_draw_orbs') {   // d2client_27bd0.cpp / d2client_27a90.cpp with no poison or frozen state, at 800x600
        const [life, lifeMax, mana, manaMax, alive, fluid, overlap, out] = a, recs = [];
        const fill = (cur, mx, kind) => {
          if (kind === 1 && cur > mx) cur = mx;
          let f = Math.trunc(Number(BigInt.asIntN(32, BigInt(cur >>> 0) * 80n)) / mx);
          if (kind === 0 && alive && f > 0 && f <= 2) f = 2;
          return f;
        };
        if (lifeMax) {
          const f = fill(life, lifeMax, 0);
          if (f) recs.push([0xd1b6, fluid, 0, 0x1d, 587, 0, f, 5]);
          recs.push([0xd096, overlap, 0, 0x1c, 595, -1, 5, 0]);
        }
        if (manaMax) {
          const f = fill(mana, manaMax, 1);
          recs.push([0xd1b6, fluid, 1, 689, 587, 0, f, 5], [0xd096, overlap, 1, 690, 591, -1, 5, 0]);
        }
        recs.forEach((r, i) => r.forEach((v, j) => { heap[(out >> 2) + i * 8 + j] = v; }));
        return recs.length;
      }
      throw new Error(`stub: ${name}`);
    },
  });
}
