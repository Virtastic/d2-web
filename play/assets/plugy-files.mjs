// Where PlugY's own files come from. PlugY keeps its stash art (StashBtns, SharedGoldBtns, TradeStash .dc6) and
// PlugY.ini in a folder beside the game (D2SE: <D2>/D2SE/PlugY/ and <D2>/D2SE/PlugY.ini), never in an MPQ.
// A hosted dev page serves them under /retail/ (ci/stage.sh, play/serve.mjs); a bring-your-own-files host serves
// nothing, so the door keeps the copies the player gave with their install folder (byo-source.mjs) and the shell
// reads those. The site never hosts them.
import { isDc6 } from './plugy-stash-ui.mjs';

export const PLUGY_ART = ['StashBtns', 'SharedGoldBtns', 'TradeStash'];
export const PLUGY_FILES = [...PLUGY_ART.map((n) => n + '.dc6'), 'PlugY.ini'];
const LOWER = PLUGY_FILES.map((n) => n.toLowerCase());
/** True for a file the PlugY stash reads (by its base name, any case). */
export const isPlugyFile = (name) => LOWER.includes(String(name).split(/[\\/]/).pop().toLowerCase());
/** The PlugY files among `files` ({ name } each), keyed by lower-case base name; the last one given wins. */
export function plugyFilesOf(files) {
  const out = {};
  for (const f of files || []) if (f && isPlugyFile(f.name)) out[String(f.name).split(/[\\/]/).pop().toLowerCase()] = f;
  return out;
}

/**
 * Reads PlugY.ini and the art. byo: from `given` (Blobs keyed by lower-case name, plugyFilesOf); otherwise from the
 * host under `roots` with `fetch`. ini() resolves to the text of a PlugY.ini with a [STASH] section, or null.
 * art(name) resolves to the DC6 bytes, or rejects with "<name>.dc6 not served|not given".
 */
export function plugySource({ byo = false, given = null, fetch: get = globalThis.fetch, roots = ['/retail/D2SE/', '/retail/'] } = {}) {
  const stash = (t) => (typeof t === 'string' && /\[STASH\]/i.test(t) ? t : null);
  if (byo) {
    const blob = (n) => (given || {})[n.toLowerCase()];
    return {
      async ini() { const b = blob('PlugY.ini'); return b ? stash(await b.text()) : null; },
      async art(n) {
        const b = blob(n + '.dc6'), bytes = b && new Uint8Array(await b.arrayBuffer());
        if (isDc6(bytes)) return bytes;
        throw new Error(n + '.dc6 not given (choose the Diablo II folder that holds D2SE/PlugY)');
      },
    };
  }
  return {
    async ini() {
      for (const r of roots) { try { const t = await get(r + 'PlugY.ini'), s = t.ok ? stash(await t.text()) : null; if (s) return s; } catch {} }
      return null;
    },
    async art(n) {
      for (const r of roots) { const res = await get(r + 'PlugY/' + n + '.dc6'); const b = new Uint8Array(await res.arrayBuffer()); if (res.ok && isDc6(b)) return b; }
      throw new Error(n + '.dc6 not served');
    },
  };
}
