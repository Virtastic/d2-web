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
import { GAME_REFRESH_MS } from './bnet-gamelist.mjs';
import { composeTiled, createBlender, createFont, decodeFontTbl, recordTiled, textColourPalette,
  LABEL_TINT, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';
import { recordDc6, recordDcc, recordPrim } from './drawlist.mjs';
import { parseStatstring } from './bnet-statstring.mjs';
import { advanceBandRecord, bandTipLines } from './bnet-band-record.mjs';
import { drawBg, present, deriveWideBg } from './widebg.mjs';
import { wrapSpaces } from './bnet-login-screen.mjs';
import { localSlash } from './bnet-slash.mjs';

const B = 'data\\global\\ui\\BIGMENU\\';
const WAITING_FLOOR_Y = 494;   // retail y where the Battle.net chat art's moss floor starts; the waiting room has none
const F = 'data\\local\\font\\latin\\';
export const BNET_LOBBY_ASSETS = {
  background: B + 'bnet.dc6', waitingBackground: B + 'waitingroombkgd.dc6',
  logoBlack: B + 'D2logoBlackBnet.dc6', logoFire: B + 'D2logoBnet.dc6',
  panel: B + 'blankbckg2.dc6', createPanel: B + 'creategamebckg.dc6', joinPanel: B + 'joingamebckg.dc6',
  topButton: B + 'chatrighttopbuttons.dc6', rightButton: B + 'chatrightbuttons.dc6',
  leftButton: B + 'chatleftbuttons.dc6', leftButton1: B + 'chatleftbuttons1.dc6', leftButton2: B + 'chatleftbuttons2.dc6',
  squelch: 'data\\global\\UncompOverlays\\SquelchAura.dc6', selectAura: 'data\\global\\UncompOverlays\\SelectAura.dc6',
  joinScrollbar: 'data\\global\\ui\\FRONTEND\\joingamescrollbars.dc6', scrollbar: B + 'scrollbar.dc6', arrows: B + 'chatarrows.dc6', gem: B + 'gembutton.dc6',
  gameButton: B + 'GameButtonBlank.dc6', cancelButton: B + 'CancelButtonBlank.dc6',
  clickBox: B + 'joingameclickbox.dc6', greyBox: B + 'GreyLevelBox.dc6',
  radio: B + 'radiobutton.dc6', radioGrey: B + 'radiobuttonGrey.dc6',
  numberArrows: B + 'numberarrows.dc6', numberGreyArrows: B + 'numbergreyarrows.dc6',
  popup: B + 'PopUpOK.dc6',
  helpPanel: B + 'HelpBckg.dc6', channelPanel: B + 'createjoinbckg.dc6', ladderPanel: B + 'LadderChoiceEXP.dc6', wideButton: B + 'widebuttonblank.dc6',
  // D2Multi's font slots (R19C60.. R19C80).
  font8: F + 'font8', formal10: F + 'fontformal10', font16: F + 'font16', font24: F + 'font24', font30: F + 'font30',
  exocet10: F + 'fontexocet10', ridiculous: F + 'fontridiculous', sucker: F + 'reallythelastsucker',
};

export const BNET_LOBBY_STRINGS = {
  send: 5305, whisper: 5306, squelch: 5307, help: 5308, emote: 5309, unsquelch: 5310,
  create: 5312, join: 5313, channel: 5314, ladder: 5315, quit: 5316, enterChat: 11126,
  newsTitle: 11123, level: 4057, lastDifficulty: 11124, none: 11125, normal: 5156, nightmare: 5155, hell: 5154,
  hardcore: 5016, levelN: 5017, accountName: 5224, deadHardcore: 5304, ok: 5102, cancel: 5103, pleaseWait: 5243,
  createGame: 5150, joinGame: 5151, gameName: 5274, password: 5225, passwordOptional: 5256, descriptionOptional: 5257,
  maxPlayers: 5258, levelDifference: 5259, levels: 5260, gameListHeader: 5275,
  helpTitle: 5308, channelsTitle: 5263, channelName: 5262, ladderTitle: 5315,
  standardLadder: 5317, hardcoreLadder: 5318, expansionLadder: 10090, expansionHardcoreLadder: 10091,
  diffNormal: 10018, diffNightmare: 10017, diffHell: 10016,
};
// The class names by class index (D2Multi+0x8D80); anything else is 5383.
// No retail string is known for an empty ladder yet, so the screen says it in its own words.
const LADDER_EMPTY_TEXT = 'No characters are ranked on this ladder yet.';
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
  charInfo: { x: 143, y: 588, w: 230, h: 87, font: 'formal10', pitch: 19 },
  chatLog: { x: 28, y: 410, w: 354, h: 298, font: 'formal10', pitch: 19, scrollbar: { x: 400, bottom: 389, top: 85 } },
  chatEdit: { x: 28, y: 450, w: 335, h: 32, font: 'formal10', limit: 200 },
  newsTitle: { x: 28, y: 138, w: 354, h: 60, font: 'formal10', centred: true },
  gem: { x: 398, y: 474 },
  // The user band (D2Multi+0xF750): eleven 60-wide cells from x=40, portrait at y=571, name under it alternating 577 / 592.
  band: { x: 40, cell: 60, visible: 11, portraitY: 571, nameY: [577, 592], font: 'sucker', top: 492, bottom: 591, tipBottom: 488, tipPitch: 16 },
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
  // HELP: the list's strings (string.tbl 702..723, the two expansion ladder paragraphs after 722), each wrapped to the list's width and followed by
  // an empty row unless it is listed in `joined`; a 14-row list at pitch 19 with the join list's scroll bar cels along x 753.
  helpPanel: {
    title: { x: 500, y: 112, w: 200, h: 22, font: 'font30', centred: true, text: 'helpTitle' },
    ids: [702, 703, 704, 705, 706, 707, 708, 709, 710, 711, 712, 713, 714, 715, 716, 717, 718, 719, 720, 721, 722, 10092, 10093, 723],
    joined: [702, 704, 710, 713, 714, 716, 717, 719],
    list: { x: 447, w: 290, font: 'formal10', firstBaseline: 144, pitch: 19, rows: 14, colour: 4, scrollbar: { x: 753, bottom: 390, top: 134 } },
    ok: button('help-ok', 671, 433, 96, 32, 'cancelButton', 0, 'ok'),
  },
  // CHANNEL: the join panel's art with a channel name edit and the list of channels (retail asks the chat connection for it).
  channelPanel: {
    title: { x: 496, y: 114, w: 200, h: 22, font: 'font30', centred: true, text: 'channelsTitle' },
    labels: [{ x: 427, y: 135, w: 150, h: 12, text: 'channelName' }, { x: 427, y: 187, w: 150, h: 12, text: 'channelsTitle' }],
    edit: { id: 'channel', x: 432, y: 162, w: 158, h: 20, limit: 32 },
    list: { x: 432, firstBaseline: 212, pitch: 19, rows: 10, font: 'formal10', colour: 4, selColour: 0, area: { x: 432, top: 197, w: 321, h: 200 }, scrollbar: { x: 753, bottom: 392, top: 205, height: 197, page: 9 } },
    ok: button('channel-ok', 671, 433, 96, 32, 'cancelButton', 0, 'ok'),
    cancel: button('panel-cancel', 433, 433, 96, 32, 'cancelButton', 0, 'cancel'),
  },
  // LADDER: four wide buttons (two cels side by side), STANDARD and HARDCORE then the expansion pair.
  ladderPanel: {
    title: { x: 496, y: 112, w: 200, h: 22, font: 'font30', centred: true, text: 'ladderTitle' },
    buttons: [button('ladder-hardcore', 463, 238, 272, 32, 'wideButton', 0, 'hardcoreLadder', { kind: 1 }), button('ladder-standard', 463, 188, 272, 32, 'wideButton', 0, 'standardLadder', { kind: 0 }),
      button('ladder-expansion-hardcore', 463, 338, 272, 32, 'wideButton', 0, 'expansionHardcoreLadder', { kind: 3 }), button('ladder-expansion', 463, 288, 272, 32, 'wideButton', 0, 'expansionLadder', { kind: 2 })],
    cancel: button('panel-cancel', 433, 433, 96, 32, 'cancelButton', 0, 'cancel'),
  },
  // The ranked list: the HELP panel's art and list box, rank / name / level / experience columns.
  ladderList: {
    title: { x: 500, y: 112, w: 200, h: 22, font: 'font30', centred: true },
    list: { x: 447, w: 290, font: 'formal10', firstBaseline: 144, pitch: 19, rows: 13, colour: 4, scrollbar: { x: 753, bottom: 390, top: 134 },
      cols: { rank: 24, name: 32, level: 208, experience: 290 } },
    ok: button('ladder-list-ok', 671, 433, 96, 32, 'cancelButton', 0, 'ok'),
    back: button('ladder-list-back', 433, 433, 96, 32, 'cancelButton', 0, 'cancel'),
  },
  // JOIN GAME (D2Multi+0x48F0).
  joinGame: {
    title: { x: 496, y: 106, w: 200, h: 22, font: 'font30', centred: true, text: 'joinGame' },
    labels: [{ x: 428, y: 121, w: 150, h: 12, text: 'gameName' }, { x: 602, y: 121, w: 150, h: 12, text: 'password' },
      { x: 431, y: 212, w: 200, h: 12, text: 'gameListHeader' }],
    edits: [{ id: 'name', x: 432, y: 148, w: 155, h: 20, limit: 16 }, { id: 'password', x: 606, y: 148, w: 155, h: 20, limit: 16, password: true }],
    list: { x: 432, y: 393, w: 160, h: 173, font: 'formal10', firstBaseline: 235, pitch: 19, rows: 9, colour: 4, selectedColour: 0,
      // The scrollbar control (D2Win 0xAD60 / 0xAF40): its box runs `height` up from the row below `bottom`; a track click pages by `page` rows.
      scrollbar: { x: 592, bottom: 392, top: 229, height: 173, page: 9 },
      // The description control beside the list: the selected game's text, wrapped to its width, centred, a line every 18 rows.
      desc: { x: 609, w: 143, font: 'font8', colour: 4, firstBaseline: 213, pitch: 18, lines: 10 } },
    ok: button('join-game', 594, 433, 172, 32, 'gameButton', 0, 'joinGame'),
    cancel: button('panel-cancel', 433, 433, 96, 32, 'cancelButton', 0, 'cancel'),
  },
  // The join in flight (D2Multi 0x14100): the panel gone, the notice's popup frame with a CANCEL button and a centred Font24 `Please Wait` baseline 223.
  joining: { cancel: button('busy-cancel', 351, 337, 96, 32, 'cancelButton', 0, 'cancel'), text: { x: 400, y: 223, font: 'font24', string: 'pleaseWait' } },
  // The notice (D2Multi+0x1A360..0x1A3F0): PopUpOK, its OK a CancelButtonBlank, a centred Font24 box inset 10/8.
  notice: { x: 268, y: 350, ok: button('notice-ok', 351, 337, 96, 32, 'cancelButton', 0, 'ok'),
    body: { x: 268, y: 320, w: 264, h: 120, insetX: 10, insetY: 8, font: 'font24', centred: true } },
};

