// The front door's upload popup: while the player's files are read, unzipped and stored, the door shows the same
// popup the client raises while it dials Battle.net (bnet-connecting.mjs): the PopUp_340x224 frame, a Font30 title,
// a Font24 status and the FontFormal12 dots, at that popup's own layout. The door has no archives yet, so the cels
// come from its door art (door-art-build.mjs) rather than the client's MPQ reads.
//
// byo-source.mjs reports steps; this module turns each into what the popup shows. A step is one of
//   { kind: 'reading' }                      a folder (or a dropped folder) is being walked
//   { kind: 'unzip', name, n, m, entry? }    archive n of m is being unpacked
//   { kind: 'store', name, pct }             a picked file is being copied into this browser's storage
//   { kind: 'process', name, n, m }          file n of m is being inspected
//   { kind: 'done', found, total }           finished: `found` of the `total` game files are in hand
//   { kind: 'error', text }                  failed: the popup stays up until it is dismissed
import { BNET_CONNECTING_LAYOUT } from './bnet-connecting.mjs';

export const UPLOAD_DONE_MS = 1500;   // a finished read lingers this long, then closes on its own

/** What the popup shows for one step: { phase, title, text, busy, ok }. */
export function uploadView(step) {
  switch (step.kind) {
    case 'reading': return { phase: 'reading', title: 'Reading Files', text: 'Reading the folder', busy: true };
    case 'unzip': return { phase: 'unzipping', title: 'Unzipping',
      text: `Unzipping ${step.name}${step.m > 1 ? ` (${step.n} of ${step.m})` : ''}${step.entry ? `: ${step.entry}` : ''}`, busy: true };
    case 'store': return { phase: 'storing', title: 'Storing Files', text: `Storing ${step.name} in this browser: ${step.pct}%`, busy: true };
    case 'process': return { phase: 'processing', title: 'Reading Files', text: `Processing file ${step.n} of ${step.m}: ${step.name}`, busy: true };
    case 'done': return { phase: 'done', title: 'Done', ok: true,
      text: step.found >= step.total ? 'All files found.' : `${step.found} of ${step.total} files found.` };
    case 'error': return { phase: 'error', title: 'Error', text: step.text || 'These files could not be read.', ok: true };
    default: throw new Error(`upload step ${step.kind}`);
  }
}

/** The popup's state machine. `view` is null while closed; `log` keeps every view shown, in order. */
export function createUploadModal({ onChange = () => {} } = {}) {
  let view = null;
  const log = [];
  const set = (v) => { view = v; if (v) log.push(v); onChange(v); };
  return {
    get view() { return view; },
    get open() { return !!view; },
    log,
    step: (s) => set(uploadView(s)),
    /** OK, Enter or Escape: closes a finished or failed popup; a running one ignores it. */
    dismiss() { if (view && !view.busy) { set(null); return true; } return false; },
    close() { if (view) set(null); },
  };
}

/**
 * One upload, start to finish: `pick(step)` resolves { handles, files } (empty when the player cancelled), `add(src, step)`
 * reads them and resolves { found, total }. Any failure lands on the error view; a cancelled pick shows nothing, and a
 * pick that was read (a folder walked, an archive opened) but held no game file says so.
 */
export async function runUpload(modal, pick, add) {
  try {
    const src = await pick(modal.step);
    if (!src || (!src.handles?.length && !src.files?.length)) {
      if (modal.open) modal.step({ kind: 'error', text: NOTHING });
      return null;
    }
    const r = await add(src, modal.step);
    modal.step({ kind: 'done', ...r });
    return r;
  } catch (e) {
    modal.step({ kind: 'error', text: uploadErrorText(e) });
    return null;
  }
}
const NOTHING = 'No Diablo II files were found in what you chose.';
export function uploadErrorText(e) {
  if (e?.name === 'NotAllowedError' || e?.name === 'SecurityError') return 'The browser did not allow reading these files.';
  if (e?.name === 'NotReadableError' || e?.name === 'NotFoundError') return 'A file could not be read. Choose it again.';
  if (e?.name === 'QuotaExceededError') return 'This browser has no room left to store the files.';
  return 'These files could not be read.';
}

