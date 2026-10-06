// F89: the title's OTHER MULTIPLAYER control, and the TCP/IP Options screen behind it. Retail's
// front end has both; before this module the title's OTHER MULTIPLAYER was `action: () => {}` and a
// click did nothing. Nothing here is a stand-in: the background, the buttons, the fonts, the anchors and the
// labels are read off retail's own draw lists (the retail captures,
// f89-tcpip-options.txt) and the same Sky/Units palettes the title uses.
//
// What is OUT of this module, and named here so the gap is not silent: the host/join TRANSPORT.
// HOST GAME advances to character select, as retail's capture shows; JOIN GAME would open a host-IP
// modal and dial a server, which is the realm backend's work, so JOIN is a
// recorded gap and the join modal/failure screens (f89-tcpip-join-dialog.txt / -join-failure.txt)
// are not built yet.
import { composeTiled, createBlender, createFont, decodeFontTbl, recordTiled, textColourPalette,
  LABEL_TINT, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';

const P = 'data\\global\\ui\\FrontEnd\\';
export const OTHER_ASSETS = {
  menuBackground: FRONTEND.background,           // gameselectscreenEXP.dc6, the same art as the title
  tcpipBackground: P + 'tcpipscreen.dc6',
  button: FRONTEND.button,                        // WideButtonBlank.dc6 (body + 16px tail)
  mediumButton: P + 'MediumButtonBlank.dc6',      // the TCP/IP screen's CANCEL
  headingFont: 'data\\local\\font\\latin\\font42.dc6',
  headingFontTbl: 'data\\local\\font\\latin\\font42.tbl',
};

// Where retail draws each control on the 800x600 screen. A button cel's anchor is its bottom row, so
// its top is `anchorY - (h - 1)`; a label's anchor is its own y in the draw list.
export const OTHER_LAYOUT = {
  menu: {
    x: 264, tailX: 520, width: 272, height: 35,
    buttons: [
      { id: 'bnet', label: 'OPEN BATTLE.NET', anchorY: 310 },
      { id: 'tcpip', label: 'TCP/IP GAME', anchorY: 350 },
      { id: 'cancel', label: 'CANCEL', anchorY: 568 },
    ],
  },
  tcpip: {
    x: 265, tailX: 521, width: 272, height: 35,
    buttons: [
      { id: 'host', label: 'HOST GAME', anchorY: 206 },
      { id: 'join', label: 'JOIN GAME', anchorY: 264 },
    ],
    cancel: { x: 39, anchorY: 571, width: 128, height: 35, label: 'CANCEL' },
    heading: { x: 259, y: 63, colour: 4 },        // Font42 "TCP/IP Options"
    ipLabel: { x: 325, y: 126 },                   // Font16 "Your IP Address is: "
    ipValue: { x: 325, y: 141 },                   // Font16, the client's own address
  },
};

export function createOtherMultiplayerScreen({ read, decodeDc6, decodeDatPalette, onExit = () => {},
  onHost = () => {}, onJoin = () => {}, ip = '', string = () => '', documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));            // Units: buttons and labels
  const skyPalette = decodeDatPalette(read(FRONTEND.backgroundPalette)); // Sky: the whole frame
  const skyPl2 = read('data\\global\\palette\\Sky\\pal.pl2');
  const frameCanvas = (f) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    const rgba = new Uint8ClampedArray(f.width * f.height * 4);
    for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(skyPalette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
    c.getContext('2d').putImageData(new ImageData(rgba, c.width, c.height), 0, 0);
    return c;
  };
  // A two-part wide button (256px body + 16px tail) per state; a medium button is a single 128px cel.
  const buttonParts = (dc6, framesPerState) => [0, 1].map((state) => {
    const parts = dc6.frames.slice(state * framesPerState, (state + 1) * framesPerState);
    const width = parts.reduce((n, f) => n + f.width, 0);
    const height = Math.max(...parts.map((f) => f.height), 0);
    const canvas = documentObject.createElement('canvas'); canvas.width = width; canvas.height = height;
    const cctx = canvas.getContext('2d'); let x = 0;
    for (const f of parts) { cctx.drawImage(frameCanvas(f), x, 0); x += f.width; }
    return { canvas, width, height, path: null, parts: parts.map((f, k) => ({ frame: f, index: state * framesPerState + k })) };
  });
  const wideDc6 = decodeDc6(read(OTHER_ASSETS.button));
  const wide = buttonParts(wideDc6, 2).map((b) => ({ ...b, path: OTHER_ASSETS.button }));
  const mediumDc6 = decodeDc6(read(OTHER_ASSETS.mediumButton));
  const medium = buttonParts(mediumDc6, 1).map((b) => ({ ...b, path: OTHER_ASSETS.mediumButton }));

  const menuBg = { ...composeTiled(decodeDc6(read(OTHER_ASSETS.menuBackground)), skyPalette, TITLE_COLUMNS, documentObject), path: OTHER_ASSETS.menuBackground };
  const tcpipBg = { ...composeTiled(decodeDc6(read(OTHER_ASSETS.tcpipBackground)), skyPalette, TITLE_COLUMNS, documentObject), path: OTHER_ASSETS.tcpipBackground };
  const logos = [FRONTEND.logoBlack[0], FRONTEND.logoFire[0], FRONTEND.logoBlack[1], FRONTEND.logoFire[1]].map((path) => {
    const dc6 = decodeDc6(read(path));
    return { path, dc6, canvases: dc6.frames.map(frameCanvas) };
  });

  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const blend = createBlender(skyPl2, palette);
  const logoBlend = createBlender(skyPl2, skyPalette);
  const labelDraw = { op: 'celcolor', mode: 4, blend };
  const labelFont = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: tinted, documentObject, path: FRONTEND.font, drawAs: labelDraw });
  const headingFont = createFont({ dc6: decodeDc6(read(OTHER_ASSETS.headingFont)), tbl: decodeFontTbl(read(OTHER_ASSETS.headingFontTbl)),
    palette: textColourPalette(skyPl2, skyPalette, OTHER_LAYOUT.tcpip.heading.colour), documentObject,
    path: OTHER_ASSETS.headingFont, drawAs: { op: 'celcolor', mode: 5, extra: OTHER_LAYOUT.tcpip.heading.colour } });
  const bodyFont = createFont({ dc6: decodeDc6(read(FRONTEND.versionFont)), tbl: decodeFontTbl(read(FRONTEND.versionFontTbl)),
    palette: textColourPalette(skyPl2, skyPalette, 4), documentObject, path: FRONTEND.versionFont, drawAs: { op: 'celcolor', mode: 5, extra: 4 } });
  const cursorDc6 = decodeDc6(read(FRONTEND.cursor));

  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sctx = scene.getContext('2d', { willReadFrequently: true });
  const view = (w, h) => { const scale = Math.min(w / 800, h / 600); return { scale, ox: Math.round((w - 800 * scale) / 2), oy: Math.round((h - 600 * scale) / 2) }; };

  let sub = 'menu';                 // 'menu' | 'tcpip'
  let pressed = null, pointer = [799, 599], tick = 0;

  // Retail's D2Win button label: exocet10, centred over the button width, anchored from its top.
  const drawButtonLabel = (ctx, text, x, y, w, h) => {
    ctx && labelFont.drawAnchored(ctx, text, x + Math.max(Math.trunc((w - labelFont.measure(text)) / 2), 0), (y + h - 1) + 4 - Math.trunc((h - labelFont.lineHeight) / 2));
  };
  const drawLogos = (ctx) => {
    for (const logo of logos) {
      const n = logo.canvases.length;
      const frameIndex = Math.floor(tick * 25 / 60) % (n - 1);
      const frame = logo.dc6.frames[frameIndex], canvas = logo.canvases[frameIndex];
      if (!frame || !canvas.width) continue;
      const fire = logo.path.toLowerCase().includes('fire');
      const left = 400 + frame.offsetX, top = 120 + frame.offsetY - frame.height + 1;
      if (fire) logoBlend(ctx, frame, left, top, 3); else ctx.drawImage(canvas, left, top);
      recordDc6(logo.path, frameIndex, frame, left, top, { op: 'cel', mode: fire ? 3 : 5 });
    }
  };
  const drawWide = (ctx, b, state, x, y) => { ctx.drawImage(wide[state].canvas, x, y); let cx = x; for (const p of wide[state].parts) { recordDc6(OTHER_ASSETS.button, p.index, p.frame, cx, y); cx += p.frame.width; } };

  // The boxes the pointer hits, on the 800x600 surface, for the current sub-screen.
  const boxes = () => sub === 'menu'
    ? OTHER_LAYOUT.menu.buttons.map((b) => ({ id: b.id, label: b.label, kind: 'wide', x: OTHER_LAYOUT.menu.x, y: b.anchorY - (OTHER_LAYOUT.menu.height - 1), w: OTHER_LAYOUT.menu.width, h: OTHER_LAYOUT.menu.height }))
    : [...OTHER_LAYOUT.tcpip.buttons.map((b) => ({ id: b.id, label: b.label, kind: 'wide', x: OTHER_LAYOUT.tcpip.x, y: b.anchorY - (OTHER_LAYOUT.tcpip.height - 1), w: OTHER_LAYOUT.tcpip.width, h: OTHER_LAYOUT.tcpip.height })),
       { id: 'cancel', label: OTHER_LAYOUT.tcpip.cancel.label, kind: 'medium', x: OTHER_LAYOUT.tcpip.cancel.x, y: OTHER_LAYOUT.tcpip.cancel.anchorY - (OTHER_LAYOUT.tcpip.cancel.height - 1), w: OTHER_LAYOUT.tcpip.cancel.width, h: OTHER_LAYOUT.tcpip.cancel.height }];
  const hit = (x, y, w, h) => { const v = view(w, h), sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale; return boxes().find((b) => sx >= b.x && sx < b.x + b.w && sy >= b.y && sy < b.y + b.h) ?? null; };
  const activate = (b) => {
    if (sub === 'menu') {
      if (b.id === 'tcpip') { sub = 'tcpip'; return; }
      if (b.id === 'cancel') { onExit(); return; }
      onJoin(b.id);                                    // OPEN BATTLE.NET: the gateway/updater path (F231)
      return;
    }
    if (b.id === 'host') { onHost(); return; }
    if (b.id === 'join') { onJoin('join'); return; }
    if (b.id === 'cancel') { sub = 'menu'; return; }   // retail returns to the OTHER MULTIPLAYER menu
  };

  return {
    get sub() { return sub; },
    set sub(s) { sub = s === 'tcpip' ? 'tcpip' : 'menu'; },
    get evidence() { return { sub, ip, boxes: boxes().map(({ id, label, x, y, w, h }) => ({ id, label, x, y, w, h })),
      menu: OTHER_LAYOUT.menu.buttons.map((b) => b.label), tcpip: [...OTHER_LAYOUT.tcpip.buttons.map((b) => b.label), OTHER_LAYOUT.tcpip.cancel.label],
      cursor: pointer }; },
    targets(w, h) { const v = view(w, h); return Object.fromEntries(boxes().map((b) => [b.id, [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)]])); },
    hover(x, y, w, h) { const v = view(w, h); pointer = [Math.trunc((x - v.ox) / v.scale), Math.trunc((y - v.oy) / v.scale)]; },
    press(x, y, w, h) { pressed = hit(x, y, w, h); return pressed?.id ?? null; },
    release(token, x, y, w, h) { const b = hit(x, y, w, h); pressed = null; if (b && b.id === token) { activate(b); return true; } return false; },
    cancel() { pressed = null; },
    escape() { if (sub === 'tcpip') sub = 'menu'; else onExit(); },
    step() { tick++; },
    draw(out, w, h) {
      const v = view(w, h);
      out.fillStyle = '#000'; out.fillRect(0, 0, w, h); out.imageSmoothingEnabled = false;
      sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600);
      const bg = sub === 'menu' ? menuBg : tcpipBg;
      sctx.drawImage(bg.canvas, 0, 0); recordTiled(bg.path, bg, 0, 0);
      if (sub === 'menu') {
        drawLogos(sctx);
        for (const b of OTHER_LAYOUT.menu.buttons) {
          const box = boxes().find((x) => x.id === b.id), down = pressed === b.id;
          drawWide(sctx, b, down ? 1 : 0, box.x, box.y);
          drawButtonLabel(sctx, b.label, box.x, box.y, box.w, box.h);
        }
      } else {
        headingFont.drawAnchored(sctx, 'TCP/IP Options', OTHER_LAYOUT.tcpip.heading.x, OTHER_LAYOUT.tcpip.heading.y);
        bodyFont.drawAnchored(sctx, string('strYourIpAddress') || 'Your IP Address is: ', OTHER_LAYOUT.tcpip.ipLabel.x, OTHER_LAYOUT.tcpip.ipLabel.y);
        bodyFont.drawAnchored(sctx, ip, OTHER_LAYOUT.tcpip.ipValue.x, OTHER_LAYOUT.tcpip.ipValue.y);
        for (const b of OTHER_LAYOUT.tcpip.buttons) {
          const box = boxes().find((x) => x.id === b.id), down = pressed === b.id;
          drawWide(sctx, b, down ? 1 : 0, box.x, box.y);
          drawButtonLabel(sctx, b.label, box.x, box.y, box.w, box.h);
        }
        const c = OTHER_LAYOUT.tcpip.cancel, cbox = boxes().find((x) => x.id === 'cancel'), cdown = pressed === 'cancel';
        const m = medium[cdown ? 1 : 0];
        sctx.drawImage(m.canvas, c.x, cbox.y);
        let cx = c.x; for (const p of m.parts) { recordDc6(OTHER_ASSETS.mediumButton, p.index, p.frame, cx, cbox.y); cx += p.frame.width; }
        drawButtonLabel(sctx, c.label, c.x, cbox.y, c.width, c.height);
      }
      const f = cursorDc6.frames[0];
      const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
      sctx.drawImage(frameCanvas(f), left, top);
      recordDc6(FRONTEND.cursor, 0, f, left, top);
      out.drawImage(scene, v.ox, v.oy, 800 * v.scale, 600 * v.scale);
    },
  };
}
