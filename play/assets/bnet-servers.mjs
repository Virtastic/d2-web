// The Battle.net server list: a config FILE this project owns, not a player setting.
//
// the list is configured UNDER THE HOOD by us -- "a file in the repo, we edit the
// file, the page reads it at boot. Not a URL param, not an env var, not anything a player touches."
// So `play/assets/bnet-servers.json` is the only surface there is. This module adds no input field,
// no settings panel and no way for a player to type or edit an endpoint: the screen it draws only
// RENDERS the configured entries and lets a player pick one.
//
// This client HOSTS no Battle.net. Emulating Battle.net is another project's legality matter, and the
// server-side protocol code this repo already has (`src/core/realm/`) is a different
// lane and is untouched here. Each entry is a REGION, and the region IS the server the client points
// at; `host` is the endpoint it would dial.
//
// OFF BY DEFAULT, and "off" is one state rather than several: the shipped file is empty, and an empty
// file, a missing file, a 404 and malformed JSON all resolve to the same `{ enabled: false }`. With
// that, Battle.net stays exactly what retail's strict page has -- the BATTLE.NET entry inert, the
// gateway bar at retail's own default -- and the page attempts no Battle.net connection at all.

/** The config file, relative to the document (staged to `build-wasm/site/assets/` by ci/stage.sh). */
export const BNET_CONFIG_URL = 'assets/bnet-servers.json';

/** Retail 1.13c's own bar text under BATTLE.NET, and the prefix a picked region replaces it with. */
export const DEFAULT_GATEWAY_LABEL = 'GATEWAY: U.S. EAST';
export const GATEWAY_BAR_PREFIX = 'GATEWAY: ';

/**
 * Parse the config file's text into `{ enabled, servers, error }`.
 *
 * `enabled` is exactly "the file listed at least one usable server", so a config that is empty,
 * malformed or missing half an entry is the OFF state and never a half-on one. A `region` with no
 * `host` (or the reverse) is dropped rather than pointed at an empty endpoint.
 */
export function parseBnetServers(text) {
  let parsed;
  try { parsed = JSON.parse(text); }
  catch (error) { return { enabled: false, servers: [], error: `config is not JSON (${error})` }; }
  const raw = Array.isArray(parsed) ? parsed : parsed?.servers;
  if (!Array.isArray(raw)) return { enabled: false, servers: [], error: 'config has no "servers" array' };
  const servers = raw
    .map((s) => ({ region: String(s?.region ?? s?.name ?? '').trim(), host: String(s?.host ?? s?.address ?? '').trim() }))
    .filter((s) => s.region && s.host);
  return { enabled: servers.length > 0, servers, error: '' };
}

/**
 * Read the config at boot. A missing or unreadable file is the OFF state with the reason kept for the
 * evidence, never an exception that could strand the front end (the same rule the title screen's own
 * optional pieces follow). `cache: 'no-store'` matters: the file is edited in the repo and the page is
 * reloaded, and a revalidated 304 would keep serving the list that was there before the edit.
 */
export async function loadBnetServers(fetchImpl = globalThis.fetch) {
  const base = globalThis.document?.baseURI ?? 'http://localhost/';
  const url = new URL(BNET_CONFIG_URL, base).href;
  try {
    const res = await fetchImpl(url, { cache: 'no-store' });
    if (!res.ok) return { enabled: false, servers: [], url, error: `config HTTP ${res.status}` };
    return { ...parseBnetServers(await res.text()), url };
  } catch (error) { return { enabled: false, servers: [], url, error: String(error) }; }
}

/**
 * Where the dialog puts its rows: one per configured server, on retail's own button geometry (a
 * WideButtonBlank cel is 272x35), centred as a block. The layout is OURS -- retail has no local
 * gateway screen to copy (the gateway list is Battle.net's own, drawn after a BNCS login, and this
 * repo has neither a capture nor prior art for it), so this is a declared gap, not a measurement.
 */
export function gatewayRows(servers, { width = 800, height = 600, rowWidth = 272, rowHeight = 35, step = 44 } = {}) {
  const total = rowHeight + Math.max(0, servers.length - 1) * step;
  const top = Math.trunc((height - total) / 2);
  return servers.map((s, i) => ({ region: s.region, host: s.host, index: i,
    x: Math.trunc((width - rowWidth) / 2), y: top + i * step, w: rowWidth, h: rowHeight }));
}

