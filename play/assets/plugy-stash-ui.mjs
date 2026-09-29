// PlugY shared-stash page control over the retail stash panel (PY1). Default ON for players (the maintainer); the shell turns it off under ?parity=1, where it draws nothing. Page 0 is the engine's own stash; pages 1.. are
// .sss pages (plugy-stash.mjs). Placeholder art: plain canvas arrows + "n/m" -- PlugY's own art is not
// captured. The shell swaps a page in/out of the engine stash via d2_session_stash_export/clear/insert (JM bytes).
export function createPlugyStash({ enabled = false, pages = 2, onPage = null } = {}) {
  const s = { enabled, pages, page: 0 };
  const rect = (g) => ({ x: g.left + 96, y: g.top + 8, w: 96, h: 18 });
  const scaleOf = (w, h) => { const k = Math.min(1, w / 800, h / 600); return { k, x: (w - 800 * k) / 2, y: (h - 600 * k) / 2 }; };
  return Object.assign(s, {
    rect,
    draw(ctx, w, h, g, open) {
      if (!s.enabled || !open || !g) return null;
      const r = rect(g), o = scaleOf(w, h);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.k, o.k);
      ctx.fillStyle = '#000'; ctx.fillRect(r.x, r.y, r.w, r.h);
      ctx.fillStyle = '#c7b377'; ctx.font = '12px sans-serif'; ctx.textAlign = 'center'; ctx.textBaseline = 'middle';
      ctx.fillText('<', r.x + 10, r.y + 9); ctx.fillText(`${s.page + 1}/${s.pages}`, r.x + 48, r.y + 9); ctx.fillText('>', r.x + 86, r.y + 9);
      ctx.restore();
      return { ...r, page: s.page, pages: s.pages };
    },
    /** Returns true when the click hit the control (and pages it). */
    click(px, py, w, h, g, open) {
      if (!s.enabled || !open || !g) return false;
      const r = rect(g), o = scaleOf(w, h), x = (px - o.x) / o.k, y = (py - o.y) / o.k;
      if (x < r.x || x >= r.x + r.w || y < r.y || y >= r.y + r.h) return false;
      const was = s.page;
      if (x < r.x + 32) s.page = (s.page + s.pages - 1) % s.pages; else if (x >= r.x + 64) s.page = (s.page + 1) % s.pages;
      if (s.page !== was && onPage && onPage(was, s.page) === false) s.page = was;   // a refused swap (engine insert failed, nothing lost) stays on the page
      return true;
    },
  });
}