// The character box's title and name line is appended to the box's text list with colour 1 when the character record's
// hardcore bit (bit 2 of its flag word) is set, else 0: the colour is the third argument of the text-box append call.
// The statstring and event-flag meanings used below are read from the decompile: statstring flags 0x04 hardcore, 0x08 dead,
// 0x20 expansion, 0x40 ladder; the portrait token by event flag is 0x02, 0x08, 0x01, 0x04, 0x101000 in that priority.
// The character box (D2Multi+0xB4B6..0xB65C): title and name (red when hardcore), "Level N Class",
// "Last Difficulty Setting Completed:" and the last difficulty finished. `progression` is the
// character's +0x1EC & 0x1F; an expansion character counts five per difficulty, a classic one four.
// Realm names arrive as `Char*account`: retail shows the part before the first '*' (a leading '*' keeps the name whole) and takes the account from after it.
export const displayName = (n) => { if (n.startsWith('*')) return n; const i = n.indexOf('*'); return i > 0 ? n.slice(0, i) : n; };
export const accountName = (n) => { const i = n.indexOf('*'); return i >= 0 && i < n.length - 1 ? n.slice(i + 1) : n; };
// A channel user's name in the band (caption routine D2Win+0xD4D0). The portrait token comes from the event flags (operator 0x02
// first, then 0x08, then 0x01, then 0x04, then 0x101000); the tokens for 0x08 and 0x01 (not with 0x02) draw colour 3 over a colour-6
// shadow one pixel up AND one pixel left (the disassembly subtracts one from both the x and the y of the shadow call); every other
// user, operators included, draws colour 1 when the statstring is hardcore, else 0.
// The star comes from the text, not from a flag test in the caption: the token routine (D2Multi+0xF340) returns early for a non-operator
// user whose event flags hold 0x01, 0x04, 0x08, 0x1000 or 0x100000 (mask 0x10100d) and never reads the statstring, so the record keeps no
// product tag and no hardcore bit; the caption then finds no Diablo II tag and draws the "*account" text (see bandNameText).
export const bandReadsStatstring = (flags) => Boolean(flags & 2) || !(flags & 0x10100d);
export function bandNameStyle(flags, hardcore) {
  if (!(flags & 2) && flags & 9) return { layers: [[6, -1, -1], [3, 0, 0]] };
  return { layers: [[hardcore && bandReadsStatstring(flags) ? 1 : 0, 0, 0]] };
}
// The band text. The name arrives as `Char*account`. The lobby stores two strings for the user: "*account" (the whole name when it
// already starts with '*'; the whole name after the star only when the account part is empty) and the character name (the part before
// the '*', 16 characters at most, empty when the name starts with '*'). The caption draws the character name for the three Diablo II
// products and the "*account" text for every other product. Ladder-title prefixes (the caption's control-mask title bits) are not modelled.
const D2_PRODUCT_CODES = new Set(['VD2D', 'TS2D', 'PX2D']);
export function bandNameText(name, product) {
  const lead = name.startsWith('*');
  if (D2_PRODUCT_CODES.has(product)) return lead ? '' : displayName(name).slice(0, 16);
  return lead ? name.slice(0, 50) : `*${accountName(name)}`.slice(0, 50);
}
// `title(class, progression, hardcore, expansion)` is the engine's title word (D2Multi 0xB300 via D2Win #10205); none given, no prefix.
export function characterInfoLines(character, string, title = null) {
  const c = character ?? {};
  const progression = (c.progression ?? 0) & 0x1f;
  const done = Math.trunc(progression / (c.expansion ? 5 : 4));
  const S = BNET_LOBBY_STRINGS;
  const classId = CLASS_STRINGS[c.charClass] ?? 5383;
  return [
    { text: `${title ? title(c.charClass | 0, progression, c.hardcore ? 1 : 0, c.expansion ? 1 : 0) : ''}${c.name ?? ''}`, colour: c.hardcore ? 1 : 0 },
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

// MCP_CREATEGAME / MCP_JOINGAME result codes and the D2Lang notice each raises (D2Multi+0xDAE0's two result
// switches). Any other result is "Error Creating Game" / "Join Failed". The tables hold a string per result; the
// four character-dependent join results (0x73, 0x74, 0x7d) are picked in gameErrorText.
export const MCP_RESULT_STRINGS = {
  create: { 0x1e: 5137, 0x1f: 5138, 0x20: 5139 },
  join: { 0x29: 5160, 0x2a: 5159, 0x2b: 5161, 0x2c: 5162, 0x71: 5158, 0x78: 10101, 0x79: 10102 },
};
const CREATE_FAILED = 5140, JOIN_FAILED = 5141;
// 0x73 and 0x74 name the previous difficulty's boss, Diablo for a classic character and Baal for an expansion one; 0x7d
// names the other kind of ladder than the character's own.
const joinStringFor = (result, character) => {
  const expansion = Boolean(character?.expansion), ladder = Boolean(character?.ladder);
  if (result === 0x73) return expansion ? 21794 : 5364;
  if (result === 0x74) return expansion ? 21793 : 5363;
  if (result === 0x7d) return ladder ? 10929 : 10930;
  return MCP_RESULT_STRINGS.join[result];
};
// A game that ended under the player (D2GS socket closed): D2Lang 5357, the disconnect notice.
export const GAME_DISCONNECTED = 5357;
// A refused character logon: 0x46 is "Player Not Found", 0x7b the expired-character notice naming the character (a
// server that retires characters unused for a set time sends it). Any other result raises no notice of its own.
export const CHARLOGON_NOT_FOUND = 5133, CHARLOGON_EXPIRED = 11145;
function charlogonText(result, string, character) {
  if (result === 0x46) return string(CHARLOGON_NOT_FOUND);
  if (result === 0x7b) return string(CHARLOGON_EXPIRED).replace('%s', character?.name ?? '');
  return '';
}
export function gameErrorText(error, string, character = null) {
  if (error?.noticeString) return string(error.noticeString) || String(error.message);
  const stage = error?.mcpStage;
  if (stage === 'charlogon') return charlogonText(error.mcpResult, string, character) || String(error.message);
  if (stage !== 'create' && stage !== 'join') return String(error?.message ?? error);
  const id = stage === 'create' ? (MCP_RESULT_STRINGS.create[error.mcpResult] ?? CREATE_FAILED)
    : (joinStringFor(error.mcpResult, character) ?? JOIN_FAILED);
  return string(id) || String(error.message);
}

// D2Win button label offsets (D2Win's button draw): FontRidiculous under 35 rows, else Exocet10; the
// label rises by these rows from the bottom and is centred in the remaining height.
const labelRise = (h) => (h >= 35 ? 4 : h === 32 ? 3 : h > 20 ? 2 : 1);

// The user-list portraits animate off GetTickCount: frame = floor(elapsed / period) % frames, elapsed counted from the user's arrival.
// Frame counts are the cels'; the periods (ms per frame) are fitted to the recorded phase table (RH is not a single period: its slots fit 563..583, so it can slip a tick over a long run).
export const PORTRAIT_ANIM = { AM: [7, 500], SO: [7, 500], NE: [7, 500], AI: [7, 500], BA: [7, 500], PA: [7, 357], DZ: [5, 550], RH: [15, 576] };
export function portraitFrameAt(token, elapsedMs) {
  const [n, period] = PORTRAIT_ANIM[token] ?? [1, 1];
  return Math.floor(Math.max(0, elapsedMs) / period) % n;
}

export function createBnetLobbyScreen({ read, decodeDc6, decodeDatPalette, string = () => '', character = {},
  now = () => performance.now(), registryDifficulty = 0, onCreateGame = async () => {}, onJoinGame = async () => {}, onRefreshGames = () => {},
  offline = false,
  charTitle = null, portraitFrame = null, auraFrame = null, figure = null, onQuit = () => {}, onEnterChat = () => {}, onSendChat = () => false, onJoinChannel = () => {}, onRefreshChannels = () => {}, onLadder = null, documentObject = globalThis.document }) {
  const A = BNET_LOBBY_ASSETS, L = BNET_LOBBY_LAYOUT, S = BNET_LOBBY_STRINGS;
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const skyPalette = decodeDatPalette(read('data\\global\\palette\\Act1\\pal.dat')); // the lobby is drawn under the Act 1 palette; Sky turns the floor blue
  const skyPl2 = read('data\\global\\palette\\Act1\\pal.pl2');
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
  // Before ENTER CHAT the lobby sits on the waiting-room backdrop; the chat view uses the Battle.net one.
  let waitBg = null;
  // The waiting room has no painted widescreen image of its own: it shares the Battle.net frame, so it borrows the chat screen's (bnet) outside the 800x600.
  const waitingBackground = () => {
    const w = { ...composeTiled(decodeDc6(read(A.waitingBackground)), skyPalette, TITLE_COLUMNS, documentObject), path: A.waitingBackground };
    deriveWideBg(A.waitingBackground, A.background, w.canvas, { documentObject, blankBelow: WAITING_FLOOR_Y });
    return w;
  };
  const lobbyBg = () => (mode === 'chat' ? bg : (waitBg ??= waitingBackground()));
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
  // The join panel's list: `{ name, password, difficulty, description }` per game, in the order the realm sent them (setGames).
  let games = [];
  // CREATE, JOIN, CHANNEL and LADDER latch pressed on a mouse press and open their panel then, not on release; a press on the one already
  // latched is swallowed, and any other panel unlatches it. `latch` is the id a press latched (a programmatic open leaves it null).
  let latch = null;
  let pressed = null, pointer = [799, 599];
  let chatBar = null;   // the chat log's scroll range as last drawn: { max, at }, null with no scrollbar
  // replyTo: the last whisperer (/r); notify: /d2notify. Retail's notify flag starts clear (its only writer is /d2notify).
  const chat = { lines: [], input: '', live: false, channel: '', users: [], scroll: 0, picked: null, enable: { whisper: false, squelch: false, unsquelch: false }, replyTo: '', notify: false, history: [], recall: -1 };
  const headerText = () => (chat.channel ? `${chat.channel} (${chat.users.length})` : '');
  const form = { name: '', password: '', description: '', maxPlayers: '4', levelDifference: '4', levelLimit: false, difficulty: 0, focus: 'name', channel: '' };
  let created = now(), pinned = null;
  const clock = () => (pinned === null ? now() : pinned);

  const openCreate = () => {
    panel = 'create';
    const d = Math.min(2, Math.max(0, registryDifficulty | 0));
    Object.assign(form, { name: '', password: '', description: '', maxPlayers: '4', levelDifference: '4', levelLimit: false,
      difficulty: allowed[d] ? d : 0, focus: 'name' });
  };
  // The join panel asks the realm for its games once when it opens (JOIN's callback). Retail's idle loop adds a free-running 15 s timer that is only
  // looked at (and restarted) while the join panel is open: the first look after boot is always due, and a panel reopened within 15 s of the timer's last
  // firing waits out the rest of that period instead of restarting it.
  let timerAt = -Infinity;
  const refreshGames = () => { onRefreshGames(); };
  // The list control's state: the row picked (an index into `games`, -1 none) and the first row on show.
  let selected = -1, listFirst = 0;
  // The description control beside the list holds the text of the last row picked; every click on the list, and every refresh, empties it first.
  let desc = '';
  // D2Multi 0x14730 keeps the last row picked and when (retail's globals outlive the panel): the same row again within 500 ms is a double-click.
  let lastPick = { row: -1, at: 0 };
  // The scrollbar control: the thumb grabbed, which arrow shows pressed, and whether the mouse button is still down.
  const bar = { grab: false, up: false, down: false, held: false };
  let joining = false;
  let openStamp = 0;
  const openJoin = () => { panel = 'join'; Object.assign(form, { name: '', password: '', focus: 'name' }); selected = -1; listFirst = 0; desc = '';
  // D2Multi 0x148F0 builds the panel's list empty and asks the realm for games only when 5 s have passed since the last open (the stamp moves only then, and starts at 0, the zero-filled static, so an open under 5 s after the clock starts asks nothing).
  games = []; if (clock() - openStamp >= 5000) { openStamp = clock(); refreshGames(); } };
  // HELP, CHANNEL and LADDER: panels of their own. HELP's list keeps its place while the panel is shut; CHANNEL's list is what the chat
  // connection last reported (`setChannels`), its name edit starts on the channel in.
  let helpFirst = 0, channels = [], chanFirst = 0;
  // The ranked list a LADDER button opens: rows null while the realm is asked, [] for an empty ladder.
  let ladder = { kind: 0, rows: null }, ladderFirst = 0;
  const openHelp = () => { panel = 'help'; };
  let chanSel = -1;
  const openChannel = () => { onRefreshChannels(); panel = 'channel'; form.channel = chat.channel; form.focus = 'channel'; chanSel = -1; chanFirst = 0; };
  const openLadder = () => { panel = 'ladder'; };
  const closePanel = () => { panel = null; };
  const openLadderList = (kind) => {
    if (!onLadder) return;   // no realm behind the lobby (the headless front end): retail's button changes nothing
    const mine = ladder = { kind, rows: null }; ladderFirst = 0; panel = 'ladder-list';
    Promise.resolve().then(() => onLadder(kind)).then((rows) => { if (ladder === mine) mine.rows = Array.isArray(rows) ? rows : []; },
      (error) => { if (ladder !== mine) return; panel = 'ladder'; notice = { body: gameErrorText(error, string, character) }; });
  };
  // The help list: each string wrapped to the list's width, a blank row after it unless the string runs on into the next one.
  let helpRows = null;
  const helpLines = () => {
    if (helpRows) return helpRows;
    const H = L.helpPanel, f = font(H.list.font, H.list.colour);
    helpRows = [];
    for (const id of H.ids) {
      helpRows.push(...wrapSpaces(string(id) || '', H.list.w, (t) => f.measure(t)));
      if (!H.joined.includes(id)) helpRows.push('');
    }
    return helpRows;
  };

  const disabled = (b) => (busy && !joining && b.id !== 'busy-cancel') || ((b.id === 'create' || b.id === 'join') && deadHardcore) || (b.id === 'channel' && mode === 'lobby') ||
    (b.id.startsWith('difficulty-') && !allowed[Number(b.id.slice(11))]) || (b.id === 'join-game' && !form.name) || (b.id === 'channel-ok' && !form.channel.trim()) ||
    ((b.id === 'level-up' || b.id === 'level-down') && !form.levelLimit);
  const toggled = (b) => (b.id === 'level-box' && form.levelLimit) || (b.id === 'channel' && panel === 'channel') || (latch === b.id && panel === b.id) || (b.id === 'ladder' && panel === 'ladder') || (b.id === `difficulty-${form.difficulty}`);

  // The controls on show, in D2Multi's build order.
  const controls = () => {
    if (notice) return [L.notice.ok];
    const out = [];
    if (mode === 'chat') out.push(L.send, L.whisper, L.chatHelp, L.squelch, L.unsquelch, L.emote);
    out.push(L.create, L.channel, L.join, L.ladder, L.quit);
    // The chat line stays a control under CREATE / JOIN / CHANNEL: a press on it takes the keyboard back from the panel's box.
    if (mode === 'chat' && editsFor().length) out.push({ id: 'field-chat', type: 'field', ...L.chatEdit });
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
    } else if (panel === 'help') out.push(L.helpPanel.ok);
    else if (panel === 'channel') {
      const C = L.channelPanel, e = C.edit;
      out.push(C.ok, C.cancel, { id: `field-${e.id}`, type: 'field', x: e.x, y: e.y, w: e.w, h: e.h });
    } else if (panel === 'ladder') out.push(...L.ladderPanel.buttons, L.ladderPanel.cancel);
    else if (panel === 'ladder-list') out.push(L.ladderList.ok, L.ladderList.back);
    return out;
  };
  const box = (b) => ({ x: b.x, y: b.y - b.h + 1, w: b.w, h: b.h });
  const at = (x, y, w, h) => { const v = view(w, h); return [(x - v.ox) / v.scale, (y - v.oy) / v.scale]; };
  const hit = (x, y, w, h) => {
    const [sx, sy] = at(x, y, w, h);
    return controls().find((b) => { const r = box(b); return sx >= r.x && sx < r.x + r.w && sy >= r.y && sy < r.y + r.h; }) ?? null;
  };

  // The list's row under a point (D2Win 0x8C60: (y - top) / pitch + the first row), -1 outside the list or past the last game.
  const inList = (x, y, w, h) => {
    if (panel !== 'join' || busy || notice) return false;
    const [sx, sy] = at(x, y, w, h), J = L.joinGame.list, top = J.y - J.h;
    return sx >= J.x && sx < J.x + J.w && sy >= top && sy < J.y;
  };
  // The CHANNEL panel's list (D2Win 0x8C60 again): a press on a name selects it and copies it into the name edit; below the last name the pick clears and the edit keeps its text.
  const inChanList = (x, y, w, h) => {
    if (panel !== 'channel' || busy || notice) return false;
    const [sx, sy] = at(x, y, w, h), A = L.channelPanel.list.area;
    return sx >= A.x && sx < A.x + A.w && sy >= A.top && sy < A.top + A.h;
  };
  const pickChannel = (x, y, w, h) => {
    const [, sy] = at(x, y, w, h), A = L.channelPanel.list.area, i = chanFirst + Math.trunc((sy - A.top) / L.channelPanel.list.pitch);
    if (i < 0 || i >= channels.length) { chanSel = -1; return; }
    chanSel = i; form.channel = channels[i];
  };
  const listRow = (x, y, w, h) => {
    if (!inList(x, y, w, h)) return -1;
    const [, sy] = at(x, y, w, h), J = L.joinGame.list, top = J.y - J.h;
    const i = Math.min(listFirst, Math.max(0, games.length - 1)) + Math.trunc((sy - top) / J.pitch);
    return i < games.length ? i : -1;
  };
  // A row click (D2Multi 0x11E20): its game name goes into the name box, which loses the focus; the password box is left as typed.
  const selectGame = (i) => { selected = i; form.name = games[i].name; form.focus = null; desc = games[i].description; };
  // A press inside the list (D2Win 0x8C60 sets the row, D2Multi 0x14730 reacts): the description empties; below the last row the pick is
  // cleared and nothing else changes (the name box keeps its text); the same row twice within 500 ms joins that game, any other click selects.
  const pickRow = (row) => {
    desc = '';
    if (row < 0) { selected = -1; return; }
    selected = row;
    if (row === lastPick.row && clock() - lastPick.at <= 500) { joinGame(); return; }
    lastPick = { row, at: clock() };
    selectGame(row);
  };
  // The list's scrollbar control (D2Win 0xAD60 on a mouse press, 0xAF40 each frame while the thumb is held). Its range is the games past the rows
  // on show; a press on an arrow steps one row, on the thumb grabs it, on the track pages; a grabbed thumb follows the pointer.
  const barMax = () => Math.max(0, games.length - L.joinGame.list.rows);
  // The bar the mouse works on: the join list's, or the CHANNEL list's (the same D2Win control with its own range and box).
  const barOf = () => {
    if (busy || notice) return null;
    if (panel === 'join') { const S = L.joinGame.list.scrollbar, max = barMax(); return max ? { S, max, pos: () => listFirst, set: (p) => { listFirst = Math.max(0, Math.min(max, p)); } } : null; }
    if (panel === 'ladder-list') { const S = L.ladderList.list.scrollbar, max = Math.max(0, (ladder.rows?.length ?? 0) - L.ladderList.list.rows); return max ? { S, max, pos: () => Math.min(ladderFirst, max), set: (p) => { ladderFirst = Math.max(0, Math.min(max, p)); } } : null; }
    if (panel === 'channel') { const S = L.channelPanel.list.scrollbar, max = Math.max(0, channels.length - L.channelPanel.list.rows); return max ? { S, max, pos: () => Math.min(chanFirst, max), set: (p) => { chanFirst = Math.max(0, Math.min(max, p)); } } : null; }
    return null;
  };
  const barBox = (S) => {
    const f = art('joinScrollbar').dc6.frames[5];
    return { x: S.x, top: S.bottom + 1 - S.height, w: f.width, h: S.height, H: S.height };
  };
  const barDrag = (b, sy) => { const B = barBox(b.S); b.set(Math.trunc(((sy - B.top) - 11) * b.max / (B.H - 30))); };
  const barPress = (x, y, w, h) => {
    const b = barOf(); if (!b) return false;
    const [sx, sy] = at(x, y, w, h), B = barBox(b.S), max = b.max;
    if (sx < B.x || sx > B.x + B.w || sy < B.top || sy >= B.top + B.H) return false;
    // A new press starts with no arrow held and no thumb grabbed (the release of the last one is not always delivered by the browser).
    bar.grab = bar.up = bar.down = false;
    const local = Math.trunc(sy) - B.top, pos = b.pos(), track = B.H - 30, thumb = Math.trunc(pos * track / max);
    bar.held = true;
    if (local >= thumb + 10 && local <= thumb + 20) bar.grab = true;
    if (local < 11 && !bar.down && !bar.grab) { bar.up = true; b.set(pos - 1); }
    else if (local > B.H - 10 && !bar.up && !bar.grab) { bar.down = true; b.set(pos + 1); }
    else if (!bar.down && !bar.up) {
      if (bar.grab) b.set(Math.trunc((local - 11) * max / track));
      else if (local < thumb + 10) b.set(pos - b.S.page);
      else if (local > thumb + 20) b.set(pos + b.S.page);
    }
    return true;
  };

  const editsFor = () => (panel === 'create' ? L.createGame.edits.filter((e) => !e.levelGated || form.levelLimit)
    : panel === 'join' ? L.joinGame.edits : panel === 'channel' ? [L.channelPanel.edit] : []);
  const run = async (task) => {
    // On success the lobby stays busy: the game's first frames replace the screen.
    busy = true;
    try { await task(); }
    catch (error) { busy = false; joining = false; if (!error?.sessionLost) notice = { body: gameErrorText(error, string, character) }; }
  };
  const createGame = () => {
    if (busy || !form.name) return;
    // D2Multi 0x142C0 sends atol(text) & 0xff for both boxes (an empty box is 0); the level byte is 0xff with the checkbox off.
    const maxPlayers = (Number(form.maxPlayers) || 0) & 0xff;
    void run(() => onCreateGame({ gameName: form.name, gamePassword: form.password, description: form.description, maxPlayers,
      difficulty: form.difficulty, levelDifference: form.levelLimit ? (Number(form.levelDifference) || 0) & 0xff : 0xff }));
  };
  const joinGame = () => {
    if (busy) return;   // the JOIN button is greyed with no name, but the Enter handler behind the edit only needs the box to exist: an empty name goes on to the join request
    joining = true; void run(() => onJoinGame({ gameName: form.name, gamePassword: form.password }));
  };
  // The arrows apply D2Multi's exact rules (0x13200 / 0x131A0 / 0x13140 / 0x130E0): a step only when it stays inside the range, no clamp otherwise.
  const step = (key, delta, lo, hi) => { const n = Number(form[key]) || 0; const m = n + delta; form[key] = String(m >= lo && m <= hi ? m : n); };
  // The change callback (0x13AF0): a non-empty players box is clamped at once, below 1 to 1 and 9 or more to 8.
  const clampPlayers = () => { if (form.maxPlayers !== '') { const n = Number(form.maxPlayers) || 0; form.maxPlayers = String(n < 1 ? 1 : n >= 9 ? 8 : n); } };

  const scrollUsers = (delta) => {
    const n = chat.users.length, v = L.band.visible;
    chat.scroll = n <= v ? 0 : Math.max(0, Math.min(n - v, chat.scroll + delta));
  };
  // The user a pick names, as the list holds it now.
  const pickedUser = () => (chat.picked ? chat.users.find((u) => u.name === chat.picked) ?? null : null);
  // Retail re-reads the buttons' enabled state when a user is picked and when the line's text changes, never when a pick is dropped:
  // WHISPER needs a pick and text, SQUELCH a pick that is not squelched, UNSQUELCH one that is.
  const reeval = () => {
    const u = pickedUser(), sq = Boolean(u && (u.flags ?? 0) & 0x20);
    chat.enable = { whisper: Boolean(u && chat.input.length), squelch: Boolean(u && !sq), unsquelch: Boolean(u && sq) };
  };
  // The band cell under a point (index among those on show), -1 when none or empty.
  const bandCell = (sx, sy) => {
    const B = L.band, i = Math.floor((sx - B.x) / B.cell);
    return sy >= B.top && sy <= B.bottom && sx >= B.x && i >= 0 && i < B.visible && chat.users[chat.scroll + i] ? i : -1;
  };
  // A click on a user picks them; a click on the one picked drops the pick.
  const clickUser = (i) => {
    const u = chat.users[chat.scroll + i]; if (!u) return;
    if (chat.picked === u.name) { chat.picked = null; return; }
    chat.picked = u.name; reeval();
  };
  const activate = (b) => {
    if (busy || disabled(b)) return;
    const id = b.id;
    if (id === 'notice-ok') { notice = null; return; }
    if (id.startsWith('field-')) { form.focus = id.slice(6); return; }
    if (id === 'quit') { onQuit(); return; }
    // CREATE and JOIN are toggles: a second press closes their panel (D2Multi+0x15380 / +0x14B70).
    if (id === 'create') { if (panel !== 'create') openCreate(); return; }
    if (id === 'join') { if (panel !== 'join') openJoin(); return; }
    if (id === 'ladder-list-back') { panel = 'ladder'; return; }
    if (id === 'panel-cancel' || id === 'help-ok' || id === 'ladder-list-ok') { closePanel(); return; }
    // HELP opens its panel from either button; CHANNEL and LADDER are toggles like CREATE and JOIN.
    if (id === 'help' || id === 'chat-help') { if (panel !== 'help') openHelp(); return; }
    if (id === 'channel') { if (panel !== 'channel') openChannel(); return; }
    if (id === 'ladder') { if (panel !== 'ladder') openLadder(); return; }
    if (id === 'channel-ok') { joinChannel(); return; }
    if (id.startsWith('ladder-')) { openLadderList(L.ladderPanel.buttons.find((x) => x.id === id).kind); return; }
    if (id === 'enter-chat') { mode = 'chat'; onEnterChat(); return; }
    // The chat buttons only act on a live chat connection; WHISPER and the SQUELCHes need a picked user.
    if (id === 'whisper') { if (chat.picked && chat.input.length) { sendChat(true); reeval(); } return; }
    // EMOTE sends the line as typed (blanks and all) and clears it once it has gone; a line that opens with a slash is refused.
    if (id === 'emote') { const t = chat.input; if (t.length && !t.startsWith('/') && chat.live && sendCommand(`/me ${t}`)) { chat.input = ''; reeval(); } return; }
    if (id === 'squelch' || id === 'unsquelch') {
      // The pick stays.
      if (chat.picked && chat.live) { const n = chat.picked.replace(/^.*\*/, '') || chat.picked; sendCommand(`/${id} *${n}`); }
      return;
    }
    if (id === 'send') { sendChat(); return; }
    if (id === 'create-game') { createGame(); return; }
    if (id === 'join-game') { joinGame(); return; }
    if (id === 'level-box') { form.levelLimit = !form.levelLimit; if (!form.levelLimit && form.focus === 'levelDifference') form.focus = 'name'; return; }
    if (id.startsWith('difficulty-')) { form.difficulty = Number(id.slice(11)); return; }
    if (id === 'players-up') { step('maxPlayers', 1, 1, 8); return; }
    if (id === 'players-down') { step('maxPlayers', -1, 1, 8); return; }
    if (id === 'level-up') { step('levelDifference', 1, 0, 99); return; }
    if (id === 'level-down') { step('levelDifference', -1, 0, 99); return; }
    // The user list scroll (D2Multi+0x10D10): arrows -10 / +10 / -1 / +1, clamped to 0..count-11.
    if (id.startsWith('arrow-')) { scrollUsers([-10, 10, -1, 1][Number(id.slice(6))]); return; }
    // CHANNEL, LADDER, HELP, WHISPER, SQUELCH, UNSQUELCH and EMOTE open retail
    // panels and commands that need the Battle.net chat connection, which this client does not hold.
  };
  // A line goes out as CHATCOMMAND on the open BNCS socket; the server's TALK event brings it back
  // into the log. With no chat connection the line is only echoed here.
  // Slash commands from the buttons: at most one a second (retail's 1 s gate), on the pinned clock when set.
  let lastCommand = -Infinity;
  const sendCommand = (line) => {
    const at = clock();
    if (at - lastCommand < 1000) return false;
    lastCommand = at; return onSendChat(line);
  };
  const joinChannel = () => {
    const name = form.channel.trim(); if (!name) return;
    closePanel(); onJoinChannel(name);
  };
  // The line goes out as typed: blanks count as text (retail tests the length, not the trimmed text). Only the WHISPER button
  // addresses the picked user; SEND and Enter send the line as it is. Enter on a line of blanks only clears it.
  const sendChat = (whisper = false, enterKey = false) => {
    const line = chat.input; chat.input = ''; chat.recall = -1;
    if (!line || (enterKey && !line.trim())) return;
    // Retail stores every slash line it is sent (handled locally or not) newest first, ten deep; Tab recalls them.
    if (!whisper && line[0] === '/') { chat.history.unshift(line); if (chat.history.length > 10) chat.history.pop(); }
    chat.first = null;
    // A slash line retail handles itself (/r, /reply, /d2notify) never reaches the server.
    const local = !whisper ? localSlash(line, chat) : null;
    if (local) { if (local.notice) chat.lines.push({ text: local.notice, colour: local.colour }); if (!local.send) return; }
    const out = local ? local.send : whisper && chat.picked ? `/whisper *${chat.picked.replace(/^.*\*/, '') || chat.picked} ${line}` : line;
    if (chat.live && onSendChat(out)) {
      // Battle.net never sends a talk line back to the one who said it (PvPGN skips the sender), so the line is drawn here;
      // a server that does echo it is matched by chatEvent and not drawn twice.
      if (out === line && line[0] !== '/') {
        const me = character.name ?? ''; chat.first = null;
        chat.lines.push({ text: `<${me}> ${line}`, name: `<${me}> `, colour: 0, nameColour: 4 }); if (chat.lines.length > 200) chat.lines.shift();
        (chat.selfSaid ??= []).push(line); if (chat.selfSaid.length > 20) chat.selfSaid.shift();
      }
      return;
    }
    chat.lines.push({ text: `<${character.name ?? ''}> ${line}`, colour: 0 });
  };

  // One cel at its anchor (left x, bottom row y); D2Win draws a disabled control 50% translucent.
  const drawCel = (c, index, x, y, mode = 5) => {
    const f = c.dc6.frames[index]; if (!f) return;
    const left = x + f.offsetX, top = y + f.offsetY - f.height + 1;
    if (mode === 1) logoBlend(sctx, f, left, top, 1);
    else sctx.drawImage(c.canvases[index], left, top);
    recordDc6(c.path, index, f, left, top, { op: 'cel', mode });
  };
  // A D2Win 2x2 image (the 373x373 panels): the top cels anchored 117 rows above the bottom ones.
  const drawPanel = (key, x, y) => {
    const c = art(key), f = c.dc6.frames;
    drawCel(c, 0, x, y - f[2].height + 1); drawCel(c, 1, x + f[0].width, y - f[3].height + 1);
    drawCel(c, 2, x, y); drawCel(c, 3, x + f[2].width, y);
  };
  // Drawn 50% translucent and ignoring the mouse (no pressed cel, no click): SEND / WHISPER / EMOTE with nothing typed, SQUELCH / UNSQUELCH with
  // no user picked, the scroll arrows while every user fits the band.
  const dim = (b) => mode === 'chat' && (['whisper', 'squelch', 'unsquelch'].includes(b.id) ? !chat.enable[b.id] : ['send', 'emote'].includes(b.id) ? !chat.input.length :
    b.id.startsWith('arrow-') && (b.id === 'arrow-0' || b.id === 'arrow-2' ? chat.scroll <= 0 : chat.scroll >= chat.users.length - L.band.visible));
  // A pressed button shows its held cel only while the pointer is still over it (D2Win's button draw); leaving it shows the up cel, and the release there activates nothing.
  const held = (b) => { if (pressed !== b.id) return false; const r = box(b); return pointer[0] >= r.x && pointer[0] < r.x + r.w && pointer[1] >= r.y && pointer[1] < r.y + r.h; };
  const drawButton = (b) => {
    const off = disabled(b), down = held(b) || toggled(b);
    const key = b.id.startsWith('difficulty-') && !allowed[Number(b.id.slice(11))] ? 'radioGrey' : b.art;
    drawCel(art(key), b.base + (down ? 1 : 0), b.x, b.y, off || dim(b) ? 1 : 5);
    if (!b.label) return;
    const label = text(b.label), lf = labelFont(b.h);
    let x = b.x + Math.max(Math.trunc((b.w - lf.measure(label)) / 2), 0), y = b.y + labelRise(b.h) - Math.trunc((b.h - lf.lineHeight) / 2);
    if (held(b) || (['create', 'join', 'channel', 'ladder'].includes(b.id) && toggled(b))) { x -= 2; y += 2; }
    lf.drawAnchored(sctx, label, x, y);
  };
  // A wide button is two cels side by side (frames base and base + 1; the held pair is base + 2 and base + 3, as the other screens draw
  // widebuttonblank), its label centred over both and 2 px left and down while held.
  const drawWide = (b) => {
    const c = art(b.art), down = held(b) ? 2 : 0, w = c.dc6.frames[0].width;
    drawCel(c, b.base + down, b.x, b.y); drawCel(c, b.base + down + 1, b.x + w, b.y);
    const label = text(b.label), lf = labelFont(b.h);
    let x = b.x + Math.max(Math.trunc((b.w - lf.measure(label)) / 2), 0), y = b.y + labelRise(b.h) - Math.trunc((b.h - lf.lineHeight) / 2);
    if (down) { x -= 2; y += 2; }
    lf.drawAnchored(sctx, label, x, y);
  };
  // A D2Win text box: lines from one font height under its top (plus the inset), one font height apart.
  const drawTextBox = (t, lines) => {
    const insetX = t.insetX ?? 0, width = t.w - 2 * insetX;
    let y = t.y - t.h + (t.insetY ?? 0);
    let first = true;
    for (const { text: value, colour = 0 } of lines) {
      const f = font(t.font ?? 'font16', colour);
      for (const line of wrapSpaces(value, width, (s) => f.measure(s))) {
        y += first ? f.lineHeight : t.pitch ?? f.lineHeight; first = false;
        if (y > t.y) return;
        const tw = f.measure(line);
        const x = t.right ? t.x - tw : t.centred ? t.x + insetX + Math.max(Math.trunc((width - tw) / 2), 0) : t.x + insetX;
        f.drawAnchored(sctx, line, x, y);
      }
    }
  };
  // An edit box: its text at (x, bottom - h + 16), the focused one's "_" caret while GetTickCount / 250 is odd.
  // On the panels the caret is drawn after every control (`late` holds it until the panel's end).
  const late = [];
  const drawEdit = (e, value, focused, deferCaret = false) => {
    const f = font('formal10'), shown = e.password ? '*'.repeat(value.length) : value, y = e.y - e.h + 16;
    const body = () => f.drawAnchored(sctx, shown, e.x, y);
    const caret = () => f.drawAnchored(sctx, '_', e.x + f.measure(shown), y);
    if (deferCaret && shown) late.push(body); else body();
    if (focused && Math.floor(clock() / 250) % 2 === 1) { if (deferCaret) late.push(caret); else caret(); }
  };
  const flushCarets = () => { for (const c of late.splice(0)) c(); };
  // The chat log: each line's `<name> ` in gold, the rest in its colour; lines one pitch apart from the box top.
  const drawChatLog = (t, lines, first = null) => {
    chatBar = null;
    const f0 = font(t.font), pitch = t.pitch ?? f0.lineHeight, rows = Math.floor(t.h / pitch), out = [];
    for (const l of lines) {
      const f = font(t.font, l.colour ?? 0);
      for (const [i, part] of wrapSpaces(l.text, t.w, (x) => f.measure(x)).entries()) out.push({ part, l, first: i === 0 });
    }
    // More lines than rows: the scroll track (end caps, 10-pixel body cels, the thumb at the newest line).
    if (t.scrollbar && out.length > rows) {
      const S = t.scrollbar, bar = art('scrollbar');
      drawCel(bar, 1, S.x, S.bottom);
      for (let sy = S.bottom - 10; sy > S.top; sy -= 10) drawCel(bar, 5, S.x, sy);
      const max = out.length - rows, at = first === null ? max : Math.min(first, max);
      chatBar = { max, at };
      drawCel(bar, 0, S.x, S.top); drawCel(bar, 4, S.x, S.top + 10 + Math.round((S.bottom - 20 - S.top) * at / max));
    }
    let y = t.y - t.h;
    const from = first === null ? Math.max(0, out.length - rows) : Math.min(first, out.length);
    for (const { part, l, first } of out.slice(from, from + rows)) {
      const f = font(t.font, l.colour ?? 0);
      let x = t.x;
      if (l.name && first && part.startsWith(l.name)) {
        const nf = font(t.font, l.nameColour); nf.drawAnchored(sctx, l.name, x, y + f0.lineHeight); x += nf.measure(l.name);
        f.drawAnchored(sctx, part.slice(l.name.length), x, y + f0.lineHeight);
      } else f.drawAnchored(sctx, part, x, y + f0.lineHeight);
      y += pitch;
    }
  };
  // Portrait token by class (the class byte is statstring byte 13, less one: D2Multi+0xF340). The classes retail
  // has art for come first; an out-of-range class draws the tokens the retail capture shows for one (RO, O2 for 11).
  const portraitToken = (stat) => {
    const b = typeof stat === 'string' ? Uint8Array.from(stat, (c) => c.charCodeAt(0) & 255) : stat;
    const tail = b.indexOf(0x2c, b.indexOf(0x2c) + 1) + 1, c = tail > 0 && b[tail + 2] ? (b[tail + 13] ?? 0) - 1 : -1;
    return ['AM', 'SO', 'NE', 'PA', 'BA', 'DZ', 'AI'][c] ?? (c === 11 ? 'O2' : 'RO');
  };
  const PORTRAIT_LAYERS = { AM: 8, SO: 8, NE: 9, PA: 8, BA: 8, DZ: 8, AI: 10 };
  // The channel user's event flags (D2Multi+0xF340): operator, then 0x08, 0x01, 0x04 and 0x101000 each swap the class
  // portrait for their own token, and the list keeps them in that order ahead of everyone else (D2Multi+0xF750's insert).
  const flagToken = (fl) => (fl & 2 ? 'O7' : fl & 8 ? 'O8' : fl & 1 ? 'O5' : fl & 4 ? 'TC' : fl & 0x101000 ? 'O6' : null);
  const flagRank = (fl) => (fl & 2 ? 4 : fl & 8 ? 3 : fl & 1 ? 2 : fl & 4 ? 1 : 0);
  // Before ENTER CHAT the character's own portrait animates beside the details, one frame per 500 ms, started a quarter second before the fire logo's clock.
  const drawOwnPortrait = () => {
    const token = ['AM', 'SO', 'NE', 'PA', 'BA', 'DZ', 'AI'][character?.charClass];
    if (!token) return;
    const frame = Math.floor(Math.max(0, clock() - created + 250) / 500) % PORTRAIT_ANIM[token][0];
    for (let k = 0; k < PORTRAIT_LAYERS[token]; k++) recordDcc(token, frame, 66, L.band.portraitY);
  };
  const drawBand = () => {
    const B = L.band, f = font(B.font), shown = chat.users.slice(chat.scroll, chat.scroll + B.visible);
    // Flags only mean something on a well-formed character (class in range); a header the capture left as 0xFF bytes carries none.
    const info = shown.map((u) => { const p = parseStatstring(u.stat); return p.classId >= 0 && p.classId < 7 ? p : { dead: false, hardcore: false }; });
    // A character with both the 0x04 and 0x08 flag bits is the single RH cel, translucent; the rest draw their class token once per animation layer.
    // Retail draws the slots newest arrival first, whatever order the flags sorted them into.
    const order = shown.map((_, i) => i).sort((a, b) => (shown[b].seq ?? b) - (shown[a].seq ?? a));
    for (const i of order) {
      const x = B.x + 30 + B.cell * i, fl = shown[i].flags ?? 0, ft = flagToken(fl);
      const dead = !ft && info[i].dead && info[i].hardcore, token = ft ?? (dead ? 'RH' : portraitToken(shown[i].stat));
      // The picked user's ring: drawn under the portrait, one frame per 250 ms of the clock, five pixels right of and five above it.
      if (shown[i].name === chat.picked) {
        const c = art('selectAura'), n = c.dc6.frames.length, k = Math.floor(clock() / 250) % n, fr = c.dc6.frames[k];
        const left = x + 5 + fr.offsetX, top = B.portraitY - 5 + fr.offsetY - fr.height + 1;
        logoBlend(sctx, fr, left, top, 3); recordDc6(c.path, k, fr, left, top, { op: 'cel', mode: 3 });
      }
      const frame = portraitFrame ? portraitFrame(i) : portraitFrameAt(token, clock() - (shown[i].since ?? 0));
      if (dead) recordDcc('RH', frame, x, B.portraitY, { mode: 1 });
      else for (let k = 0; k < (PORTRAIT_LAYERS[token] ?? 1); k++) recordDcc(token, frame, x, B.portraitY);
      // The figure itself: the class's layers dressed from the user's statstring (nothing is painted by the records above).
      if (figure && !ft) {
        let fig = null; try { fig = figure(shown[i].stat, clock() - (shown[i].since ?? 0)); } catch { /* art the player's assets lack: the name still draws */ }
        for (const l of fig?.layers ?? []) sctx.drawImage(l.canvas, x + l.left, B.portraitY + l.top);
      }
      if (fl & 0x20) { const c = art('squelch'), ak = auraFrame ? auraFrame() : Math.floor(clock() / 250) % c.dc6.frames.length, fr = c.dc6.frames[ak]; if (fr) { const left = x + fr.offsetX, top = B.portraitY + 10 + fr.offsetY - fr.height + 1;
        logoBlend(sctx, fr, left, top, 3); recordDc6(c.path, ak, fr, left, top, { op: 'cel', mode: 3 }); } }
    }
    const tipCell = mode === 'chat' && !busy && !notice && !joining ? bandCell(pointer[0], pointer[1]) : -1;
    for (const i of order) {
      const fl = shown[i].flags ?? 0, nf = font(B.font), y = B.nameY[(chat.scroll + i) % 2], cx = B.x + 30 + B.cell * i;
      const rec = recOf(shown[i]), style = bandNameStyle(fl, info[i].hardcore && rec.hardcore);
      const n = bandNameText(shown[i].name, rec.tag);
      const x = cx - Math.trunc(nf.measure(n) / 2);
      for (const [colour, dx, dy] of style.layers) font(B.font, colour).drawAnchored(sctx, n, x + dx, y + dy);
      // The hovered user's tooltip comes right after their name, before the names of the users drawn after them.
      if (i === tipCell) drawUserTip(i);
    }
  };
  // Hovering a user in the band: their expansion, level, hardcore, realm and account, centred on the cell, the last line on a fixed row and the rest above it.
  const TIP_RIGHT = 797;
  // The retail record behind a channel user: advanced by every update, so a field an update does not reach keeps its old value.
  const recOf = (u) => (u.rec ??= advanceBandRecord(null, u.stat, u.flags ?? 0));
  const drawUserTip = (i) => {
    const B = L.band;
    const u = chat.users[chat.scroll + i];
    const lines = bandTipLines(recOf(u), `*${accountName(u.name)}`, (n) => text('levelN').replace('%d', String(n)), text('hardcore'));
    const f = font('font16'), cx = B.x + 30 + B.cell * i, widest = Math.max(...lines.map((l) => f.measure(l)));
    // The block is centred on the cell and held 3 pixels in from the screen's left edge.
    const shift = Math.max(0, 3 - (cx - Math.trunc(widest / 2))) + Math.min(0, TIP_RIGHT - (cx + widest - Math.trunc(widest / 2)));
    lines.forEach((l, k) => f.drawAnchored(sctx, l, cx - Math.trunc(f.measure(l) / 2) + shift, B.tipBottom - B.tipPitch * (lines.length - 1 - k)));
  };
  // The two logos are controls 34 and 37 around the panel (control 35): the first is drawn before it, the second after.
  const drawLogos = (k) => {
    const black = art('logoBlack'), fire = art('logoFire');
    for (const { x, y } of [L.logos[k]]) {
      const n = fire.dc6.frames.length, i = Math.floor(Math.max(0, clock() - created) / 40) % (n - 1), f = fire.dc6.frames[i];
      drawCel(black, i, x, y);
      const left = x + f.offsetX, top = y + f.offsetY - f.height + 1;
      logoBlend(sctx, f, left, top, 3);
      recordDc6(fire.path, i, f, left, top, { op: 'cel', mode: 3 });
    }
  };
  // The panels' own controls come after the right-hand panel and the second logo. Titles and labels are gold; with no
  // realm connection (`offline`) the player-count and level-difference rows are disabled: grey labels, no edit boxes,
  // the grey boxes and translucent arrows.
  const drawCreate = () => {
    const C = L.createGame, [name, pass, desc, players, levelDiff, levels] = C.labels;
    drawPanel('createPanel', L.panel.x, L.panel.y);
    drawTextBox(C.title, [{ text: text(C.title.text), colour: 4 }]);
    for (const t of [name, pass, desc]) drawTextBox({ ...t, font: 'font16' }, [{ text: text(t.text), colour: 4 }]);
    drawButton(C.ok); drawButton(C.cancel);
    const [nameEdit, passEdit, descEdit, playersEdit, levelEdit] = C.edits;
    for (const e of [nameEdit, passEdit, descEdit]) drawEdit(e, form[e.id], form.focus === e.id, true);
    flushCarets();   // retail draws the name / password / description carets as those edits finish, before the players label
    if (offline) {
      drawTextBox({ ...players, font: 'font16' }, [{ text: text(players.text), colour: 5 }]);
      drawTextBox({ ...levelDiff, font: 'font16' }, [{ text: text(levelDiff.text), colour: 5 }]);
      drawTextBox({ ...levels, font: 'font16' }, [{ text: text(levels.text), colour: 5 }]);
      for (const g of C.greyBoxes) { drawCel(art('greyBox'), 0, g.x, g.y); drawCel(art('numberGreyArrows'), 0, g.x + 34, g.y, 1); }
    } else {
      drawTextBox({ ...players, font: 'font16' }, [{ text: text(players.text) }]);
      drawTextBox({ ...levelDiff, font: 'font16' }, [{ text: text(levelDiff.text), colour: form.levelLimit ? 0 : 5 }]);
      drawTextBox({ ...levels, font: 'font16' }, [{ text: text(levels.text), colour: form.levelLimit ? 0 : 5 }]);
      for (const g of C.greyBoxes) drawCel(art('greyBox'), 0, g.x, g.y);
      drawEdit(playersEdit, form.maxPlayers, form.focus === 'maxPlayers', true);
      if (form.levelLimit) drawEdit(levelEdit, form.levelDifference, form.focus === 'levelDifference', true);
      drawButton(C.levelBox);
      for (const a of C.playerArrows) drawButton(a);
      if (form.levelLimit) for (const a of C.levelArrows) drawButton(a);
      else drawCel(art('numberGreyArrows'), 0, C.levelGreyArrows.x, C.levelGreyArrows.y, 1);
    }
    for (let i = C.radios.length - 1; i >= 0; i--) {
      const r = C.radios[i], label = () => drawTextBox({ ...C.radioLabels[i], w: 150, h: 12, font: 'font16' }, [{ text: text(C.radioLabels[i].text), colour: allowed[i] ? 4 : 5 }]);
      if (form.difficulty === i) { label(); drawButton(r); } else { drawButton(r); label(); }
    }
    flushCarets();
  };
  const drawJoin = () => {
    const J = L.joinGame;
    drawPanel('joinPanel', L.panel.x, L.panel.y);
    drawTextBox(J.title, [{ text: text(J.title.text), colour: 4 }]);
    for (const t of J.labels) drawTextBox({ ...t, font: 'font16' }, [{ text: text(t.text), colour: 4 }]);
    for (const e of J.edits) drawEdit(e, form[e.id], form.focus === e.id, true);
    drawButton(J.ok); drawButton(J.cancel);
    flushCarets();
    // The game list control is created after the edit boxes and buttons, so its rows draw after the carets: one name per row, 19 apart.
    const first = Math.min(listFirst, Math.max(0, games.length - 1));
    if (games.length > J.list.rows) {
      const S = J.list.scrollbar, art2 = art('joinScrollbar'), max = games.length - J.list.rows;
      drawCel(art2, bar.down ? 3 : 1, S.x, S.bottom);
      for (let sy = S.bottom - 10; sy > S.top; sy -= 10) drawCel(art2, 5, S.x, sy);
      drawCel(art2, bar.up ? 2 : 0, S.x, S.top); drawCel(art2, 4, S.x, S.top + 10 + Math.trunc((S.height - 30) * Math.min(first, max) / max));
    }
    games.slice(first, first + J.list.rows).forEach((g, i) => font(J.list.font, first + i === selected ? J.list.selectedColour : J.list.colour)
      .drawAnchored(sctx, g.name, J.list.x, J.list.firstBaseline + i * J.list.pitch));
    if (desc) {
      const D = J.list.desc, df = font(D.font, D.colour);
      wrapSpaces(desc, D.w, (t) => df.measure(t)).slice(0, D.lines).forEach((line, i) =>
        df.drawAnchored(sctx, line, D.x + Math.max(Math.trunc((D.w - df.measure(line)) / 2), 0), D.firstBaseline + i * D.pitch));
    }
  };
  const drawHelp = () => {
    const H = L.helpPanel, S = H.list.scrollbar, rows = helpLines(), art2 = art('joinScrollbar');
    drawPanel('helpPanel', L.panel.x, L.panel.y);
    drawTextBox(H.title, [{ text: text(H.title.text), colour: 4 }]);
    const max = Math.max(0, rows.length - H.list.rows), first = helpFirst;
    if (max > 0) {
      drawCel(art2, 1, S.x, S.bottom);
      for (let sy = S.bottom - 10; sy > S.top; sy -= 10) drawCel(art2, 5, S.x, sy);
      drawCel(art2, 0, S.x, S.top); drawCel(art2, 4, S.x, S.top + 10 + Math.trunc((S.bottom - 20 - S.top) * Math.min(first, max) / max));
    }
    const f = font(H.list.font, H.list.colour);
    rows.slice(first, first + H.list.rows).forEach((line, i) => { if (line) f.drawAnchored(sctx, line, H.list.x, H.list.firstBaseline + i * H.list.pitch); });
    drawButton(H.ok);
    flushCarets();
  };
  const drawChannel = () => {
    const C = L.channelPanel;
    drawPanel('channelPanel', L.panel.x, L.panel.y);
    drawTextBox(C.title, [{ text: text(C.title.text), colour: 4 }]);
    for (const t of C.labels) drawTextBox({ ...t, font: 'font16' }, [{ text: text(t.text), colour: 4 }]);
    drawButton(C.ok); drawButton(C.cancel);
    drawEdit(C.edit, form.channel, form.focus === 'channel', true);
    flushCarets();
    const max = Math.max(0, channels.length - C.list.rows), first = Math.min(chanFirst, max);
    if (max > 0) {
      const S = C.list.scrollbar, art2 = art('joinScrollbar');
      drawCel(art2, bar.down ? 3 : 1, S.x, S.bottom);
      for (let sy = S.bottom - 10; sy > S.top; sy -= 10) drawCel(art2, 5, S.x, sy);
      drawCel(art2, bar.up ? 2 : 0, S.x, S.top); drawCel(art2, 4, S.x, S.top + 10 + Math.trunc((S.height - 30) * first / max));
    }
    channels.slice(first, first + C.list.rows).forEach((name, i) => font(C.list.font, first + i === chanSel ? C.list.selColour : C.list.colour).drawAnchored(sctx, name, C.list.x, C.list.firstBaseline + i * C.list.pitch));
  };
  const drawLadder = () => {
    const D = L.ladderPanel;
    drawPanel('ladderPanel', L.panel.x, L.panel.y);
    drawTextBox(D.title, [{ text: text(D.title.text), colour: 4 }]);
    for (const b of D.buttons) drawWide(b);
    drawButton(D.cancel);
  };
  const drawLadderList = () => {
    const H = L.ladderList, S = H.list.scrollbar, rows = ladder.rows ?? [], art2 = art('joinScrollbar');
    drawPanel('helpPanel', L.panel.x, L.panel.y);
    drawTextBox(H.title, [{ text: text(L.ladderPanel.buttons.find((b) => b.kind === ladder.kind).label), colour: 4 }]);
    const max = Math.max(0, rows.length - H.list.rows), first = Math.min(ladderFirst, max);
    if (max > 0) {
      drawCel(art2, 1, S.x, S.bottom);
      for (let sy = S.bottom - 10; sy > S.top; sy -= 10) drawCel(art2, 5, S.x, sy);
      drawCel(art2, 0, S.x, S.top); drawCel(art2, 4, S.x, S.top + 10 + Math.trunc((S.bottom - 20 - S.top) * first / max));
    }
    const f = font(H.list.font, H.list.colour), c = H.list.cols, x = H.list.x;
    const right = (t, edge, y) => f.drawAnchored(sctx, t, x + edge - f.measure(t), y);
    if (ladder.rows === null) f.drawAnchored(sctx, text('pleaseWait'), x, H.list.firstBaseline);
    else if (!rows.length) wrapSpaces(LADDER_EMPTY_TEXT, H.list.w, (t) => f.measure(t)).forEach((line, i) => f.drawAnchored(sctx, line, x, H.list.firstBaseline + i * H.list.pitch));
    rows.slice(first, first + H.list.rows).forEach((r, i) => {
      const y = H.list.firstBaseline + i * H.list.pitch;
      right(String(r.rank), c.rank, y); f.drawAnchored(sctx, r.name, x + c.name, y); right(String(r.level), c.level, y); right(String(r.experience), c.experience, y);
    });
    drawButton(H.back); drawButton(H.ok);
  };
  const drawJoining = () => {
    const N = L.notice, J = L.joining, p = art('popup');
    drawCel(p, 0, N.x, N.y); drawCel(p, 1, N.x + p.dc6.frames[0].width, N.y);
    const b = J.cancel; drawCel(art(b.art), b.base, b.x, b.y, 5);
    const label = text(b.label), lf = labelFont(b.h);
    lf.drawAnchored(sctx, label, b.x + Math.max(Math.trunc((b.w - lf.measure(label)) / 2), 0), b.y + labelRise(b.h) - Math.trunc((b.h - lf.lineHeight) / 2));
    const f = font(J.text.font, 0), t = text(J.text.string);
    f.drawAnchored(sctx, t, J.text.x - Math.trunc((f.measure(t) + 2) / 2), J.text.y);
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

  // The scrollbar on show (the join list's, else the chat log's) with where its mouse area is and how to set it. The thumb sits at
  // S.top + 10 + round((S.bottom - 20 - S.top) * pos / max), as drawn; a press or drag at a row puts it there.
  const scrollbar = () => {
    if (busy || notice) return null;
    const J = L.joinGame.list;
    if (panel === 'join' && games.length > J.rows) {
      const max = games.length - J.rows;
      return { id: 'games', S: J.scrollbar, art: 'joinScrollbar', max, at: Math.min(listFirst, max), area: { x: J.x, top: J.y - J.h, bottom: J.y }, set: (p) => { listFirst = p; } };
    }
    if (panel === 'help') {
      const H = L.helpPanel, max = Math.max(0, helpLines().length - H.list.rows);
      return max ? { id: 'help', S: H.list.scrollbar, art: 'joinScrollbar', max, at: Math.min(helpFirst, max), area: { x: H.list.x, top: H.list.firstBaseline - 12, bottom: H.list.firstBaseline - 12 + H.list.rows * H.list.pitch }, set: (p) => { helpFirst = p; } } : null;
    }
    if (panel === 'ladder-list') {
      const H = L.ladderList, max = Math.max(0, (ladder.rows?.length ?? 0) - H.list.rows);
      return max ? { id: 'ladder-list', S: H.list.scrollbar, art: 'joinScrollbar', max, at: Math.min(ladderFirst, max), area: { x: H.list.x, top: H.list.firstBaseline - 12, bottom: H.list.firstBaseline - 12 + H.list.rows * H.list.pitch }, set: (p) => { ladderFirst = p; } } : null;
    }
    if (panel === 'channel') {
      const CL = L.channelPanel.list, max = Math.max(0, channels.length - CL.rows);
      return max ? { id: 'channels', S: CL.scrollbar, art: 'joinScrollbar', max, at: Math.min(chanFirst, max), area: { x: CL.area.x, top: CL.area.top, bottom: CL.area.top + CL.area.h }, set: (p) => { chanFirst = p; } } : null;
    }
    const C = L.chatLog;
    if (!panel && mode === 'chat' && chatBar) {
      return { id: 'chat', S: C.scrollbar, art: 'scrollbar', max: chatBar.max, at: chatBar.at, area: { x: C.x, top: C.y - C.h, bottom: C.y }, set: (p) => { chat.first = p >= chatBar.max ? null : p; } };
    }
    return null;
  };
  const barWidth = (b) => art(b.art).dc6.frames[4]?.width ?? 10;
  const overBar = (b, sx, sy) => sx >= b.S.x && sx < b.S.x + barWidth(b) && sy >= b.S.top && sy <= b.S.bottom;

  const dragBar = (b, sy) => b.set(Math.min(b.max, Math.max(0, Math.round((sy - b.S.top - 10) * b.max / (b.S.bottom - 20 - b.S.top)))));

  const typeInto = (key, value, limit, digits) => {
    if (form[key].length >= limit - 1) return;   // D2Win's edit box refuses a character at strlen >= limit - 1 (D2Win+0x15450), so a limit of 16 holds 15
    if (digits && !/[0-9]/.test(value)) return;
    if (key === 'name' && (panel === 'create' || panel === 'join') && !/[0-9A-Za-z '_-]/.test(value)) return;
    form[key] += value;
    if (key === 'maxPlayers') clampPlayers();
  };

  return {
    get mode() { return mode; },
    // The channel names the chat connection reported, in its order.
    // The help list's scroll (the list call at D2Win 0x88A0): the first row on show.
    ladderState() { return { panel, kind: ladder.kind, rows: ladder.rows }; },
    scrollHelp(pos) { helpFirst = Math.max(0, pos); },
    setChannels(list) { channels = list.map(String); chanFirst = 0; },
    setGames(list) {
      // The refresh (D2Multi 0x11F80) rebuilds the list and empties the description. The pick is looked up again by its name: found, it follows
      // the game to its new row; gone, the same row number stays picked (now another game). The scroll position is kept only when a row was picked.
      const kept = selected >= 0 ? games[selected]?.name ?? null : null;
      const wasAtEnd = games.length > 9 && listFirst === games.length - 9;
      games = list.map((g) => ({ name: String(g.name ?? ''), password: !!g.password, difficulty: g.difficulty ?? 0, description: String(g.description ?? ''), status: g.status ?? 0 }));
      const found = kept === null ? -1 : games.findIndex((g) => g.name === kept);
      if (found >= 0) selected = found;
      desc = '';
      if (selected < 0 || kept === null) listFirst = 0;
      else if (wasAtEnd) listFirst = games.length;   // a list scrolled to its end before the refresh is restored past the end (retail shows the scrollbar at its maximum)
    },
    // The game list's scroll (D2Win 0x88A0, the call the scrollbar makes): the first row on show, only floored at 0.
    scrollGames(pos) { listFirst = Math.max(0, pos); },
    selectedGame() { return selected >= 0 && games[selected] ? { ...games[selected] } : null; },
    // Time-driven work: the join panel's refresh. draw() runs it too.
    tick() { if (panel === 'join' && clock() - timerAt >= GAME_REFRESH_MS) { timerAt = clock(); refreshGames(); } },
    get panel() { return panel; },
    get busy() { return busy; },
    get evidence() {
      return { mode, panel, notice, busy, character: { name: character.name ?? '', charClass: character.charClass ?? null, level: character.level ?? null },
        info: characterInfoLines(character, string, charTitle).map((l) => l.text), difficulties: allowed,
        form: { ...form, password: undefined, passwordLength: form.password.length }, chat: chat.lines.map((l) => l.text),
        boxes: controls().map((b) => ({ id: b.id, ...box(b), disabled: disabled(b) })), cursor: pointer };
    },
    // A failed create/join that kept the session: retail's notice over the lobby.
    // A BNCS chat event as D2Multi's table (D2Multi+0x19BC8) takes it: 1 / 2 user in channel / joins, 3 leaves,
    // 5 talk, 7 channel. True when the screen drew it (the rest go through chatLine).
    chatEvent(e) {
      const say = (line) => { chat.first = null; chat.lines.push(line); if (chat.lines.length > 200) chat.lines.shift(); };
      const plain = (n) => n.replace(/^\*/, '');
      const shown = displayName(e.username ?? ''), acct = accountName(e.username ?? '');
      switch (e.event) {
        case 0x07: chat.channel = e.text; chat.users = []; chat.scroll = 0; say({ text: `You have joined channel: ${e.text}`, colour: 5 }); return true;
        case 0x01: case 0x02: {
          const user = { name: e.username, stat: e.raw ?? e.text ?? '', flags: e.flags ?? 0, since: clock(), seq: chat.seq = (chat.seq ?? 0) + 1 }, at = chat.users.findIndex((u) => plain(u.name) === plain(user.name));
          if (at >= 0) chat.users[at] = { ...user, since: chat.users[at].since ?? user.since, seq: chat.users[at].seq, rec: advanceBandRecord(recOf(chat.users[at]), user.stat, user.flags) };
          else { let k = chat.users.length; while (k > 0 && flagRank(chat.users[k - 1].flags ?? 0) < flagRank(user.flags)) k--; chat.users.splice(k, 0, user); }
          scrollUsers(0);
          if (e.event === 0x02 && chat.notify) say({ text: `${shown} has joined the channel.`, colour: 5 });
          return true;
        }
        case 0x03: chat.users = chat.users.filter((u) => plain(u.name) !== plain(e.username)); scrollUsers(0); if (chat.notify) say({ text: `${shown} has left the channel.`, colour: 5 }); return true;
        case 0x05: {
          if (chat.selfSaid?.length && shown.toLowerCase() === String(character.name ?? '').toLowerCase()) { const at = chat.selfSaid.indexOf(e.text); if (at >= 0) { chat.selfSaid.splice(0, at + 1); return true; } }
          const fl = chat.users.find((u) => u.name === e.username)?.flags ?? 0; say({ text: `<${shown}> ${e.text}`, name: `<${shown}> `, colour: fl & 2 ? 0 : fl & 9 ? 3 : 0, nameColour: fl & 2 ? 0 : fl & 9 ? 3 : 4 }); return true; }
        case 0x04: chat.replyTo = e.username.replace(/^.*\*/, '') || e.username; say({ text: e.username.startsWith('*') && shown === e.username ? `${shown} whispers: ${e.text}` : `${shown} (*${acct}) whispers: ${e.text}`, colour: 2 }); return true;
        case 0x0A: say({ text: `You whisper to ${shown}: ${e.text}`, colour: 2 }); return true;
        case 0x0D: say({ text: `The channel "${e.text}" is full. If you are having problems using Battle.net commands, please click the Help button.`, colour: 1 }); return true;
        case 0x09: { const u = chat.users.find((x) => x.name === e.username); if (u) { const prev = recOf(u); u.flags = e.flags ?? 0; if (e.raw?.length) u.stat = e.raw; u.rec = advanceBandRecord(prev, u.stat, u.flags); } return true; }
        case 0x0E: return true;
        case 0x0F: say({ text: `The channel "${e.text}" is restricted. If you are having problems using Battle.net commands, please click the Help button.`, colour: 1 }); return true;
        case 0x06: case 0x12: case 0x15: case 0x16: say({ text: e.text, colour: 3 }); return true;
        case 0x13: say({ text: e.text, colour: 1 }); return true;
        case 0x17: say({ text: `*${shown} ${e.text}*`, colour: 5 }); return true;
        default: return false;
      }
    },
    // The list-scroll call (D2Win 0x88A0): the first visible row, not clamped to the last page, only to the row count.
    scrollChat(pos) { chat.first = Math.max(0, pos); },
    chatLine(line) { chat.first = null; chat.lines.push(line); if (chat.lines.length > 200) chat.lines.shift(); },
    chatLive(on) { chat.live = Boolean(on); },
    // The chat state a retail capture set directly (the reply target); tests only.
    get chatState() { return chat; },
    fail(error) { notice = { body: gameErrorText(error, string, character) }; busy = false; },
    typeKey(code, key) {
      if (busy) return true;
      const enter = code === 'Enter' || code === 'NumpadEnter';
      if (notice) { if (enter || code === 'Escape') notice = null; return true; }
      // HELP and LADDER have no edit box, and a press on the chat line moves the focus to it: the keys are the chat line's.
      if (panel && editsFor().length && form.focus !== 'chat') {
        const edits = editsFor(), ids = edits.map((e) => e.id);
        if (code === 'Tab') { form.focus = ids[(ids.indexOf(form.focus) + 1) % ids.length]; return true; }
        // Every edit's Enter callback is the panel's OK (D2Multi+0x142C0 create, +0x14100 join).
        if (enter) { if (panel === 'create') createGame(); else if (panel === 'channel') joinChannel(); else if (panel === 'join') joinGame(); return true; }
        const e = edits.find((x) => x.id === form.focus); if (!e) return false;
        if (code === 'Backspace') { form[e.id] = form[e.id].slice(0, -1); if (e.id === 'maxPlayers') clampPlayers(); return true; }
        if (!key || key.length !== 1 || key < ' ' || key > '~') return false;
        typeInto(e.id, key, e.limit, e.digits);
        return true;
      }
      // ENTER CHAT's key is Enter (descriptor D2Multi+0x1A060, key 13).
      if (mode === 'lobby') { if (enter) { mode = 'chat'; onEnterChat(); return true; } return false; }
      if (enter) { sendChat(false, true); reeval(); return true; }
      // The chat edit's key filter (D2Multi+0xB760) answers Tab, not Up: it steps forward through the slash-line history, wrapping to the newest.
      if (code === 'Tab') { if (chat.history.length) { chat.recall = (chat.recall + 1) % chat.history.length; chat.input = chat.history[chat.recall]; } reeval(); return true; }
      if (code === 'Backspace') { chat.input = chat.input.slice(0, -1); reeval(); return true; }
      if (!key || key.length !== 1 || key < ' ') return false;
      if (chat.input.length < L.chatEdit.limit - 1) chat.input += key;
      reeval();
      return true;
    },
    // Touch typing: the text the keyboard edits (the chat line, or the focused CREATE / JOIN edit box),
    // null when nothing takes text; `editAt` is the chat line's own hit test (the panels' edit boxes
    // are ordinary controls whose press already focuses them).
    editValue() {
      if (busy || notice) return null;
      if (panel && editsFor().length && form.focus !== 'chat') { const e = editsFor().find((x) => x.id === form.focus); return e ? String(form[e.id]) : null; }
      return mode === 'chat' ? chat.input : null;
    },
    editAt(x, y, w, h) {
      if (busy || notice || (panel && editsFor().length) || mode !== 'chat') return false;
      const [sx, sy] = at(x, y, w, h), r = box(L.chatEdit);
      return sx >= r.x && sx < r.x + r.w && sy >= r.y && sy < r.y + r.h;
    },
    // The scrollbar's mouse (the web's: a press or drag on the track sets the thumb, the wheel moves a row): the token of the bar a
    // press at this point grabbed, else null; `scrollDrag` follows the pointer; `wheel` is true when the point is over a list that scrolls.
    scrollPress(x, y, w, h) {
      if (panel === "join") return null;
      if (panel === "channel") return barPress(x, y, w, h) ? "bar" : null;
      const b = scrollbar(), [sx, sy] = at(x, y, w, h); if (!b || !overBar(b, sx, sy)) return null; dragBar(b, sy); return b.id;
    },
    scrollDrag(token, y, w, h) {
      if (token === "bar") { if (!bar.held) return false; const b = barOf(); if (b && bar.grab) barDrag(b, at(0, y, w, h)[1]); return true; }
      const b = scrollbar(); if (!b || b.id !== token) return false; dragBar(b, at(0, y, w, h)[1]); return true;
    },
    // The button's release: the thumb lets go, the arrow that was pressed stays drawn pressed until the next press (retail keeps it).
    scrollEnd() { bar.grab = bar.held = false; },
    wheel(x, y, w, h, rows) {
      if (panel === "join") return false; // retail ignores the wheel in the join list
      const b = scrollbar(), [sx, sy] = at(x, y, w, h);
      if (!b || !(overBar(b, sx, sy) || (sx >= b.area.x && sx < b.S.x + barWidth(b) && sy >= b.area.top && sy < b.area.bottom))) return false;
      b.set(Math.min(b.max, Math.max(0, b.at + rows))); return true;
    },
    targets(w, h) { const v = view(w, h); return Object.fromEntries(controls().map((b) => { const r = box(b); return [b.id, [Math.round(v.ox + (r.x + r.w / 2) * v.scale), Math.round(v.oy + (r.y + r.h / 2) * v.scale)]]; })); },
    hover(x, y, w, h) {
      const [sx, sy] = at(x, y, w, h); pointer = [Math.trunc(sx), Math.trunc(sy)];
      const b = barOf(); if (bar.grab && bar.held && b) barDrag(b, sy);
    },
    press(x, y, w, h) {
      { const [px, py] = at(x, y, w, h); pointer = [Math.trunc(px), Math.trunc(py)]; }
      if (barPress(x, y, w, h)) { pressed = null; return 'scrollbar'; }
      if (inList(x, y, w, h)) { pickRow(listRow(x, y, w, h)); pressed = null; return 'list'; }
      if (inChanList(x, y, w, h)) { pickChannel(x, y, w, h); pressed = null; return 'list'; }
      if (joining) return null;
      if (mode === 'chat' && !busy && !notice) { const [sx, sy] = at(x, y, w, h), i = bandCell(sx, sy); if (i >= 0) { pressed = null; return `user:${chat.scroll + i}`; } }
      const b = hit(x, y, w, h); if (!b || disabled(b) || dim(b)) return null; 
      if (['create', 'join', 'channel', 'ladder'].includes(b.id)) {
        if (panel === b.id) return null;
        activate(b); latch = b.id; pressed = b.id; return `latched:${b.id}`;
      }
      pressed = b.id; return pressed; },
    release(token, x, y, w, h) {
      bar.grab = bar.held = false;
      const b = hit(x, y, w, h); pressed = null;
      if (typeof token === 'string' && token.startsWith('user:')) {
        const [sx, sy] = at(x, y, w, h), i = bandCell(sx, sy);
        if (i >= 0 && chat.scroll + i === Number(token.slice(5))) { clickUser(i); return true; }
        return false;
      }
      if (typeof token === 'string' && token.startsWith('latched:')) return true;
      if (b && b.id === token) { activate(b); return true; }
      return false;
    },
    pickUser(name) { chat.picked = name ?? null; reeval(); },
    activate(id) { const b = controls().find((x) => x.id === id); if (b) activate(b); return Boolean(b); },
    cancel() { pressed = null; bar.grab = bar.up = bar.down = bar.held = false; },
    // Escape: the notice's OK, the open panel's CANCEL, else QUIT (the lobby's escape button).
    escape() { if (busy) return; if (notice) notice = null; else if (panel) closePanel(); else onQuit(); },
    step() {},
    pinClock(ms, builtMs = 0) { pinned = ms; created = builtMs; },
    draw(out, w, h) {
      this.tick();
      const v = view(w, h);
      out.fillStyle = '#000'; out.fillRect(0, 0, w, h); out.imageSmoothingEnabled = false;
      sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600);
      const back = lobbyBg();
      drawBg(sctx, back.path, back.canvas); recordTiled(back.path, back, 0, 0);
      if (mode === 'chat') { for (const b of [L.send, L.whisper, L.chatHelp, L.squelch, L.unsquelch, L.emote]) drawButton(b); }
      for (const b of [L.create, L.channel, L.join, L.ladder, L.quit]) drawButton(b);
      if (mode === 'chat') {
        if (chat.channel) drawTextBox(L.newsTitle, [{ text: headerText(), colour: 3 }]);
        drawChatLog(L.chatLog, chat.lines, chat.first ?? null);
        drawEdit(L.chatEdit, chat.input, (!editsFor().length || form.focus === 'chat') && !notice);   // HELP and LADDER take no edit box, so the chat line keeps the caret
        drawCel(art('gem'), 0, L.gem.x, L.gem.y);
      } else {
        drawButton(L.enterChat); drawButton(L.help);
        drawTextBox(L.charInfo, characterInfoLines(character, string, charTitle));
        drawTextBox(L.newsTitle, [{ text: text('newsTitle'), colour: 3 }]);
      }
      drawLogos(0);
      drawPanel('panel', L.panel.x, L.panel.y);
      if (deadHardcore && !panel) drawTextBox(L.serverBox, [{ text: text('deadHardcore') }]);
      drawLogos(1);
      if (mode !== 'chat') drawOwnPortrait();
      if (panel === 'create') drawCreate();
      else if (panel === 'join' && !joining) drawJoin();
      else if (panel === 'help') drawHelp();
      else if (panel === 'channel') drawChannel();
      else if (panel === 'ladder') drawLadder();
      else if (panel === 'ladder-list') drawLadderList();
      if (mode === 'chat') { drawBand(); for (const b of L.arrows) drawButton(b); }
      if (joining) drawJoining();
      if (notice) drawNotice();
      const f = cursorDc6.frames[0];
      const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
      sctx.drawImage(cursorCanvas, left, top);
      recordDc6(FRONTEND.cursor, 0, f, left, top);
      present(out, scene, v, back.path);
    },
  };
}