/**
 * The region-select dialog: the configured servers as retail's own buttons under retail's own font,
 * over a dimmed title screen. It owns no config -- it is handed the list the file gave -- and its only
 * verb a player has is `pick`.
 *
 * `evidence.drawn` is IDENTITY evidence: for each row, the glyphs actually blitted (character, font
 * DC6 frame index and path) and the button cels actually drawn (retail DC6 path, frame index, anchor,
 * FNV-1a of the encoded frame bytes). A row that only counted rows would pass on a screen of wrong
 * labels; a consumer compares the PAINTED text to the configured region.
 */
export function createGatewaySelector({ servers, buttons, drawLabel, drawHeading = null, recordButton, width = 800, height = 600 }) {
  // The row is retail's OWN composed button, not a box we invented: drawing a 512-wide
  // WideButtonBlank inside a 272-wide rect would overflow the rect we hit-test against.
  const rows = gatewayRows(servers, { width, height, rowWidth: buttons?.wide?.[0]?.width || 272,
    rowHeight: buttons?.wide?.[0]?.height || 35 });
  let open = false, selected = -1, hot = -1, pressed = -1, drawn = { heading: null, rows: [] };
  const rowAt = (x, y) => rows.findIndex((r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h);
  return {
    get open() { return open; },
    get selected() { return selected >= 0 ? { ...servers[selected] } : null; },
    get evidence() {
      return { enabled: true, configured: servers.map((s) => ({ region: s.region, host: s.host })),
        rows: rows.map((r) => ({ region: r.region, host: r.host, x: r.x, y: r.y, w: r.w, h: r.h })),
        open, hot: hot >= 0 ? rows[hot].region : null, selected: this.selected, drawn };
    },
    openDialog() { if (!servers.length) return false; open = true; hot = -1; pressed = -1; return true; },
    closeDialog() { open = false; pressed = -1; },
    /** Row centres in CANVAS pixels, from the same hit test the pointer uses. */
    targets(w, h) {
      const scale = Math.min(w / width, h / height);
      const ox = Math.round((w - width * scale) / 2), oy = Math.round((h - height * scale) / 2);
      return Object.fromEntries(rows.map((r) => [r.region,
        [Math.round(ox + (r.x + r.w / 2) * scale), Math.round(oy + (r.y + r.h / 2) * scale)]]));
    },
    hitTest(x, y) { return open ? rowAt(x, y) : -1; },
    hover(x, y) { hot = this.hitTest(x, y); return hot; },
    press(x, y) { pressed = this.hitTest(x, y); hot = pressed; return pressed >= 0 ? `gateway:${pressed}` : null; },
    release(token, x, y) {
      const i = this.hitTest(x, y); pressed = -1;
      if (i >= 0 && token === `gateway:${i}`) { selected = i; open = false; return selected; }
      return -1;
    },
    cancel() { pressed = -1; },
    /** The bar under BATTLE.NET: retail's default until a region is picked (`gatewayBar`). */
    gatewayBar() { return selected >= 0 ? GATEWAY_BAR_PREFIX + servers[selected].region : DEFAULT_GATEWAY_LABEL; },
    /** Paint the dialog; returns the identity evidence for the frame just drawn. */
    draw(ctx) {
      drawn = { heading: null, rows: [] };
      if (!open) return drawn;
      ctx.fillStyle = 'rgba(0,0,0,0.65)';
      ctx.fillRect(0, 0, width, height);
      if (drawHeading) {
        const glyphs = [];
        drawHeading(ctx, 'SELECT GATEWAY', Math.trunc(height / 2) - rows.length * 22 - 28, (g) => glyphs.push(g));
        drawn.heading = { text: glyphs.map((g) => g.character).join(''), glyphs: glyphs.map((g) => ({ character: g.character, index: g.index, path: g.path })) };
      }
      const b = buttons?.wide;
      for (const r of rows) {
        const glyphs = [];
        let cels = [];
        // Retail's own button states: hover and rest are the up cel, only the row the pointer is
        // held on gets the down cel (the front-end press/release rule).
        const cel = b?.[r.index === pressed ? 1 : 0];
        if (cel) { ctx.drawImage(cel.canvas, r.x, r.y); cels = recordButton(cel, r.x, r.y) || []; }
        drawLabel(ctx, r.region, r.x, r.y, r.w, r.h, 0, 0, (g) => glyphs.push(g));
        drawn.rows.push({ region: r.region, host: r.host, x: r.x, y: r.y, w: r.w, h: r.h,
          button: cels.map((c) => ({ path: c.path, frame: c.frame, x: c.x, y: c.y, hash: c.hash })),
          text: glyphs.map((g) => g.character).join(''),
          glyphs: glyphs.map((g) => ({ character: g.character, index: g.index, path: g.path })) });
      }
      return drawn;
    },
  };
}