// ---- drawing, in the door's 800x600 frame. `art` is the door art; `font(name, colour, text, x, top)` draws a door font ----
// The connecting popup sits 8px left of the 800-wide frame's middle; the upload popup is centred (340 wide -> x 230).
const DX = (800 - 340) / 2 - BNET_CONNECTING_LAYOUT.popup.x;
const shift = (b) => ({ ...b, x: b.x + DX });
const B = BNET_CONNECTING_LAYOUT;
const L = { popup: shift(B.popup), title: shift(B.title), status: shift(B.status), dots: shift(B.dots) };
const OK = { x: (800 - 128) / 2, anchorY: 416, w: 128, h: 35 };   // MediumButtonBlank in the connecting popup's CANCEL row, centred
const wrap = (text, width, measure) => {   // D2Win's text box wrap, as bnet-connecting.mjs does it
  const lines = []; let line = null;
  for (const word of String(text).split(' ')) {
    const next = line === null ? word : `${line} ${word}`;
    if (line !== null && measure(next) > width) { lines.push(line); line = word; } else line = next;
  }
  if (line) lines.push(line);
  return lines;
};

export const uploadOkHit = (view, p) => !!view?.ok && p.x >= OK.x && p.x < OK.x + OK.w && p.y > OK.anchorY - OK.h && p.y <= OK.anchorY;

export function drawUploadModal(g, view, { art, measure, text, pressed = false, dots = 1 }) {
  if (!view) return;
  g.save(); g.fillStyle = 'rgba(0,0,0,0.5)'; g.fillRect(0, 0, 800, 600); g.restore();   // the door behind the popup takes no input
  const cel = (img, frames, i, x, anchorY) => {
    const fr = frames?.[i]; if (!fr) return 0;
    const [sx, w, h, ox, oy] = fr;
    g.drawImage(img, sx, 0, w, h, x + ox, anchorY + oy - h + 1, w, h);
    return w;
  };
  if (art?.img.popup) { let x = L.popup.x; for (const i of [0, 1]) x += cel(art.img.popup, art.meta.popup, i, x, L.popup.anchorY); }
  else { g.fillStyle = '#0b0a08'; g.fillRect(230, 202, 340, 224); g.strokeStyle = '#5a4a2a'; g.lineWidth = 2; g.strokeRect(231, 203, 338, 222); }
  const box = (font, colour, s, b) => {
    const lh = art?.meta.fonts[font]?.ch ?? 0, x = b.x + b.inset, w = b.w - 2 * b.inset, top = b.bottom - b.h + b.insetY;
    for (const [i, line] of wrap(s, w, (t) => measure(font, t)).entries())
      text(font, colour, line, x + Math.max(Math.trunc((w - measure(font, line)) / 2), 0), top + i * (b.step ?? lh));
  };
  box('font30', 'white', view.title, L.title);
  box('font24', 'white', view.text, L.status);
  if (view.busy) box('fontformal12', 'gold', '.'.repeat(dots), L.dots);
  if (view.ok) {
    if (art?.img.medium) cel(art.img.medium, art.meta.medium, pressed ? 1 : 0, OK.x, OK.anchorY);
    else { g.fillStyle = pressed ? '#100d08' : '#1d1810'; g.fillRect(OK.x, OK.anchorY - OK.h + 1, OK.w, OK.h); }
    const d = pressed ? 2 : 0;
    text('fontexocet10', 'white', 'OK', OK.x + (OK.w - measure('fontexocet10', 'OK')) / 2 - d, OK.anchorY - OK.h + 10 + d, 'multiply');
  }
}
