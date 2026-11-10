// Retail's "Connecting to Battle.net" popup (1.13c D2Launch+0xFC40), drawn over the main menu while
// the client dials the committed region. D2Launch+0x19290 opens it (through +0xEE10) when Bnclient is
// not connected yet; its modal loop's idle (+0xF950) refreshes the status (+0xF800) every frame until
// Bnclient reports the connection, and only then is the login screen built.
//
// The five controls are retail's descriptors (D2Launch+0x23980..0x23A40), D2Win's (type, x, bottom
// row, width, height, inset x, inset y):
//   - the PopUp_340x224 frame, two cels, at (222, 426);
//   - CANCEL (string 5103), a MediumButtonBlank at (330, 416);
//   - the title, a centred Font30 text box (222, 280, 340x60) holding 5171;
//   - the status, a centred Font24 text box (222, 360, 340x70, inset 10/8): Bnclient+0x80's code 0..6
//     names strings 5171..5177, any other code leaves the box empty;
//   - the dots, a centred FontFormal12 colour-4 box (222, 370, 340x20, inset 10/4): "%.*s" of twenty
//     dots, ((GetTickCount - opened) / 1000) % 20 + 1 of them.
// A D2Win text box draws its first line one font line height below its top, inset included.
import { createBlender, createFont, decodeFontTbl, textColourPalette, LABEL_TINT, FRONTEND } from './frontend.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';

const P = 'data\\global\\ui\\FrontEnd\\';
const F = 'data\\local\\font\\latin\\';
export const BNET_CONNECTING_ASSETS = {
  popup: P + 'PopUp_340x224.dc6', button: P + 'MediumButtonBlank.dc6',
  titleFont: F + 'font30.dc6', titleFontTbl: F + 'font30.tbl',
  statusFont: F + 'font24.dc6', statusFontTbl: F + 'font24.tbl',
  dotsFont: F + 'fontformal12.dc6', dotsFontTbl: F + 'fontformal12.tbl',
};
export const BNET_CONNECTING_STRINGS = { title: 5171, status: [5171, 5172, 5173, 5174, 5175, 5176, 5177], cancel: 5103 };
export const BNET_CONNECTING_LAYOUT = {
  popup: { x: 222, anchorY: 426 },
  cancel: { x: 330, anchorY: 416, w: 128, h: 35 },
  title: { x: 222, bottom: 280, w: 340, h: 60, inset: 0, insetY: 0, step: 23 },
  status: { x: 222, bottom: 360, w: 340, h: 70, inset: 10, insetY: 8, step: 19 },
  dots: { x: 222, bottom: 370, w: 340, h: 20, inset: 10, insetY: 4, colour: 4 },
};

// D2Win's text box wrap: the longest run of space-separated words that fits the width.
const wrap = (text, width, measure) => {
  const lines = [];
  let line = null;
  for (const word of String(text).split(' ')) {
    const next = line === null ? word : `${line} ${word}`;
    if (line !== null && measure(next) > width) { lines.push(line); line = word; } else line = next;
  }
  if (line) lines.push(line);
  return lines;
};

