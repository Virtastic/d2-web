// Retail's Battle.net lobby (1.13c D2Multi), drawn the way D2Multi builds it out of D2Win controls:
//   - the lobby a character lands in after character select (D2Multi+0xB300, the screen before ENTER
//     CHAT): the BIGMENU bnet background, the two small Battle.net logos, the right-hand blankbckg2
//     panel, CREATE / CHANNEL / JOIN / LADDER / QUIT, ENTER CHAT and HELP, the character box and the
//     news title;
//   - the chat screen ENTER CHAT opens (the same descriptor table, D2Multi+0x19D30..0x1A270);
//   - CREATE GAME (D2Multi+0x14BC0, descriptors +0x1A930..+0x1AE40, CANCEL +0x1AFF0) and JOIN GAME
//     (D2Multi+0x48F0, descriptors +0x1A510..+0x1A780) over the right-hand panel;
//   - the PopUpOK notice (descriptors D2Multi+0x1A360..+0x1A3F0).
// Every control below is a D2Win descriptor (type, x, bottom row y, w, h, ...) read off D2Multi's data
// section; the art slots resolve to the BIGMENU files D2Multi's loader (D2Multi+0x9400..0x9E00) opens.
import { composeTiled, createBlender, createFont, decodeFontTbl, recordTiled, textColourPalette,
  LABEL_TINT, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';
import { drawBg, present } from './widebg.mjs';
import { wrapSpaces } from './bnet-login-screen.mjs';

const B = 'data\\global\\ui\\BIGMENU\\';
const F = 'data\\local\\font\\latin\\';
export const BNET_LOBBY_ASSETS = {
  background: B + 'bnet.dc6',
  logoBlack: B + 'D2logoBlackBnet.dc6', logoFire: B + 'D2logoBnet.dc6',
  panel: B + 'blankbckg2.dc6', createPanel: B + 'creategamebckg.dc6', joinPanel: B + 'joingamebckg.dc6',
  topButton: B + 'chatrighttopbuttons.dc6', rightButton: B + 'chatrightbuttons.dc6',
  leftButton: B + 'chatleftbuttons.dc6', leftButton1: B + 'chatleftbuttons1.dc6', leftButton2: B + 'chatleftbuttons2.dc6',
  arrows: B + 'chatarrows.dc6',
  gameButton: B + 'GameButtonBlank.dc6', cancelButton: B + 'CancelButtonBlank.dc6',
  clickBox: B + 'joingameclickbox.dc6', greyBox: B + 'GreyLevelBox.dc6',
  radio: B + 'radiobutton.dc6', radioGrey: B + 'radiobuttonGrey.dc6',
  numberArrows: B + 'numberarrows.dc6', numberGreyArrows: B + 'numbergreyarrows.dc6',
  popup: B + 'PopUpOK.dc6',
  // D2Multi's font slots (R19C60.. R19C80).
  formal10: F + 'fontformal10', font16: F + 'font16', font24: F + 'font24', font30: F + 'font30',
  exocet10: F + 'fontexocet10', ridiculous: F + 'fontridiculous',
};

export const BNET_LOBBY_STRINGS = {
  send: 5305, whisper: 5306, squelch: 5307, help: 5308, emote: 5309, unsquelch: 5310,
  create: 5312, join: 5313, channel: 5314, ladder: 5315, quit: 5316, enterChat: 11126,
  newsTitle: 11123, level: 4057, lastDifficulty: 11124, none: 11125, normal: 5156, nightmare: 5155, hell: 5154,
  deadHardcore: 5304, ok: 5102, cancel: 5103,
  createGame: 5150, joinGame: 5151, gameName: 5274, password: 5225, passwordOptional: 5256, descriptionOptional: 5257,
  maxPlayers: 5258, levelDifference: 5259, levels: 5260, gameListHeader: 5275,
  diffNormal: 10018, diffNightmare: 10017, diffHell: 10016,
};
// The class names by class index (D2Multi+0x8D80); anything else is 5383.
export const CLASS_STRINGS = [4011, 4010, 4009, 4008, 4007, 10097, 10098];

// The lobby's controls (D2Multi+0x19D30..0x1A270): [id, type, x, y (bottom row), w, h, art, base frame, label].
const button = (id, x, y, w, h, art, base, label, extra = {}) => ({ id, type: 'button', x, y, w, h, art, base, label, ...extra });
export const BNET_LOBBY_LAYOUT = {
  send: button('send', 27, 470, 80, 20, 'leftButton', 0, 'send'),
  whisper: button('whisper', 107, 470, 80, 20, 'leftButton', 0, 'whisper'),
  chatHelp: button('chat-help', 187, 470, 80, 20, 'leftButton', 0, 'help'),
  squelch: button('squelch', 27, 490, 72, 20, 'leftButton1', 0, 'squelch'),
  unsquelch: button('unsquelch', 99, 490, 96, 20, 'leftButton2', 0, 'unsquelch'),
  emote: button('emote', 195, 490, 72, 20, 'leftButton1', 0, 'emote'),
  create: button('create', 533, 469, 120, 20, 'topButton', 0, 'create', { toggle: true }),
  channel: button('channel', 535, 490, 80, 20, 'rightButton', 0, 'channel', { toggle: true }),
  join: button('join', 652, 469, 120, 20, 'topButton', 0, 'join', { toggle: true }),
  ladder: button('ladder', 614, 490, 80, 20, 'rightButton', 0, 'ladder', { toggle: true }),
  quit: button('quit', 693, 490, 80, 20, 'rightButton', 0, 'quit'),
  arrows: [button('arrow-0', 725, 583, 25, 30, 'arrows', 0, null), button('arrow-1', 751, 583, 25, 30, 'arrows', 2, null),
    button('arrow-2', 725, 528, 25, 30, 'arrows', 4, null), button('arrow-3', 751, 528, 25, 30, 'arrows', 6, null)],
  enterChat: button('enter-chat', 27, 480, 120, 20, 'topButton', 0, 'enterChat'),
  help: button('help', 146, 480, 120, 20, 'topButton', 0, 'help'),
  charInfo: { x: 143, y: 588, w: 230, h: 87, font: 'formal10' },
  chatLog: { x: 28, y: 410, w: 354, h: 298, font: 'formal10' },
  chatEdit: { x: 28, y: 450, w: 335, h: 32, font: 'formal10', limit: 255 },
  newsTitle: { x: 28, y: 138, w: 354, h: 60, font: 'formal10', centred: true },
  serverBox: { x: 447, y: 398, w: 290, h: 269, font: 'formal10', centred: true },
  logos: [{ x: 84, y: 52 }, { x: 726, y: 52 }],
  panel: { x: 415, y: 447 },
  // CREATE GAME (D2Multi+0x14BC0).
  createGame: {
    title: { x: 496, y: 111, w: 200, h: 22, font: 'font30', centred: true, text: 'createGame' },
    labels: [
      { x: 427, y: 134, w: 150, h: 12, text: 'gameName' },
      { x: 427, y: 188, w: 300, h: 12, text: 'passwordOptional' },
      { x: 427, y: 243, w: 300, h: 12, text: 'descriptionOptional' },
      { x: 632, y: 306, w: 300, h: 12, text: 'maxPlayers', right: true },
      { x: 632, y: 342, w: 300, h: 12, text: 'levelDifference', right: true, levelGated: true },
      { x: 705, y: 342, w: 150, h: 12, text: 'levels', levelGated: true },
    ],
    edits: [
      { id: 'name', x: 432, y: 162, w: 158, h: 20, limit: 16 },
      { id: 'password', x: 432, y: 217, w: 158, h: 20, limit: 16, password: true },
      { id: 'description', x: 432, y: 268, w: 333, h: 20, limit: 32 },
      { id: 'maxPlayers', x: 657, y: 308, w: 27, h: 20, limit: 3, digits: true },
      { id: 'levelDifference', x: 657, y: 342, w: 27, h: 20, limit: 3, digits: true, levelGated: true },
    ],
    greyBoxes: [{ x: 647, y: 346 }, { x: 647, y: 311 }],
    ok: button('create-game', 594, 433, 172, 32, 'gameButton', 0, 'createGame'),
    cancel: button('panel-cancel', 433, 433, 96, 32, 'cancelButton', 0, 'cancel'),
    levelBox: button('level-box', 431, 341, 15, 16, 'clickBox', 0, null),
    radios: [button('difficulty-0', 430, 381, 16, 16, 'radio', 0, null), button('difficulty-1', 555, 381, 16, 16, 'radio', 0, null),
      button('difficulty-2', 698, 381, 16, 16, 'radio', 0, null)],
    radioLabels: [{ x: 459, y: 380, text: 'diffNormal' }, { x: 579, y: 380, text: 'diffNightmare' }, { x: 723, y: 380, text: 'diffHell' }],
    playerArrows: [button('players-up', 681, 296, 15, 12, 'numberArrows', 0, null), button('players-down', 681, 310, 15, 12, 'numberArrows', 2, null)],
    levelArrows: [button('level-up', 681, 332, 15, 12, 'numberArrows', 0, null), button('level-down', 681, 346, 15, 12, 'numberArrows', 2, null)],
    levelGreyArrows: { x: 681, y: 346 },
  },
  // JOIN GAME (D2Multi+0x48F0).
  joinGame: {
    title: { x: 496, y: 106, w: 200, h: 22, font: 'font30', centred: true, text: 'joinGame' },
    labels: [{ x: 428, y: 121, w: 150, h: 12, text: 'gameName' }, { x: 602, y: 121, w: 150, h: 12, text: 'password' },
      { x: 431, y: 212, w: 200, h: 12, text: 'gameListHeader' }],
    edits: [{ id: 'name', x: 432, y: 148, w: 155, h: 20, limit: 16 }, { id: 'password', x: 606, y: 148, w: 155, h: 20, limit: 16, password: true }],
    list: { x: 432, y: 393, w: 160, h: 173, font: 'formal10' },
    ok: button('join-game', 594, 433, 172, 32, 'gameButton', 0, 'joinGame'),
    cancel: button('panel-cancel', 433, 433, 96, 32, 'cancelButton', 0, 'cancel'),
  },
  // The notice (D2Multi+0x1A360..0x1A3F0): PopUpOK, its OK a CancelButtonBlank, a centred Font24 box inset 10/8.
  notice: { x: 268, y: 350, ok: button('notice-ok', 351, 337, 96, 32, 'cancelButton', 0, 'ok'),
    body: { x: 268, y: 320, w: 264, h: 120, insetX: 10, insetY: 8, font: 'font24', centred: true } },
};

// The character box (D2Multi+0xB4B6..0xB65C): title and name (red when hardcore), "Level N Class",
// "Last Difficulty Setting Completed:" and the last difficulty finished. `progression` is the
// character's +0x1EC & 0x1F; an expansion character counts five per difficulty, a classic one four.
export function characterInfoLines(character, string) {
  const c = character ?? {};
  const progression = (c.progression ?? 0) & 0x1f;
  const done = Math.trunc(progression / (c.expansion ? 5 : 4));
  const S = BNET_LOBBY_STRINGS;
  const classId = CLASS_STRINGS[c.charClass] ?? 5383;
  return [
    { text: String(c.name ?? ''), colour: c.hardcore ? 1 : 0 },
    { text: `${string(S.level)} ${Math.max(1, c.level | 0)} ${string(classId)}`, colour: 0 },
    { text: string(S.lastDifficulty), colour: 0 },
    { text: string(done === 0 ? S.none : done === 1 ? S.normal : done === 2 ? S.nightmare : S.hell), colour: 0 },
  ];
}

// CREATE GAME's difficulty rule (D2Multi+0x14BC0): Nightmare opens at progression 4 (classic) or 5
// (expansion), Hell at 8 or 10.
export function difficultiesAllowed(character) {
  const p = (character?.progression ?? 0) & 0x1f, step = character?.expansion ? 5 : 4;
  return [true, p >= step, p >= 2 * step];
}

// MCP_CREATEGAME / MCP_JOINGAME result codes and the D2Lang notice each raises. The codes are
// bnetdocs'; the strings are the D2Lang entries carrying the same text.
export const MCP_RESULT_STRINGS = {
  create: { 0x1e: 5137, 0x1f: 5138, 0x20: 5140, 0x6e: 5359 },
  join: { 0x29: 5160, 0x2a: 5159, 0x2b: 5161, 0x2c: 5162, 0x6e: 5359, 0x71: 5361, 0x73: 5364, 0x74: 5363, 0x78: 5362 },
};
export function gameErrorText(error, string) {
  const id = MCP_RESULT_STRINGS[error?.mcpStage]?.[error?.mcpResult];
  if (id) return string(id) || String(error.message);
  return String(error?.message ?? error);
}

// D2Win button label offsets (D2Win's button draw): FontRidiculous under 35 rows, else Exocet10; the
// label rises by these rows from the bottom and is centred in the remaining height.
const labelRise = (h) => (h >= 35 ? 4 : h === 32 ? 3 : h > 20 ? 2 : 1);

export function createBnetLobbyScreen({ read, decodeDc6, decodeDatPalette, string = () => '', character = {},
  now = () => performance.now(), registryDifficulty = 0, onCreateGame = async () => {}, onJoinGame = async () => {},
  onQuit = () => {}, documentObject = globalThis.document }) {
  const A = BNET_LOBBY_ASSETS, L = BNET_LOBBY_LAYOUT, S = BNET_LOBBY_STRINGS;
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
  const artCache = new Map();
  const art = (key) => {
    if (!artCache.has(key)) { const path = A[key]; const dc6 = decodeDc6(read(path)); artCache.set(key, { path, dc6, canvases: dc6.frames.map(frameCanvas) }); }
    return artCache.get(key);
  };
  const bg = { ...composeTiled(decodeDc6(read(A.background)), skyPalette, TITLE_COLUMNS, documentObject), path: A.background };
  const logoBlend = createBlender(skyPl2, skyPalette);
  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const blend = createBlender(skyPl2, palette);
  const fontCache = new Map();
  // A text face in a D2Win colour (0 white, 1 red, 2 green, 3 blue, 4 gold, 5 grey).
  const font = (key, colour = 0) => {
    const k = `${key}:${colour}`;
    if (!fontCache.has(k)) {
      const path = `${A[key]}.dc6`;
      fontCache.set(k, createFont({ dc6: decodeDc6(read(path)), tbl: decodeFontTbl(read(`${A[key]}.tbl`)),
        palette: colour ? textColourPalette(skyPl2, skyPalette, colour) : skyPalette, documentObject, path,
        drawAs: { op: 'celcolor', mode: 5, extra: colour } }));
    }
    return fontCache.get(k);
  };
  // Button labels: the tinted unit palette, blended (as on the login screen's buttons).
  const labelFont = (h) => {
    const key = h < 35 ? 'ridiculous' : 'exocet10', k = `label:${key}`;
    if (!fontCache.has(k)) {
      const path = `${A[key]}.dc6`;
      fontCache.set(k, createFont({ dc6: decodeDc6(read(path)), tbl: decodeFontTbl(read(`${A[key]}.tbl`)), palette: tinted, documentObject,
        path, drawAs: { op: 'celcolor', mode: 4, blend } }));
    }
    return fontCache.get(k);
  };
  const cursorDc6 = decodeDc6(read(FRONTEND.cursor));
  const cursorCanvas = frameCanvas(cursorDc6.frames[0]);
  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sctx = scene.getContext('2d', { willReadFrequently: true });
  const view = (w, h) => { const scale = Math.min(w / 800, h / 600); return { scale, ox: Math.round((w - 800 * scale) / 2), oy: Math.round((h - 600 * scale) / 2) }; };
  const text = (key) => string(S[key]) || '';

  const deadHardcore = Boolean(character.hardcore && character.dead);
  const allowed = difficultiesAllowed(character);
  // 'lobby' (the first view after character select) or 'chat'; `panel` is null, 'create' or 'join'.
  let mode = 'lobby', panel = null, notice = null, busy = false;
  let pressed = null, pointer = [799, 599];
  const chat = { lines: [], input: '' };
  const form = { name: '', password: '', description: '', maxPlayers: '4', levelDifference: '', levelLimit: false, difficulty: 0, focus: 'name' };
  let created = now(), pinned = null;
  const clock = () => (pinned === null ? now() : pinned);

  const openCreate = () => {
    panel = 'create';
    const d = Math.min(2, Math.max(0, registryDifficulty | 0));
    Object.assign(form, { name: '', password: '', description: '', maxPlayers: '4', levelDifference: '', levelLimit: false,
      difficulty: allowed[d] ? d : 0, focus: 'name' });
  };
  const openJoin = () => { panel = 'join'; Object.assign(form, { name: '', password: '', focus: 'name' }); };
  const closePanel = () => { panel = null; };

  const disabled = (b) => busy || ((b.id === 'create' || b.id === 'join') && deadHardcore) || (b.id === 'channel' && mode === 'lobby') ||
    (b.id.startsWith('difficulty-') && !allowed[Number(b.id.slice(11))]) ||
    ((b.id === 'level-up' || b.id === 'level-down') && !form.levelLimit);
  const toggled = (b) => (b.id === 'create' && panel === 'create') || (b.id === 'join' && panel === 'join') ||
    (b.id === 'level-box' && form.levelLimit) || (b.id === `difficulty-${form.difficulty}`);

  // The controls on show, in D2Multi's build order.
  const controls = () => {
    if (notice) return [L.notice.ok];
    const out = [];
    if (mode === 'chat') out.push(L.send, L.whisper, L.chatHelp, L.squelch, L.unsquelch, L.emote);
    out.push(L.create, L.channel, L.join, L.ladder, L.quit);
    if (mode === 'chat') out.push(...L.arrows); else out.push(L.enterChat, L.help);
    if (panel === 'create') {
      const C = L.createGame;
      out.push(C.ok, C.cancel, C.levelBox, ...C.radios, ...C.playerArrows);
      if (form.levelLimit) out.push(...C.levelArrows);
      for (const e of C.edits) out.push({ id: `field-${e.id}`, type: 'field', x: e.x, y: e.y, w: e.w, h: e.h });
    } else if (panel === 'join') {
      const J = L.joinGame;
      out.push(J.ok, J.cancel);
      for (const e of J.edits) out.push({ id: `field-${e.id}`, type: 'field', x: e.x, y: e.y, w: e.w, h: e.h });
    }
    return out;
  };
  const box = (b) => ({ x: b.x, y: b.y - b.h + 1, w: b.w, h: b.h });
  const at = (x, y, w, h) => { const v = view(w, h); return [(x - v.ox) / v.scale, (y - v.oy) / v.scale]; };
  const hit = (x, y, w, h) => {
    const [sx, sy] = at(x, y, w, h);
    return controls().find((b) => { const r = box(b); return sx >= r.x && sx < r.x + r.w && sy >= r.y && sy < r.y + r.h; }) ?? null;
  };

  const editsFor = () => (panel === 'create' ? L.createGame.edits.filter((e) => !e.levelGated || form.levelLimit)
    : panel === 'join' ? L.joinGame.edits : []);
  const run = async (task) => {
    // On success the lobby stays busy: the game's first frames replace the screen.
    busy = true;
    try { await task(); }
    catch (error) { busy = false; if (!error?.sessionLost) notice = { body: gameErrorText(error, string) }; }
  };
  const createGame = () => {
    if (busy || !form.name) return;
    const maxPlayers = Math.min(8, Math.max(1, Number(form.maxPlayers) || 8));
    void run(() => onCreateGame({ gameName: form.name, gamePassword: form.password, description: form.description, maxPlayers,
      difficulty: form.difficulty, levelDifference: form.levelLimit && form.levelDifference !== '' ? Number(form.levelDifference) : 0xff }));
  };
  const joinGame = () => { if (busy || !form.name) return; void run(() => onJoinGame({ gameName: form.name, gamePassword: form.password })); };
  const step = (key, delta, lo, hi) => { const n = Number(form[key]) || 0; form[key] = String(Math.min(hi, Math.max(lo, n + delta))); };

  const activate = (b) => {
    if (busy || disabled(b)) return;
    const id = b.id;
    if (id === 'notice-ok') { notice = null; return; }
    if (id.startsWith('field-')) { form.focus = id.slice(6); return; }
    if (id === 'quit') { onQuit(); return; }
    // CREATE and JOIN are toggles: a second press closes their panel (D2Multi+0x15380 / +0x14B70).
    if (id === 'create') { if (panel === 'create') closePanel(); else openCreate(); return; }
    if (id === 'join') { if (panel === 'join') closePanel(); else openJoin(); return; }
    if (id === 'panel-cancel') { closePanel(); return; }
    if (id === 'enter-chat') { mode = 'chat'; return; }
    if (id === 'send') { sendChat(); return; }
    if (id === 'create-game') { createGame(); return; }
    if (id === 'join-game') { joinGame(); return; }
    if (id === 'level-box') { form.levelLimit = !form.levelLimit; if (!form.levelLimit && form.focus === 'levelDifference') form.focus = 'name'; return; }
    if (id.startsWith('difficulty-')) { form.difficulty = Number(id.slice(11)); return; }
    if (id === 'players-up') { step('maxPlayers', 1, 1, 8); return; }
    if (id === 'players-down') { step('maxPlayers', -1, 1, 8); return; }
    if (id === 'level-up') { step('levelDifference', 1, 0, 99); return; }
    if (id === 'level-down') { step('levelDifference', -1, 0, 99); return; }
    // CHANNEL, LADDER, HELP, WHISPER, SQUELCH, UNSQUELCH, EMOTE and the scroll arrows open retail
    // panels and commands that need the Battle.net chat connection, which this client does not hold.
  };
  // NOT retail: the line is echoed into the log only; the chat channel is not joined over BNCS.
  const sendChat = () => {
    const line = chat.input.trim(); chat.input = '';
    if (line) chat.lines.push({ text: `<${character.name ?? ''}> ${line}`, colour: 0 });
  };

  // One cel at its anchor (left x, bottom row y); D2Win draws a disabled control 50% translucent.
  const drawCel = (c, index, x, y, mode = 5) => {
    const f = c.dc6.frames[index]; if (!f) return;
    const left = x + f.offsetX, top = y + f.offsetY - f.height + 1;
    if (mode === 1) { sctx.save(); sctx.globalAlpha = 0.5; sctx.drawImage(c.canvases[index], left, top); sctx.restore(); }
    else sctx.drawImage(c.canvases[index], left, top);
    recordDc6(c.path, index, f, left, top, { op: 'cel', mode });
  };
  // A D2Win 2x2 image (the 373x373 panels): the top cels anchored 117 rows above the bottom ones.
  const drawPanel = (key, x, y) => {
    const c = art(key), f = c.dc6.frames;
    drawCel(c, 0, x, y - f[2].height); drawCel(c, 1, x + f[0].width, y - f[3].height);
    drawCel(c, 2, x, y); drawCel(c, 3, x + f[2].width, y);
  };
  const drawButton = (b) => {
    const off = disabled(b), down = pressed === b.id || toggled(b);
    const key = b.id.startsWith('difficulty-') && !allowed[Number(b.id.slice(11))] ? 'radioGrey' : b.art;
    drawCel(art(key), b.base + (down ? 1 : 0), b.x, b.y, off ? 1 : 5);
    if (!b.label) return;
    const label = text(b.label), lf = labelFont(b.h);
    let x = b.x + Math.max(Math.trunc((b.w - lf.measure(label)) / 2), 0), y = b.y + labelRise(b.h) - Math.trunc((b.h - lf.lineHeight) / 2);
    if (pressed === b.id) { x -= 2; y += 2; }
    lf.drawAnchored(sctx, label, x, y);
  };
  // A D2Win text box: lines from one font height under its top (plus the inset), one font height apart.
  const drawTextBox = (t, lines) => {
    const insetX = t.insetX ?? 0, width = t.w - 2 * insetX;
    let y = t.y - t.h + (t.insetY ?? 0);
    for (const { text: value, colour = 0 } of lines) {
      const f = font(t.font ?? 'font16', colour);
      for (const line of wrapSpaces(value, width, (s) => f.measure(s))) {
        y += f.lineHeight;
        if (y > t.y) return;
        const tw = f.measure(line);
        const x = t.right ? t.x - tw : t.centred ? t.x + insetX + Math.max(Math.trunc((width - tw) / 2), 0) : t.x + insetX;
        f.drawAnchored(sctx, line, x, y);
      }
    }
  };
  // An edit box: its text at (x, bottom - h + 16), the focused one's "_" caret while GetTickCount / 250 is odd.
  const drawEdit = (e, value, focused) => {
    const f = font('formal10'), shown = e.password ? '*'.repeat(value.length) : value, y = e.y - e.h + 16;
    f.drawAnchored(sctx, shown, e.x, y);
    if (focused && Math.floor(clock() / 250) % 2 === 1) f.drawAnchored(sctx, '_', e.x + f.measure(shown), y);
  };
  const drawLogos = () => {
    const black = art('logoBlack'), fire = art('logoFire');
    for (const { x, y } of L.logos) {
      drawCel(black, 0, x, y);
      const n = fire.dc6.frames.length, i = Math.floor(Math.max(0, clock() - created) / 40) % (n - 1), f = fire.dc6.frames[i];
      const left = x + f.offsetX, top = y + f.offsetY - f.height + 1;
      logoBlend(sctx, f, left, top, 3);
      recordDc6(fire.path, i, f, left, top, { op: 'cel', mode: 3 });
    }
  };
  const drawCreate = () => {
    const C = L.createGame;
    drawPanel('createPanel', L.panel.x, L.panel.y);
    drawTextBox(C.title, [{ text: text(C.title.text) }]);
    for (const t of C.labels) drawTextBox({ ...t, font: 'font16' }, [{ text: text(t.text), colour: t.levelGated && !form.levelLimit ? 5 : 0 }]);
    for (const g of C.greyBoxes) drawCel(art('greyBox'), 0, g.x, g.y);
    for (const e of C.edits) if (!e.levelGated || form.levelLimit) drawEdit(e, form[e.id], form.focus === e.id);
    drawButton(C.ok); drawButton(C.cancel); drawButton(C.levelBox);
    C.radios.forEach((r, i) => {
      drawButton(r);
      drawTextBox({ ...C.radioLabels[i], w: 150, h: 12, font: 'font16' }, [{ text: text(C.radioLabels[i].text), colour: allowed[i] ? 4 : 5 }]);
    });
    for (const a of C.playerArrows) drawButton(a);
    if (form.levelLimit) for (const a of C.levelArrows) drawButton(a);
    else drawCel(art('numberGreyArrows'), 0, C.levelGreyArrows.x, C.levelGreyArrows.y);
  };
  const drawJoin = () => {
    const J = L.joinGame;
    drawPanel('joinPanel', L.panel.x, L.panel.y);
    drawTextBox(J.title, [{ text: text(J.title.text) }]);
    for (const t of J.labels) drawTextBox({ ...t, font: 'font16' }, [{ text: text(t.text) }]);
    for (const e of J.edits) drawEdit(e, form[e.id], form.focus === e.id);
    drawButton(J.ok); drawButton(J.cancel);
  };
  const drawNotice = () => {
    const N = L.notice;
    sctx.save(); sctx.globalAlpha = 0x7f / 0xff; sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600); sctx.restore();
    recordPrim('boxalpha', 0, 0, 800, 600, 0, 0x7f);
    const p = art('popup');
    drawCel(p, 0, N.x, N.y); drawCel(p, 1, N.x + p.dc6.frames[0].width, N.y);
    drawButton(N.ok);
    drawTextBox(N.body, [{ text: notice.body }]);
  };

  const typeInto = (key, value, limit, digits) => {
    if (form[key].length >= limit) return;
    if (digits && !/[0-9]/.test(value)) return;
    form[key] += value;
  };

  return {
    get mode() { return mode; },
    get panel() { return panel; },
    get busy() { return busy; },
    get evidence() {
      return { mode, panel, notice, busy, character: { name: character.name ?? '', charClass: character.charClass ?? null, level: character.level ?? null },
        info: characterInfoLines(character, string).map((l) => l.text), difficulties: allowed,
        form: { ...form, password: undefined, passwordLength: form.password.length }, chat: chat.lines.map((l) => l.text),
        boxes: controls().map((b) => ({ id: b.id, ...box(b), disabled: disabled(b) })), cursor: pointer };
    },
    // A failed create/join that kept the session: retail's notice over the lobby.
    fail(error) { notice = { body: gameErrorText(error, string) }; busy = false; },
    typeKey(code, key) {
      if (busy) return true;
      const enter = code === 'Enter' || code === 'NumpadEnter';
      if (notice) { if (enter || code === 'Escape') notice = null; return true; }
      if (panel) {
        const edits = editsFor(), ids = edits.map((e) => e.id);
        if (code === 'Tab') { form.focus = ids[(ids.indexOf(form.focus) + 1) % ids.length]; return true; }
        // Every edit's Enter callback is the panel's OK (D2Multi+0x142C0 create, +0x14100 join).
        if (enter) { if (panel === 'create') createGame(); else joinGame(); return true; }
        const e = edits.find((x) => x.id === form.focus); if (!e) return false;
        if (code === 'Backspace') { form[e.id] = form[e.id].slice(0, -1); return true; }
        if (!key || key.length !== 1 || key < ' ' || key > '~') return false;
        typeInto(e.id, key, e.limit, e.digits);
        return true;
      }
      // ENTER CHAT's key is Enter (descriptor D2Multi+0x1A060, key 13).
      if (mode === 'lobby') { if (enter) { mode = 'chat'; return true; } return false; }
      if (enter) { sendChat(); return true; }
      if (code === 'Backspace') { chat.input = chat.input.slice(0, -1); return true; }
      if (!key || key.length !== 1 || key < ' ') return false;
      if (chat.input.length < L.chatEdit.limit) chat.input += key;
      return true;
    },
    targets(w, h) { const v = view(w, h); return Object.fromEntries(controls().map((b) => { const r = box(b); return [b.id, [Math.round(v.ox + (r.x + r.w / 2) * v.scale), Math.round(v.oy + (r.y + r.h / 2) * v.scale)]]; })); },
    hover(x, y, w, h) { const [sx, sy] = at(x, y, w, h); pointer = [Math.trunc(sx), Math.trunc(sy)]; },
    press(x, y, w, h) { const b = hit(x, y, w, h); if (!b || disabled(b)) return null; pressed = b.id; return pressed; },
    release(token, x, y, w, h) { const b = hit(x, y, w, h); pressed = null; if (b && b.id === token) { activate(b); return true; } return false; },
    activate(id) { const b = controls().find((x) => x.id === id); if (b) activate(b); return Boolean(b); },
    cancel() { pressed = null; },
    // Escape: the notice's OK, the open panel's CANCEL, else QUIT (the lobby's escape button).
    escape() { if (busy) return; if (notice) notice = null; else if (panel) closePanel(); else onQuit(); },
    step() {},
    pinClock(ms, builtMs = 0) { pinned = ms; created = builtMs; },
    draw(out, w, h) {
      const v = view(w, h);
      out.fillStyle = '#000'; out.fillRect(0, 0, w, h); out.imageSmoothingEnabled = false;
      sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600);
      drawBg(sctx, bg.path, bg.canvas); recordTiled(bg.path, bg, 0, 0);
      if (mode === 'chat') { for (const b of [L.send, L.whisper, L.chatHelp, L.squelch, L.unsquelch, L.emote]) drawButton(b); }
      for (const b of [L.create, L.channel, L.join, L.ladder, L.quit]) drawButton(b);
      if (mode === 'chat') {
        for (const b of L.arrows) drawButton(b);
        const C = L.chatLog, f = font(C.font), rows = Math.floor(C.h / f.lineHeight);
        drawTextBox(C, chat.lines.slice(-rows));
        drawEdit(L.chatEdit, chat.input, !panel && !notice);
      } else {
        drawButton(L.enterChat); drawButton(L.help);
        drawTextBox(L.newsTitle, [{ text: text('newsTitle'), colour: 3 }]);
      }
      drawTextBox(L.charInfo, characterInfoLines(character, string));
      drawLogos();
      if (panel === 'create') drawCreate();
      else if (panel === 'join') drawJoin();
      else {
        drawPanel('panel', L.panel.x, L.panel.y);
        if (deadHardcore) drawTextBox(L.serverBox, [{ text: text('deadHardcore') }]);
      }
      if (notice) drawNotice();
      const f = cursorDc6.frames[0];
      const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
      sctx.drawImage(cursorCanvas, left, top);
      recordDc6(FRONTEND.cursor, 0, f, left, top);
      present(out, scene, v, bg.path);
    },
  };
}
