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
  // `host` is the realm's TCP endpoint (region identity / what a native client dials); `ws` is the
  // SAME realm's browser-reachable WebSocket endpoint (through `tools/realmd/ws-bridge.mjs`), which
  // is what the page can actually open -- a browser has no raw TCP. A region is usable by the page
  // only when it carries `ws`; without it the region still lists, but selecting it dials nothing.
  const servers = raw
    .map((s) => ({ region: String(s?.region ?? s?.name ?? '').trim(), host: String(s?.host ?? s?.address ?? '').trim(),
      ws: String(s?.ws ?? '').trim() }))
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

/** The measured retail 1.13c gateway screen (`the retail captures`, F233). */
export const GATEWAY_LAYOUT = Object.freeze({
  panel: { x: 237, y: 217, w: 340, h: 339, path: 'data\\global\\ui\\FrontEnd\\GWListBox.dc6',
    tiles: [{ frame: 0, x: 0, y: 0 }, { frame: 1, x: 256, y: 0 }, { frame: 2, x: 0, y: 255 }, { frame: 3, x: 256, y: 255 }] },
  heading: { text: 'SELECT GATEWAY', font: 'data\\local\\font\\latin\\font24.dc6', x: 302, y: 250, shift: 4 },
  body: { font: 'data\\local\\font\\latin\\font8.dc6', x: 257, shift: 4,
    lines: [{ text: 'If the current gateway does not best represent', y: 275 },
      { text: 'your location, select another Battle.net gateway', y: 293 },
      { text: 'connection.', y: 311 }] },
  rows: { x: 253, textX: 257, firstAnchorY: 360, pitch: 24, hitTop: 338, w: 292, h: 24, shift: 4,
    selectedShift: 0, selectedColor: '#404040', font: 'data\\local\\font\\latin\\fontformal12.dc6' },
  button: { path: 'data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6', w: 96, h: 32,
    y: 507, anchorY: 529, labelFont: 'data\\local\\font\\latin\\fontridiculous.dc6',
    ok: { x: 281, label: 'OK' }, cancel: { x: 436, label: 'CANCEL' } },
});

/** Retail's listbox places one configured region per 24px row, in the measured panel. */
export function gatewayRows(servers) {
  return servers.map((s, i) => ({ region: s.region, host: s.host, index: i,
    x: GATEWAY_LAYOUT.rows.x, y: GATEWAY_LAYOUT.rows.hitTop + i * GATEWAY_LAYOUT.rows.pitch,
    w: GATEWAY_LAYOUT.rows.w, h: GATEWAY_LAYOUT.rows.pitch, anchorY: GATEWAY_LAYOUT.rows.firstAnchorY + i * GATEWAY_LAYOUT.rows.pitch }));
}

/** A repo-owned server list presented in retail 1.13c's measured gateway selection screen. */
export function createGatewaySelector({ servers, screen, width = 800, height = 600, onCommit = null }) {
  const rows = gatewayRows(servers);
  const defaultIndex = Math.max(0, servers.findIndex((s) => s.region.toUpperCase() === 'U.S. EAST'));
  let open = false, committed = servers.length ? defaultIndex : -1, choice = committed, hot = null, pressed = null;
  let drawn = { panel: [], heading: null, body: [], rows: [], selection: null, buttons: [] };
  const rowAt = (x, y) => rows.findIndex((r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h);
  const buttonAt = (x, y) => Object.entries(GATEWAY_LAYOUT.button).filter(([k]) => k === 'ok' || k === 'cancel')
    .find(([k, b]) => x >= b.x && x < b.x + GATEWAY_LAYOUT.button.w && y >= GATEWAY_LAYOUT.button.y && y < GATEWAY_LAYOUT.button.y + GATEWAY_LAYOUT.button.h)?.[0] ?? null;
  const selectedServer = () => committed >= 0 ? { ...servers[committed] } : null;
  return {
    get open() { return open; },
    get selected() { return selectedServer(); },
    get evidence() {
      return { enabled: true, configured: servers.map((s) => ({ region: s.region, host: s.host })),
        rows: rows.map((r) => ({ region: r.region, host: r.host, x: r.x, y: r.y, w: r.w, h: r.h, anchorY: r.anchorY })),
        open, hot, selected: selectedServer(), highlighted: choice >= 0 ? servers[choice].region : null, drawn };
    },
    openDialog() { if (!servers.length) return false; open = true; choice = committed; hot = null; pressed = null; return true; },
    closeDialog() { open = false; choice = committed; pressed = null; },
    targets(w, h) {
      const scale = Math.min(w / width, h / height);
      const ox = Math.round((w - width * scale) / 2), oy = Math.round((h - height * scale) / 2);
      return Object.fromEntries([
        ...rows.map((r) => [r.region, [Math.round(ox + (r.x + r.w / 2) * scale), Math.round(oy + (r.y + r.h / 2) * scale)]]),
        ...['ok', 'cancel'].map((k) => { const b = GATEWAY_LAYOUT.button[k]; return [k.toUpperCase(), [Math.round(ox + (b.x + GATEWAY_LAYOUT.button.w / 2) * scale), Math.round(oy + (GATEWAY_LAYOUT.button.y + GATEWAY_LAYOUT.button.h / 2) * scale)]]; }),
      ]);
    },
    hitTest(x, y) { return open ? { row: rowAt(x, y), button: buttonAt(x, y) } : { row: -1, button: null }; },
    hover(x, y) {
      const h = this.hitTest(x, y); hot = h.row >= 0 ? servers[h.row].region : h.button?.toUpperCase() ?? null; return hot;
    },
    press(x, y) {
      const h = this.hitTest(x, y); pressed = h.row >= 0 ? `row:${h.row}` : h.button; hot = h.row >= 0 ? servers[h.row].region : h.button?.toUpperCase() ?? null;
      return pressed ? `gateway:${pressed}` : null;
    },
    release(token, x, y) {
      const h = this.hitTest(x, y), target = h.row >= 0 ? `row:${h.row}` : h.button, wasPressed = pressed;
      pressed = null;
      if (!target || token !== `gateway:${target}` || wasPressed !== target) return -1;
      if (h.row >= 0) { choice = h.row; return choice; }
      if (h.button === 'ok') { committed = choice; open = false; if (onCommit && committed >= 0) onCommit({ ...servers[committed] }); return committed; }
      if (h.button === 'cancel') { choice = committed; open = false; return -1; }
      return -1;
    },
    cancel() { pressed = null; },
    gatewayBar() { return committed >= 0 ? GATEWAY_BAR_PREFIX + servers[committed].region.toUpperCase() : DEFAULT_GATEWAY_LABEL; },
    draw(ctx) {
      drawn = { panel: [], heading: null, body: [], rows: [], selection: null, buttons: [] };
      if (!open) return drawn;
      // GWListBox.dc6's four retail frame tiles; the capture's panel is x=237..576, y=217..555.
      drawn.panel = screen.drawPanel(ctx);
      drawn.heading = screen.drawHeading(ctx);
      drawn.body = screen.drawBody(ctx);
      if (choice >= 0) drawn.selection = screen.drawSelection(ctx, rows[choice]);
      for (const row of rows) drawn.rows.push({ ...screen.drawRegion(ctx, row, row.index === choice), region: row.region, host: row.host });
      for (const key of ['ok', 'cancel']) drawn.buttons.push(screen.drawButton(ctx, key, pressed === key));
      return drawn;
    },
  };
}