export function createBnetConnectingPopup({ read, decodeDc6, decodeDatPalette, string = () => '', now = () => performance.now(),
  onCancel = () => {}, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const skyPalette = decodeDatPalette(read(FRONTEND.backgroundPalette));
  const skyPl2 = read('data\\global\\palette\\Sky\\pal.pl2');
  const frameCanvas = (f) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    const rgba = new Uint8ClampedArray(f.width * f.height * 4);
    for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(skyPalette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
    c.getContext('2d').putImageData(new ImageData(rgba, c.width, c.height), 0, 0);
    return c;
  };
  const cels = (path) => { const dc6 = decodeDc6(read(path)); return { path, dc6, canvases: dc6.frames.map(frameCanvas) }; };
  const A = BNET_CONNECTING_ASSETS, L = BNET_CONNECTING_LAYOUT, S = BNET_CONNECTING_STRINGS;
  const popup = cels(A.popup), button = cels(A.button);
  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const labelFont = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: tinted, documentObject,
    path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend: createBlender(skyPl2, palette) } });
  const boxFont = (dc6, tbl, colour) => createFont({ dc6: decodeDc6(read(dc6)), tbl: decodeFontTbl(read(tbl)),
    palette: colour ? textColourPalette(skyPl2, skyPalette, colour) : skyPalette, documentObject, path: dc6, drawAs: { op: 'celcolor', mode: 5, extra: colour } });
  const titleFont = boxFont(A.titleFont, A.titleFontTbl, 0);
  const statusFont = boxFont(A.statusFont, A.statusFontTbl, 0);
  const dotsFont = boxFont(A.dotsFont, A.dotsFontTbl, L.dots.colour);

  // D2Launch+0xFC40 takes GetTickCount for the dots and starts on code 0's text.
  let opened = now(), code = 0, pressed = false;

  const drawCel = (ctx, c, index, x, anchorY) => {
    const f = c.dc6.frames[index]; if (!f) return 0;
    const left = x + f.offsetX, top = anchorY + f.offsetY - f.height + 1;
    ctx.drawImage(c.canvases[index], left, top);
    recordDc6(c.path, index, f, left, top, { op: 'cel', mode: 5 });
    return f.width;
  };
  const drawBox = (ctx, font, text, box) => {
    const x = box.x + box.inset, w = box.w - 2 * box.inset, top = box.bottom - box.h + box.insetY;
    for (const [i, line] of wrap(text, w, (t) => font.measure(t)).entries())
      font.drawAnchored(ctx, line, x + Math.max(Math.trunc((w - font.measure(line)) / 2), 0), top + font.lineHeight + i * (box.step ?? 0));
  };
  const statusText = () => {
    return code >= 0 && code < S.status.length ? string(S.status[code]) || '' : '';
  };
  const inCancel = (sx, sy) => { const c = L.cancel; return sx >= c.x && sx < c.x + c.w && sy > c.anchorY - c.h && sy <= c.anchorY; };

  return {
    get evidence() { return { code, status: statusText(), dots: this.dots() }; },
    // Bnclient+0x80's connection stage, as D2Launch+0xF800 reads it each frame.
    setStatus(next) { code = next; },
    clock(ms) { opened = 0; now = () => ms; },
    dots() { return (Math.trunc(Math.max(0, now() - opened) / 1000) % 20) + 1; },
    draw(ctx) {
      let x = L.popup.x;
      for (const i of [0, 1]) x += drawCel(ctx, popup, i, x, L.popup.anchorY);
      const c = L.cancel, state = pressed ? 1 : 0;
      drawCel(ctx, button, state, c.x, c.anchorY);
      const label = string(S.cancel) || '';
      labelFont.drawAnchored(ctx, label, c.x + (pressed ? -2 : 0) + Math.max(Math.trunc((c.w - labelFont.measure(label)) / 2), 0),
        c.anchorY + (pressed ? 2 : 0) + 4 - Math.trunc((c.h - labelFont.lineHeight) / 2));
      drawBox(ctx, titleFont, string(S.title) || '', L.title);
      drawBox(ctx, statusFont, statusText(), L.status);
      drawBox(ctx, dotsFont, '.'.repeat(this.dots()), L.dots);
    },
    // Scene coordinates (800x600). The popup is modal: only CANCEL answers.
    press(sx, sy) { pressed = inCancel(sx, sy); return pressed ? 'cancel' : null; },
    release(token, sx, sy) { const hit = pressed && token === 'cancel' && inCancel(sx, sy); pressed = false; if (hit) onCancel(); return hit; },
    cancel() { pressed = false; },
    key(code) { if (code === 'Escape') { onCancel(); return true; } return false; },
    targets() { const c = L.cancel; return { CANCEL: [c.x + c.w / 2, c.anchorY - c.h / 2] }; },
  };
}

// D2Launch+0x15090's notice, raised over the menu when the connection fails (D2Launch+0xE510 event 0 with string 5189): the
// PopUpOK frame at (268, 350), OK a CancelButtonBlank at (351, 337) labelled in FontRidiculous, the text a centred Font24 box
// at x=268 264 wide whose top is row 200, its lines starting 8 rows lower and stepping 19. Its OK runs `onOk`.
export const BNET_CONNECT_NOTICE_ASSETS = {
  popup: P + 'PopUpOK.dc6', button: P + 'CancelButtonBlank.dc6',
  bodyFont: F + 'font24.dc6', bodyFontTbl: F + 'font24.tbl',
  labelFont: F + 'fontridiculous.dc6', labelFontTbl: F + 'fontridiculous.tbl',
};
export const BNET_CONNECT_NOTICE_LAYOUT = {
  x: 268, anchorY: 350, ok: { x: 351, anchorY: 337, w: 96, h: 32, labelDy: -9, label: 5102 },
  body: { x: 268, width: 264, top: 200, inset: 10, insetY: 8, step: 19 },
};

export function createBnetConnectNotice({ read, decodeDc6, decodeDatPalette, string = () => '', text = '', onOk = () => {}, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const skyPalette = decodeDatPalette(read(FRONTEND.backgroundPalette));
  const skyPl2 = read('data\\global\\palette\\Sky\\pal.pl2');
  const frameCanvas = (f) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    const rgba = new Uint8ClampedArray(f.width * f.height * 4);
    for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(skyPalette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
    c.getContext('2d').putImageData(new ImageData(rgba, c.width, c.height), 0, 0);
    return c;
  };
  const cels = (path) => { const dc6 = decodeDc6(read(path)); return { path, dc6, canvases: dc6.frames.map(frameCanvas) }; };
  const A = BNET_CONNECT_NOTICE_ASSETS, N = BNET_CONNECT_NOTICE_LAYOUT;
  const frame = cels(A.popup), button = cels(A.button);
  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const labelFont = createFont({ dc6: decodeDc6(read(A.labelFont)), tbl: decodeFontTbl(read(A.labelFontTbl)), palette: tinted, documentObject,
    path: A.labelFont, drawAs: { op: 'celcolor', mode: 4, blend: createBlender(skyPl2, palette) } });
  const bodyFont = createFont({ dc6: decodeDc6(read(A.bodyFont)), tbl: decodeFontTbl(read(A.bodyFontTbl)), palette: skyPalette, documentObject,
    path: A.bodyFont, drawAs: { op: 'celcolor', mode: 5, extra: 0 } });
  let pressed = false;
  const inOk = (sx, sy) => { const o = N.ok; return sx >= o.x && sx < o.x + o.w && sy > o.anchorY - o.h && sy <= o.anchorY; };
  const drawCel = (ctx, c, index, x, anchorY) => {
    const f = c.dc6.frames[index]; if (!f) return 0;
    const left = x + f.offsetX, top = anchorY + f.offsetY - f.height + 1;
    ctx.drawImage(c.canvases[index], left, top);
    recordDc6(c.path, index, f, left, top, { op: 'cel', mode: 5 });
    return f.width;
  };
  return {
    get evidence() { return { text, pressed }; },
    draw(ctx) {
      let x = N.x;
      for (const i of [0, 1]) x += drawCel(ctx, frame, i, x, N.anchorY);
      drawCel(ctx, button, pressed ? 1 : 0, N.ok.x, N.ok.anchorY);
      const label = string(N.ok.label) || '';
      labelFont.drawAnchored(ctx, label, N.ok.x + Math.max(Math.trunc((N.ok.w - labelFont.measure(label)) / 2), 0), N.ok.anchorY + N.ok.labelDy + 4 - Math.trunc((35 - labelFont.lineHeight) / 2));
      const b = N.body, bw = b.width - 2 * b.inset;
      for (const [i, line] of wrap(text, bw, (t) => bodyFont.measure(t)).entries())
        bodyFont.drawAnchored(ctx, line, b.x + b.inset + Math.max(Math.trunc((bw - bodyFont.measure(line)) / 2), 0), b.top + b.insetY + bodyFont.lineHeight + i * b.step);
    },
    press(sx, sy) { pressed = inOk(sx, sy); return pressed ? 'ok' : null; },
    release(token, sx, sy) { const hit = pressed && token === 'ok' && inOk(sx, sy); pressed = false; if (hit) onOk(); return hit; },
    cancel() { pressed = false; },
    key(code) { if (code === 'Enter' || code === 'NumpadEnter' || code === 'Escape') { onOk(); return true; } return false; },
    targets() { const o = N.ok; return { OK: [o.x + o.w / 2, o.anchorY - o.h / 2] }; },
  };
}

// The status-2 failure box (D2Launch+0xE510 default row: title 5248, body 5189, small print 5190), raised over the main menu when the
// dial fails. It is the large popup, not the small PopUpOK: D2Gfx dims the surface, four PopUpLargest cels, a MediumButtonBlank OK,
// a centred Font30 title, the Font24 body on one line and the Font16 small print in colour 4 wrapped to its own box. The geometry is
// the capture the login screen's same box was measured from (vectors/drawlist/bnet-connect-fail.txt). Its OK runs `onOk`.
export const BNET_CONNECT_FAILURE_LAYOUT = {
  cels: [[0, 158, 427], [1, 414, 427], [2, 158, 460], [3, 414, 460]], ok: { x: 335, anchorY: 450, w: 128, h: 35, label: 5102 },
  title: { baseline: 199, width: 457 }, line: { x: 172, baseline: 235 }, small: { x: 172, width: 456, baseline: 330, step: 14 },
};
export function createBnetConnectFailureNotice({ read, decodeDc6, decodeDatPalette, string = () => '', title = 5248, body = 5189, detail = 5190,
  onOk = () => {}, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const skyPalette = decodeDatPalette(read(FRONTEND.backgroundPalette));
  const skyPl2 = read('data\\global\\palette\\Sky\\pal.pl2');
  const frameCanvas = (f) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    const rgba = new Uint8ClampedArray(f.width * f.height * 4);
    for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(skyPalette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
    c.getContext('2d').putImageData(new ImageData(rgba, c.width, c.height), 0, 0);
    return c;
  };
  const cels = (path) => { const dc6 = decodeDc6(read(path)); return { path, dc6, canvases: dc6.frames.map(frameCanvas) }; };
  const L = BNET_CONNECT_FAILURE_LAYOUT;
  const frame = cels(P + 'PopUpLargest.dc6'), button = cels(P + 'MediumButtonBlank.dc6');
  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const font = (dc6, tbl, pal, extra) => createFont({ dc6: decodeDc6(read(F + dc6)), tbl: decodeFontTbl(read(F + tbl)), palette: pal, documentObject,
    path: F + dc6, drawAs: { op: 'celcolor', mode: 5, extra } });
  const titleFont = font('font30.dc6', 'font30.tbl', skyPalette, 0), bodyFont = font('font24.dc6', 'font24.tbl', skyPalette, 0);
  const smallFont = createFont({ dc6: decodeDc6(read(FRONTEND.versionFont)), tbl: decodeFontTbl(read(FRONTEND.versionFontTbl)),
    palette: textColourPalette(skyPl2, skyPalette, 4), documentObject, path: FRONTEND.versionFont, drawAs: { op: 'celcolor', mode: 5, extra: 4 } });
  const labelFont = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: tinted, documentObject,
    path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend: createBlender(skyPl2, palette) } });
  const text = { title: string(title) || '', body: string(body) || '', detail: string(detail) || '' };
  let pressed = false;
  const inOk = (sx, sy) => { const o = L.ok; return sx >= o.x && sx < o.x + o.w && sy > o.anchorY - o.h && sy <= o.anchorY; };
  const drawCel = (ctx, c, index, x, anchorY) => {
    const f = c.dc6.frames[index]; if (!f) return 0;
    const left = x + f.offsetX, top = anchorY + f.offsetY - f.height + 1;
    ctx.drawImage(c.canvases[index], left, top);
    recordDc6(c.path, index, f, left, top, { op: 'cel', mode: 5 });
    return f.width;
  };
  return {
    get evidence() { return { text: [text.title, text.body, text.detail], pressed }; },
    draw(ctx) {
      ctx.save(); ctx.globalAlpha = 0x7f / 0xff; ctx.fillStyle = '#000'; ctx.fillRect(0, 0, 800, 600); ctx.restore();
      recordPrim('boxalpha', 0, 0, 800, 600, 0, 0x7f);
      for (const [i, x, y] of L.cels) drawCel(ctx, frame, i, x, y);
      drawCel(ctx, button, pressed ? 1 : 0, L.ok.x, L.ok.anchorY);
      const nudge = pressed ? 2 : 0, label = string(L.ok.label) || '';
      labelFont.drawAnchored(ctx, label, L.ok.x + Math.max(Math.trunc((L.ok.w - labelFont.measure(label)) / 2), 0) - nudge,
        L.ok.anchorY + 4 - Math.trunc((35 - labelFont.lineHeight) / 2) + nudge);
      titleFont.drawAnchored(ctx, text.title, L.line.x + Math.max(Math.trunc((L.title.width - titleFont.measure(text.title)) / 2), 0), L.title.baseline);
      bodyFont.drawAnchored(ctx, text.body, L.line.x, L.line.baseline);
      const S = L.small;
      for (const [i, line] of wrap(text.detail, S.width, (t) => smallFont.measure(t)).entries()) smallFont.drawAnchored(ctx, line, S.x, S.baseline + i * S.step);
    },
    press(sx, sy) { pressed = inOk(sx, sy); return pressed ? 'ok' : null; },
    release(token, sx, sy) { const hit = pressed && token === 'ok' && inOk(sx, sy); pressed = false; if (hit) onOk(); return hit; },
    cancel() { pressed = false; },
    key(code) { if (code === 'Enter' || code === 'NumpadEnter' || code === 'Escape') { onOk(); return true; } return false; },
    targets() { const o = L.ok; return { OK: [o.x + o.w / 2, o.anchorY - o.h / 2] }; },
  };
}
