// Retail's Battle.net account login screen (1.13c D2Launch+0x19290), drawn the way retail draws it:
// the menu background and fire logo (D2Launch+0x1B310), two textbox2 edit frames, the Font16 labels
// and instructions, and the four D2Win buttons. The art, anchors, fonts and colours are read off
// retail's own frame of this screen (the headless FRONTEND capture, D2CAPTURE_FRONTEND_SCREEN=bnetlogin).
//
// The control rules are retail's, from the same function and its callbacks:
//   - the account box holds 16 characters, the password box 13 (D2Win edit-box limits set at +0x19498/+0x194f3);
//   - the account box refuses " # % * , / < > ? \ and anything outside 0x21..0x7e (D2Launch+0x126C0);
//   - LOG IN is enabled only while 1 <= account <= 50 and 1 <= password <= 13 characters
//     (D2Launch+0x122C0, called on every edit); disabled it draws 3WideButtonBlank frames 4/5.
// The endpoint is never typed: it is the region committed on the gateway list (bnet-servers.json).
//
// The screens the login screen's buttons open are retail's too, read off the same capture:
//   - CREATE NEW ACCOUNT (D2Launch+0x17780): account / password / verify, OK validates (D2Launch+0x19880)
//     and raises D2Launch+0x15090's notice box per rule, or the PLEASE READ screen (D2Launch+0x184D0);
//   - ACCOUNT SETTINGS (D2Launch+0x10844): the title, three explanations and three buttons;
//   - CHANGE PASSWORD (D2Launch+0x15F10, OK +0x1A030), GET NEW PASSWORD (+0x10A40, SEND +0x10F70) and
//     CHANGE EMAIL (+0x103E5, SEND +0x10D40): four, two and four labelled edit boxes, their rules and notices.
import { composeTiled, createBlender, createFont, decodeFontTbl, recordTiled, textColourPalette,
  LABEL_TINT, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';
import { drawBg, present } from './widebg.mjs';

const P = 'data\\global\\ui\\FrontEnd\\';
const F = 'data\\local\\font\\latin\\';
export const BNET_LOGIN_ASSETS = {
  textbox: P + 'textbox2.dc6',
  wideButton: FRONTEND.button,              // WideButtonBlank: CREATE NEW ACCOUNT, ACCOUNT SETTINGS
  loginButton: P + '3WideButtonBlank.dc6',  // LOG IN: normal f0/f1, down f2/f3, disabled f4/f5
  mediumButton: P + 'MediumButtonBlank.dc6', // EXIT, the message box's OK
  popup: P + 'PopUpWide.dc6',               // the message box frame, two cels
  noticePopup: P + 'PopUpOK.dc6',           // D2Launch+0x15090's notice box frame, two cels
  noticeButton: P + 'CancelButtonBlank.dc6', // its OK button
  wideTextbox: P + 'EditTextBoxWide.dc6',   // the email forms' edit frame, two cels
  readBackground: P + 'TermsofUseBckg.dc6',  // the PLEASE READ panel, six cels
  bigFont: F + 'font42.dc6', bigFontTbl: F + 'font42.tbl',
  tinyFont: F + 'fontridiculous.dc6', tinyFontTbl: F + 'fontridiculous.tbl',
  titleFont: F + 'font30.dc6', titleFontTbl: F + 'font30.tbl',
  bodyFont: F + 'font24.dc6', bodyFontTbl: F + 'font24.tbl',
  editFont: F + 'fontformal12.dc6', editFontTbl: F + 'fontformal12.tbl',
};

// D2Lang string-table indices (the control descriptors at D2Launch+0x21310..0x21670).
export const BNET_LOGIN_STRINGS = { account: 5224, password: 5225, info: 5205,
  create: 5221, settings: 11108, login: 5288, exit: 5101, ok: 5102, cancel: 5103,
  createInfo: 5223, verify: 10834,
  settingsTitle: 11109, settingsLines: [11110, 11111, 11112], changePassword: 11050, getPassword: 11113, changeEmail: 11115,
  readTitle: 5227, readBodies: [5228, 5229, 11127],
  // The three account forms (descriptors D2Launch+0x21310.. and +0x24730..): titles, labels, SEND.
  changePasswordTitle: 11051, currentPassword: 11052, newPassword: 11053, verifyNewPassword: 11054,
  getPasswordTitle: 11114, getPasswordInfo: 11103, email: 11095, send: 11062,
  changeEmailTitle: 11116, changeEmailInfo: 11120, oldEmail: 11117, newEmail: 11118, verifyNewEmail: 11119 };

// The account forms' notices: the change-email refusals (D2Launch+0x10D40), the get-password receipt
// (+0x10F70) and the connect failure every one of them raises (D2Launch+0x103C0, 0x10A40, 0x10E50).
export const BNET_ACCOUNT_STRINGS = { emailSame: 11121, emailMismatch: 11122, passwordSent: 11104, disconnected: 5347 };

// The create form's refusals (D2Launch+0x19880), in the order it tests them.
export const BNET_CREATE_ERROR_STRINGS = { accountShort: 5216, accountLong: 5217, mismatch: 5218, passwordShort: 5219, passwordLong: 5220 };

// The login failure box (D2Launch+0xE510 on the Bnclient event Bnclient+0x109A0 raises for the
// LOGONRESPONSE2 result): title 5244, body by result. A second wrong password for the same account
// swaps 5207 for 11107 (D2Launch's per-account failure counter); result 6 shows the server's text.
export const BNET_LOGIN_ERROR_STRINGS = { title: 5244, noAccount: 5208, password: 5207, passwordAgain: 11107, closed: 5209 };

// Retail's anchors on the 800x600 surface. A cel's anchor is its bottom row; text y is the glyph bottom.
export const BNET_LOGIN_LAYOUT = {
  boxes: { x: 319, account: 347, password: 401 },
  edit: { x: 330, account: 343, password: 397 },
  labels: { x: 321, account: 318, password: 372, colour: 4 },
  // The instruction block: a centred D2Win text box at x=200, 400 wide (descriptor D2Launch+0x21520),
  // its Font16 lines word-wrapped to that width, the first at y=260, one every 14 rows.
  info: { x: 200, width: 400, y: 260, step: 14, colour: 0 },
  buttons: [
    { id: 'create', kind: 'wide', x: 264, anchorY: 572 },
    { id: 'settings', kind: 'wide', x: 264, anchorY: 528 },
    { id: 'login', kind: 'login', x: 264, anchorY: 484 },
    { id: 'exit', kind: 'medium', x: 33, anchorY: 572 },
  ],
  limits: { account: 16, password: 13 },
  // The message box, D2Launch+0x155B0's four control descriptors (D2Launch+0x23A70..0x23B00):
  // the PopUpWide frame at (195, 422) 410x245, OK a MediumButtonBlank at (335, 412), the title a
  // centred Font30 text box at x=195 410 wide whose top is row 198, the body a left Font24 text box
  // at x=199 402 wide whose top is row 238, inset 10 across and 8 down. Lines step 19 rows.
  box: { x: 195, anchorY: 422, width: 410, ok: { x: 335, anchorY: 412 },
    title: { x: 195, width: 410, top: 198 }, body: { x: 199, width: 402, top: 238, inset: 10, insetY: 8, step: 19 } },
  // CREATE NEW ACCOUNT (D2Launch+0x17780): a third frame, label and box under the login form's two;
  // OK and EXIT are MediumButtonBlank (descriptors D2Launch+0x21550/+0x21580). The title (5222) is
  // built only when D2Lang's #10227 is 0, as on the login screen, so it is not drawn.
  create: { boxes: { account: 347, password: 401, verify: 455 }, edit: { account: 343, password: 397, verify: 451 },
    labels: { account: 318, password: 372, verify: 426 }, limits: { account: 16, password: 13, verify: 13 },
    buttons: [{ id: 'create-ok', label: 'ok', kind: 'medium', x: 627, anchorY: 572 }, { id: 'create-exit', label: 'exit', kind: 'medium', x: 33, anchorY: 572 }] },
  // ACCOUNT SETTINGS (D2Launch+0x10844): a centred Font30 title in a box 800 wide whose top is row
  // 250, three Font16 lines centred in 800 (box tops 284/369/454), three 3WideButtonBlank buttons and
  // CANCEL. Text in a D2Win text box sits one font line height below the box top.
  settings: { titleTop: 250, lineTops: [284, 369, 454],
    buttons: [{ id: 'change-password', label: 'changePassword', kind: 'login', x: 264, anchorY: 335 },
      { id: 'get-password', label: 'getPassword', kind: 'login', x: 264, anchorY: 420 },
      { id: 'change-email', label: 'changeEmail', kind: 'login', x: 264, anchorY: 505 },
      { id: 'settings-cancel', label: 'cancel', kind: 'medium', x: 33, anchorY: 572 }] },
  // The account forms. CHANGE PASSWORD frames are textbox2 cels at x=319 (descriptors D2Launch+0x21310..), the other
  // two EditTextBoxWide (two cels, 300 wide) at x=248 (+0x247F0..). Per row: the frame's bottom row `frame`,
  // the label's text row, the edit text's x and row, the characters it takes. A label text row sits 22 above
  // its box's bottom row, an edit's text 1 below the descriptor row, as on the login form. Titles are Font30
  // boxes 800 wide whose top is row 261, the explanations Font16 boxes 480 wide at x=158 whose top is row 281.
  forms: {
    cpass: { title: 'changePasswordTitle', frameArt: 'textbox', frameX: 319, frameW: 169, labelX: 321, editX: 330, editW: 162,
      rows: [{ id: 'account', label: 'account', frame: 347, labelY: 318, editY: 343, limit: 16 },
        { id: 'old', label: 'currentPassword', frame: 401, labelY: 372, editY: 397, limit: 13, mask: true },
        { id: 'password', label: 'newPassword', frame: 455, labelY: 426, editY: 451, limit: 13, mask: true },
        { id: 'verify', label: 'verifyNewPassword', frame: 509, labelY: 480, editY: 505, limit: 13, mask: true }],
      buttons: [{ id: 'cpass-ok', label: 'ok', kind: 'medium', x: 627, anchorY: 572 }, { id: 'cpass-exit', label: 'exit', kind: 'medium', x: 33, anchorY: 572 }] },
    gpass: { title: 'getPasswordTitle', info: 'getPasswordInfo', frameArt: 'wideTextbox', frameX: 248, frameW: 300, labelX: 250, editX: 259, editW: 293,
      rows: [{ id: 'account', label: 'account', frame: 427, labelY: 398, editY: 423, limit: 16 },
        { id: 'email', label: 'email', frame: 477, labelY: 448, editY: 473, limit: 255 }],
      buttons: [{ id: 'gpass-ok', label: 'send', kind: 'medium', x: 627, anchorY: 572 }, { id: 'gpass-cancel', label: 'cancel', kind: 'medium', x: 33, anchorY: 572 }] },
    cemail: { title: 'changeEmailTitle', info: 'changeEmailInfo', frameArt: 'wideTextbox', frameX: 248, frameW: 300, labelX: 250, editX: 259, editW: 293,
      rows: [{ id: 'account', label: 'account', frame: 402, labelY: 373, editY: 398, limit: 16 },
        { id: 'oldEmail', label: 'oldEmail', frame: 452, labelY: 423, editY: 448, limit: 255 },
        { id: 'newEmail', label: 'newEmail', frame: 502, labelY: 473, editY: 498, limit: 255 },
        { id: 'verifyEmail', label: 'verifyNewEmail', frame: 552, labelY: 523, editY: 548, limit: 255 }],
      buttons: [{ id: 'cemail-ok', label: 'send', kind: 'medium', x: 627, anchorY: 572 }, { id: 'cemail-cancel', label: 'cancel', kind: 'medium', x: 33, anchorY: 572 }] },
    title: { top: 261 }, info: { x: 158, width: 480, top: 281 },
  },
  // D2Launch+0x15090's notice (descriptors D2Launch+0x238C0..0x23920): the PopUpOK frame at
  // (268, 350), OK a CancelButtonBlank at (351, 337) labelled in FontRidiculous, the text a centred
  // Font24 box at x=268 264 wide whose top is row 200; its lines start 8 rows lower and step 19.
  notice: { x: 268, anchorY: 350, ok: { x: 351, anchorY: 337, w: 96, h: 32, labelDy: -9 },
    body: { x: 268, width: 264, top: 200, inset: 10, insetY: 8, step: 19 } },
  // PLEASE READ (D2Launch+0x184D0): the TermsofUseBckg panel at (103, 332/526), OK (525, 513) and
  // CANCEL (133, 513), a centred Font42 title box at x=103 570 wide, top row 104, and three centred
  // FontFormal12 boxes at x=110 570 wide, tops 162/292/382, lines 22 rows apart.
  read: { x: 103, anchors: [332, 526], title: { x: 103, width: 570, top: 104 }, body: { x: 110, width: 570, tops: [162, 292, 382], step: 22 },
    buttons: [{ id: 'read-ok', label: 'ok', kind: 'medium', x: 525, anchorY: 513 }, { id: 'read-cancel', label: 'cancel', kind: 'medium', x: 133, anchorY: 513 }] },
};

// D2Launch+0x19880, the create form's OK: the first rule that fails names its notice, else null.
// The verify compare is Unicode::stricmp, so case does not count.
export function createAccountError(account, password, verify) {
  const S = BNET_CREATE_ERROR_STRINGS;
  if (account.length < 2) return S.accountShort;
  if (account.length > 15) return S.accountLong;
  if (password.toLowerCase() !== verify.toLowerCase()) return S.mismatch;
  if (password.length < 2) return S.passwordShort;
  if (password.length > 20) return S.passwordLong;
  return null;
}

// D2Launch+0x1A030, CHANGE PASSWORD's OK: the account, then new against verify (case does not count),
// then the new password's length; the same notices as the create form. A new password is 2..20 long.
export function changePasswordError(account, password, verify) { return createAccountError(account, password, verify); }

// D2Launch+0x10D40, CHANGE EMAIL's SEND: the two new addresses must match (byte for byte), then differ from the old.
export function changeEmailError(oldEmail, newEmail, verifyEmail) {
  const S = BNET_ACCOUNT_STRINGS;
  if (newEmail !== verifyEmail) return S.emailMismatch;
  if (oldEmail === newEmail) return S.emailSame;
  return null;
}

const ACCOUNT_REFUSED = new Set(['"', '#', '%', '*', ',', '/', '<', '>', '?', '\\']);
// D2Launch+0x126C0, the account box's character filter.
export const accountCharAllowed = (ch) => ch.length === 1 && ch > ' ' && ch < '\x7f' && !ACCOUNT_REFUSED.has(ch);
// D2Launch+0x122C0, the LOG IN enable rule.
export const loginEnabled = (account, password) => account.length >= 1 && account.length <= 50 && password.length >= 1 && password.length <= 13;

// D2Win's text box breaks a paragraph at spaces into the longest lines that fit its width.
export function wrapWords(text, width, measure) {
  const lines = [];
  let line = '';
  for (const word of String(text).split(/\s+/).filter(Boolean)) {
    const next = line ? `${line} ${word}` : word;
    if (line && measure(next) > width) { lines.push(line); line = word; } else line = next;
  }
  if (line) lines.push(line);
  return lines;
}

// D2Win's text box as the message box fills it (D2Launch+0x12A70): the text is cut at each newline
// and every piece wrapped at single spaces, so a run of spaces inside a line survives (retail draws
// "incorrect.  You" with both); the space a line breaks at is not drawn.
export function wrapSpaces(text, width, measure) {
  const lines = [];
  for (const piece of String(text).split('\n')) {
    let line = null;
    for (const word of piece.split(' ')) {
      const next = line === null ? word : `${line} ${word}`;
      if (line !== null && line !== '' && measure(next) > width) { lines.push(line); line = word; } else line = next;
    }
    if (line) lines.push(line);
  }
  return lines;
}

// D2Launch's last failed account and its count; process lifetime, like retail's globals.
const failures = { account: null, count: 0 };
// The body retail shows for a rejected logon (D2Launch+0xE510 cases 0xD/0xE/0xF/0x1B).
export function loginErrorBody(error, account, string) {
  const S = BNET_LOGIN_ERROR_STRINGS;
  switch (error?.logonResult) {
    case 1: return string(S.noAccount);
    case 2: {
      if (failures.account === account) failures.count++; else { failures.account = account; failures.count = 1; }
      return string(failures.count < 2 ? S.password : S.passwordAgain);
    }
    case 3: return string(S.closed);
    case 6: return String(error.logonReason ?? '');
    // NOT retail: a transport failure before the logon reply. Retail reports those from its connecting
    // popup, which this client does not draw yet; the box carries the error text meanwhile.
    default: return String(error?.message ?? error);
  }
}

export function createBnetLoginScreen({ read, decodeDc6, decodeDatPalette, string = () => '', region = '',
  lastAccount = '', now = () => performance.now(), onRemember = () => {}, onAuthenticate = async () => {}, onExit = () => {},
  onCreateAccount = null, onChangePassword = null, onResetPassword = null, onChangeEmail = null, documentObject = globalThis.document }) {
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
  const A = BNET_LOGIN_ASSETS;
  const textbox = cels(A.textbox);
  const wide = cels(A.wideButton);
  const login = cels(A.loginButton);
  const medium = cels(A.mediumButton);
  const popup = cels(A.popup);
  const wideTextbox = (() => { let c = null; return () => (c ??= cels(A.wideTextbox)); })();
  // The other screens' art is read on first use, so the login screen costs what it did.
  const lazy = (path) => { let c = null; return () => (c ??= cels(path)); };
  const noticePopup = lazy(A.noticePopup), noticeButton = lazy(A.noticeButton), readBg = lazy(A.readBackground);
  const bg = { ...composeTiled(decodeDc6(read(FRONTEND.background)), skyPalette, TITLE_COLUMNS, documentObject), path: FRONTEND.background };
  const logos = [FRONTEND.logoBlack[0], FRONTEND.logoFire[0], FRONTEND.logoBlack[1], FRONTEND.logoFire[1]].map(cels);

  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const blend = createBlender(skyPl2, palette);
  const logoBlend = createBlender(skyPl2, skyPalette);
  const labelFont = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: tinted, documentObject,
    path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend } });
  const font16Dc6 = decodeDc6(read(FRONTEND.versionFont)), font16Tbl = decodeFontTbl(read(FRONTEND.versionFontTbl));
  const font16 = (colour) => createFont({ dc6: font16Dc6, tbl: font16Tbl, palette: colour ? textColourPalette(skyPl2, skyPalette, colour) : skyPalette, documentObject,
    path: FRONTEND.versionFont, drawAs: { op: 'celcolor', mode: 5, extra: colour } });
  const labelText = font16(BNET_LOGIN_LAYOUT.labels.colour);
  const infoText = font16(BNET_LOGIN_LAYOUT.info.colour);
  const boxFont = (dc6, tbl) => createFont({ dc6: decodeDc6(read(dc6)), tbl: decodeFontTbl(read(tbl)), palette: skyPalette,
    documentObject, path: dc6, drawAs: { op: 'celcolor', mode: 5, extra: 0 } });
  const titleText = boxFont(A.titleFont, A.titleFontTbl);
  const bodyText = boxFont(A.bodyFont, A.bodyFontTbl);
  const editFont = boxFont(A.editFont, A.editFontTbl);
  let bigFont = null, tinyFont = null;
  const bigText = () => (bigFont ??= boxFont(A.bigFont, A.bigFontTbl));
  const tinyLabel = () => (tinyFont ??= createFont({ dc6: decodeDc6(read(A.tinyFont)), tbl: decodeFontTbl(read(A.tinyFontTbl)), palette: tinted, documentObject,
    path: A.tinyFont, drawAs: { op: 'celcolor', mode: 4, blend } }));
  const cursorDc6 = decodeDc6(read(FRONTEND.cursor));
  const cursorCanvas = frameCanvas(cursorDc6.frames[0]);

  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sctx = scene.getContext('2d', { willReadFrequently: true });
  const view = (w, h) => { const scale = Math.min(w / 800, h / 600); return { scale, ox: Math.round((w - 800 * scale) / 2), oy: Math.round((h - 600 * scale) / 2) }; };

  const L = BNET_LOGIN_LAYOUT, C = L.create, FORMS = L.forms;
  const S = BNET_LOGIN_STRINGS, AS = BNET_ACCOUNT_STRINGS;
  const text = (key) => string(S[key]) || '';
  // The screen on show: 'login' (D2Launch+0x19290), 'create' (+0x17780), 'settings' (+0x10844),
  // 'read' (+0x184D0, PLEASE READ) or an account form: 'cpass' (+0x15F10), 'gpass' (+0x10A40), 'cemail' (+0x103E5).
  // `message` is the message box (login failure, get-password receipt, email refusals), `notice` D2Launch+0x15090's box;
  // each carries `then`, what its OK does.
  let mode = 'login';
  // D2Launch+0x19290 fills the account box from the "Last BNet" registry value and then focuses the
  // password; LOG IN writes that value before it connects (D2Launch+0x19FE5), so a failed attempt's
  // account is back in the box when OK rebuilds the screen.
  let account = '', password = '', verify = '', focus = 'account', busy = false, message = null, notice = null;
  // The account forms' boxes by id (the account box is `account` on every screen).
  let fv = {};
  const init = () => { mode = 'login'; notice = null; message = null; fv = {}; account = String(lastAccount ?? '').slice(0, L.limits.account); password = ''; verify = ''; focus = account ? 'password' : 'account'; };
  init();
  // D2Launch+0x11950 keeps the typed account and password in the launcher's state (+0x89, +0xA1) as
  // CREATE NEW ACCOUNT leaves the login screen and again on the create form's OK; D2Launch+0x17780
  // fills its first two boxes from there, never the verify box, and focuses the first empty one.
  const saved = { account: '', password: '' };
  const openCreate = () => {
    mode = 'create'; notice = null;
    account = saved.account.slice(0, C.limits.account); password = saved.password.slice(0, C.limits.password); verify = '';
    focus = !account ? 'account' : !password ? 'password' : 'verify';
  };
  // D2Launch+0x15F10, +0x10A40, +0x103E5: the account box from the launcher's saved account, the others empty;
  // CHANGE PASSWORD focuses the current password when the account is filled, the others always the account box.
  const openForm = (m) => {
    mode = m; notice = null; message = null;
    fv = { account: saved.account.slice(0, FORMS[m].rows[0].limit) };
    focus = m === 'cpass' && fv.account ? 'old' : 'account';
  };
  // D2Launch+0x10820: the settings screen again.
  const openSettings = () => { mode = 'settings'; notice = null; message = null; };
  let pressed = null, pointer = [799, 599], tick = 0;

  // One cel at its retail anchor (left x, bottom row y).
  const drawCel = (c, index, x, anchorY, mode = 5) => {
    const f = c.dc6.frames[index]; if (!f) return;
    const left = x + f.offsetX, top = anchorY + f.offsetY - f.height + 1;
    sctx.drawImage(c.canvases[index], left, top);
    recordDc6(c.path, index, f, left, top, { op: 'cel', mode });
  };
  const drawCels = (c, indices, x, anchorY) => { for (const index of indices) { drawCel(c, index, x, anchorY); x += c.dc6.frames[index].width; } };
  // The logos are D2Win animated images (speed 1): frame = (GetTickCount - created) / 40 ms, modulo
  // the frame count less one (D2Win's ANIMIMAGE draw; retail's frames 0/6/12/18/25 at 250 ms steps).
  let created = now(), pinned = null;
  const clock = () => (pinned === null ? now() : pinned);
  const drawLogos = (still = false) => {
    for (const logo of logos) {
      const frameIndex = still ? 0 : Math.floor(Math.max(0, clock() - created) / 40) % (logo.canvases.length - 1);
      const frame = logo.dc6.frames[frameIndex];
      if (!frame) continue;
      const fire = logo.path.toLowerCase().includes('fire');
      const left = 400 + frame.offsetX, top = 120 + frame.offsetY - frame.height + 1;
      if (fire) logoBlend(sctx, frame, left, top, 3); else sctx.drawImage(logo.canvases[frameIndex], left, top);
      recordDc6(logo.path, frameIndex, frame, left, top, { op: 'cel', mode: fire ? 3 : 5 });
    }
  };
  const enabled = () => !busy && loginEnabled(account, password);
  const buttonArt = (b) => (b.kind === 'wide' ? wide : b.kind === 'login' ? login : b.kind === 'notice' ? noticeButton() : medium);
  const buttonWidth = (b) => (b.kind === 'medium' ? 128 : b.kind === 'notice' ? L.notice.ok.w : 272);
  const buttonHeight = (b) => (b.kind === 'notice' ? L.notice.ok.h : 35);
  const button = (b) => ({ ...b, y: b.anchorY - buttonHeight(b) + 1, w: buttonWidth(b), h: buttonHeight(b) });
  const field = (id, anchorY, x = L.boxes.x, w = 169) => ({ id: `field-${id}`, kind: 'field', x, y: anchorY - 25, w, h: 26 });
  const boxes = () => {
    if (message) return [{ id: 'ok', kind: 'medium', x: L.box.ok.x, y: L.box.ok.anchorY - 34, w: 128, h: 35 }];
    if (notice) return [button({ id: 'notice-ok', kind: 'notice', x: L.notice.ok.x, anchorY: L.notice.ok.anchorY })];
    if (mode === 'create') return [...C.buttons.map(button), ...['account', 'password', 'verify'].map((id) => field(id, C.boxes[id]))];
    if (mode === 'settings') return L.settings.buttons.map(button);
    if (FORMS[mode]) { const F = FORMS[mode]; return [...F.buttons.map(button), ...F.rows.map((r) => field(r.id, r.frame, F.frameX, F.frameW))]; }
    if (mode === 'read') return L.read.buttons.map(button);
    return [...L.buttons.map((b) => ({ id: b.id, kind: b.kind, x: b.x, y: b.anchorY - 34, w: buttonWidth(b), h: 35 })),
      field('account', L.boxes.account), field('password', L.boxes.password)];
  };
  const at = (x, y, w, h) => { const v = view(w, h); return [(x - v.ox) / v.scale, (y - v.oy) / v.scale]; };
  const hit = (x, y, w, h) => { const [sx, sy] = at(x, y, w, h); return boxes().find((b) => sx >= b.x && sx < b.x + b.w && sy >= b.y && sy < b.y + b.h) ?? null; };

  const submit = async () => {
    if (!enabled()) return false;
    busy = true;
    lastAccount = account; onRemember(account);
    try { await onAuthenticate({ account, password }); }
    catch (error) { fail(error); }
    finally { busy = false; }
    return true;
  };
  const fail = (error) => { message = { title: string(BNET_LOGIN_ERROR_STRINGS.title) || '', body: loginErrorBody(error, account, string) }; };
  // D2Launch+0x19880: keep the two fields, then the first failing rule's notice (its OK rebuilds the
  // form, D2Launch+0x17780), else PLEASE READ.
  const createOk = () => {
    saved.account = account; saved.password = password;
    const error = createAccountError(account, password, verify);
    if (error !== null) { notice = { body: string(error) || '' }; return; }
    mode = 'read';
  };
  // PLEASE READ's OK (D2Launch+0x19FB0) connects and sends the account to Battle.net.
  // `onCreateAccount` sends the account to the realm (BNCS CREATEACCOUNT); without one the login
  // screen comes back with the login failure box naming that.
  const readOk = async () => {
    const credentials = { account, password };
    init(); account = credentials.account; focus = 'password';
    if (!onCreateAccount) { fail(new Error('this realm does not create accounts')); return; }
    busy = true;
    try { await onCreateAccount(credentials); } catch (error) { fail(error); } finally { busy = false; }
  };
  // A request that never reached the realm: D2Launch's connect failure notice (+0x10E50, string 5347), whose
  // OK (+0x17DF0) drops the connection and builds the login screen.
  const disconnected = () => { notice = { body: string(AS.disconnected) || '', then: init }; };
  const send = async (request) => {
    busy = true;
    try { return { value: await request() }; } catch { disconnected(); return null; } finally { busy = false; }
  };
  // D2Launch+0x1A030: keep the typed account, test the rules, then ask Battle.net (BNCS CHANGEPASSWORD). A notice's
  // OK rebuilds the form (+0x15F10). The accepted change continues as a log on with the new password.
  const changePasswordOk = async () => {
    saved.account = fv.account ?? '';
    const error = changePasswordError(fv.account ?? '', fv.password ?? '', fv.verify ?? '');
    if (error !== null) { notice = { body: string(error) || '', then: () => openForm('cpass') }; return; }
    const request = { account: fv.account, password: fv.old ?? '', newPassword: fv.password };
    if (!onChangePassword) { fail(new Error('this realm does not change passwords')); return; }
    const sent = await send(() => onChangePassword(request));
    if (sent === null) return;
    const result = sent.value;
    if (result !== 0 && result !== true) {
      // NOT retail: the text of Battle.net's refusal is raised by a Bnclient event this client has not recovered;
      // the login failure box with its wrong password text stands in, and its OK rebuilds the form.
      message = { title: string(BNET_LOGIN_ERROR_STRINGS.title) || '', body: string(BNET_LOGIN_ERROR_STRINGS.password) || '', then: () => openForm('cpass') };
      return;
    }
    // Retail follows the accepted change straight into its log on (D2Launch+0x19FA0).
    account = request.account; password = request.newPassword; verify = ''; lastAccount = account; onRemember(account);
    init(); account = request.account; focus = 'password';
    busy = true;
    try { await onAuthenticate({ account: request.account, password: request.newPassword }); }
    catch (error) { fail(error); } finally { busy = false; }
  };
  // D2Launch+0x10F70: SEND asks for a new password (BNCS RESETPASSWORD) and shows the receipt (11104), whose OK
  // builds the login screen.
  const getPasswordOk = async () => {
    saved.account = fv.account ?? '';
    const request = { account: fv.account ?? '', email: fv.email ?? '' };
    if (!onResetPassword) { fail(new Error('this realm does not reset passwords')); return; }
    if ((await send(() => onResetPassword(request))) === null) return;
    message = { title: '', body: string(AS.passwordSent) || '', then: init };
  };
  // D2Launch+0x10D40: SEND tests the two rules (popups 11122, 11121, whose OK rebuilds the form with the saved
  // account), then asks Battle.net (BNCS CHANGEEMAIL) and builds the login screen.
  const changeEmailOk = async () => {
    const error = changeEmailError(fv.oldEmail ?? '', fv.newEmail ?? '', fv.verifyEmail ?? '');
    if (error !== null) { message = { title: '', body: string(error) || '', then: () => openForm('cemail') }; return; }
    const request = { account: fv.account ?? '', oldEmail: fv.oldEmail ?? '', newEmail: fv.newEmail };
    if (!onChangeEmail) { fail(new Error('this realm does not change emails')); return; }
    if ((await send(() => onChangeEmail(request))) === null) return;
    init();
  };
  const activate = (b) => {
    if (busy) return;
    // OK closes the box and its callback (D2Launch+0x19290) builds the login screen afresh.
    if (b.id === 'ok') { const next = message.then ?? init; message = null; next(); return; }
    if (b.id === 'notice-ok') { const next = notice.then ?? openCreate; notice = null; next(); return; }
    if (b.id.startsWith('field-')) { focus = b.id.slice(6); return; }
    if (b.id === 'login') { void submit(); return; }
    if (b.id === 'exit') { onExit(); return; }
    // CREATE NEW ACCOUNT (D2Launch+0x1A220) keeps the typed fields and opens the create form.
    // NOT retail: retail first connects and shows the Terms of Service screen (D2Launch+0x18E10, its
    // text a file Battle.net sends); its AGREE (D2Launch+0x18E00) is what opens the form.
    if (b.id === 'create') { saved.account = account; saved.password = password; openCreate(); return; }
    if (b.id === 'create-ok') { createOk(); return; }
    // D2Launch+0x11950 keeps the typed account as the launcher leaves the login screen.
    if (b.id === 'settings') { saved.account = account; saved.password = password; openSettings(); return; }
    if (b.id === 'change-password') { openForm('cpass'); return; }
    if (b.id === 'get-password') { openForm('gpass'); return; }
    if (b.id === 'change-email') { openForm('cemail'); return; }
    // The forms' EXIT and CANCEL (D2Launch+0x186F0, +0x10A30, +0x10D30) all run +0x10820: the settings screen.
    if (b.id === 'cpass-exit' || b.id === 'gpass-cancel' || b.id === 'cemail-cancel') { openSettings(); return; }
    if (b.id === 'cpass-ok') { void changePasswordOk(); return; }
    if (b.id === 'gpass-ok') { void getPasswordOk(); return; }
    if (b.id === 'cemail-ok') { void changeEmailOk(); return; }
    // EXIT, CANCEL and PLEASE READ's CANCEL all rebuild the login screen (D2Launch+0x19290).
    if (b.id === 'create-exit' || b.id === 'settings-cancel' || b.id === 'read-cancel') { init(); return; }
    if (b.id === 'read-ok') { void readOk(); return; }
  };

  // A D2Win button: its cels, then its label centred over the whole button width (Exocet10, or
  // FontRidiculous on the notice's OK).
  const drawButton = (b) => {
    const art = buttonArt(b), down = pressed === b.id;
    const state = b.kind === 'login' && b.id === 'login' && !enabled() ? 2 : down ? 1 : 0;
    const perState = b.kind === 'medium' || b.kind === 'notice' ? 1 : 2;
    drawCels(art, Array.from({ length: perState }, (_, k) => state * perState + k), b.x, b.anchorY);
    const label = text(b.label ?? b.id), w = buttonWidth(b);
    const font = b.kind === 'notice' ? tinyLabel() : labelFont;
    const y = b.kind === 'notice' ? b.anchorY + L.notice.ok.labelDy : b.anchorY + 4 - Math.trunc((35 - labelFont.lineHeight) / 2);
    font.drawAnchored(sctx, label, b.x + Math.max(Math.trunc((w - font.measure(label)) / 2), 0), y);
  };
  // A centred D2Win text box: wrapped to its width, first line one line height below `top`.
  const drawCentred = (font, value, x, width, top, step, dy = 0) => {
    for (const [i, line] of wrapSpaces(value, width, (t) => font.measure(t)).entries())
      font.drawAnchored(sctx, line, x + Math.max(Math.trunc((width - font.measure(line)) / 2), 0), top + dy + font.lineHeight + i * step);
  };
  const dim = () => {
    sctx.save(); sctx.globalAlpha = 0x7f / 0xff; sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600); sctx.restore();
    recordPrim('boxalpha', 0, 0, 800, 600, 0, 0x7f);
  };

  // The message box over the whole login screen: D2Gfx dims the surface first (retail's #10045 call;
  // the capture logs its words 0,0,800,600 colour 0 level 0x7f), then the frame, OK, title and body.
  const drawMessage = () => {
    const B = L.box;
    dim();
    drawCels(popup, [0, 1], B.x, B.anchorY);
    drawButton({ id: 'ok', kind: 'medium', x: B.ok.x, anchorY: B.ok.anchorY });
    for (const [i, line] of wrapSpaces(message.title, B.title.width, (t) => titleText.measure(t)).entries())
      titleText.drawAnchored(sctx, line, B.title.x + Math.max(Math.trunc((B.title.width - titleText.measure(line)) / 2), 0),
        B.title.top + titleText.lineHeight - 1 + i * titleText.lineHeight);
    const body = B.body, bx = body.x + body.inset, bw = body.width - 2 * body.inset;
    for (const [i, line] of wrapSpaces(message.body, bw, (t) => bodyText.measure(t)).entries())
      bodyText.drawAnchored(sctx, line, bx, body.top + body.insetY + bodyText.lineHeight - 1 + i * body.step);
  };
  // D2Launch+0x15090's notice over the screen that raised it: the same dim, frame, OK, centred text.
  const drawNotice = () => {
    const N = L.notice, body = N.body;
    dim();
    drawCels(noticePopup(), [0, 1], N.x, N.anchorY);
    drawButton({ id: 'notice-ok', label: 'ok', kind: 'notice', x: N.ok.x, anchorY: N.ok.anchorY });
    drawCentred(bodyText, notice.body, body.x + body.inset, body.width - 2 * body.inset, body.top, body.step, body.insetY);
  };
  // Each edit box draws its text, then (the focused one, while GetTickCount / 250 is odd) its "_"
  // caret after the text before the caret (D2Win+0x14D6F). The caret here is always at the end.
  const drawEdits = (edits, x = L.edit.x, width = Infinity) => {
    const caret = Math.floor(clock() / 250) % 2 === 1;
    for (const [id, full, y] of edits) {
      let shown = full;
      while (shown && editFont.measure(shown) > width) shown = shown.slice(1);
      editFont.drawAnchored(sctx, shown, x, y);
      if (caret && focus === id) editFont.drawAnchored(sctx, '_', x + editFont.measure(shown), y);
    }
  };
  const drawLoginForm = () => {
    drawCel(textbox, 0, L.boxes.x, L.boxes.account);
    drawCel(textbox, 0, L.boxes.x, L.boxes.password);
    labelText.drawAnchored(sctx, text('account'), L.labels.x, L.labels.account);
    labelText.drawAnchored(sctx, text('password'), L.labels.x, L.labels.password);
    wrapWords(text('info'), L.info.width, (t) => infoText.measure(t)).forEach((line, i) =>
      infoText.drawAnchored(sctx, line, L.info.x + Math.trunc((L.info.width - infoText.measure(line)) / 2), L.info.y + i * L.info.step));
    drawEdits([['account', account, L.edit.account], ['password', '*'.repeat(password.length), L.edit.password]]);
    for (const b of L.buttons) drawButton(b);
  };
  const drawCreateForm = () => {
    const ids = ['account', 'password', 'verify'];
    for (const id of ids) drawCel(textbox, 0, L.boxes.x, C.boxes[id]);
    for (const id of ids) labelText.drawAnchored(sctx, text(id), L.labels.x, C.labels[id]);
    wrapWords(text('createInfo'), L.info.width, (t) => infoText.measure(t)).forEach((line, i) =>
      infoText.drawAnchored(sctx, line, L.info.x + Math.trunc((L.info.width - infoText.measure(line)) / 2), L.info.y + i * L.info.step));
    drawEdits([['account', account, C.edit.account], ['password', '*'.repeat(password.length), C.edit.password], ['verify', '*'.repeat(verify.length), C.edit.verify]]);
    for (const b of C.buttons) drawButton(b);
  };
  // D2Launch+0x15F10 / +0x10A40 / +0x103E5: frames, labels, the title box and the info text, then the boxes' text.
  const drawForm = () => {
    const F = FORMS[mode], art = F.frameArt === 'wideTextbox' ? wideTextbox() : textbox;
    for (const r of F.rows) drawCel(art, 0, F.frameX, r.frame);
    for (const r of F.rows) labelText.drawAnchored(sctx, text(r.label), F.labelX, r.labelY);
    drawCentred(titleText, text(F.title), 0, 800, FORMS.title.top, titleText.lineHeight);
    if (F.info) drawCentred(infoText, text(F.info), FORMS.info.x, FORMS.info.width, FORMS.info.top, 14);
    drawEdits(F.rows.map((r) => [r.id, r.mask ? '*'.repeat((fv[r.id] ?? '').length) : fv[r.id] ?? '', r.editY]), F.editX, F.editW);
    for (const b of F.buttons) drawButton(b);
  };
  const drawSettings = () => {
    const T = L.settings;
    drawCentred(titleText, text('settingsTitle'), 0, 800, T.titleTop, titleText.lineHeight);
    S.settingsLines.forEach((id, i) => drawCentred(infoText, string(id) || '', 0, 800, T.lineTops[i], 14));
    for (const b of T.buttons) drawButton(b);
  };
  const drawRead = () => {
    const R = L.read;
    drawCels(readBg(), [0, 1, 2], R.x, R.anchors[0]);
    drawCels(readBg(), [3, 4, 5], R.x, R.anchors[1]);
    for (const b of R.buttons) drawButton(b);
    drawCentred(bigText(), text('readTitle'), R.title.x, R.title.width, R.title.top, bigText().lineHeight);
    S.readBodies.forEach((id, i) => drawCentred(editFont, string(id) || '', R.body.x, R.body.width, R.body.tops[i], R.body.step));
  };

  const fieldValue = (id) => (FORMS[mode] ? fv[id] ?? '' : id === 'account' ? account : id === 'password' ? password : verify);
  const limit = (id) => (FORMS[mode] ? FORMS[mode].rows.find((r) => r.id === id).limit : mode === 'create' ? C.limits[id] : L.limits[id]);
  const setField = (id, value) => {
    if (FORMS[mode]) fv[id] = value; else if (id === 'account') account = value; else if (id === 'password') password = value; else verify = value;
  };
  const order = () => (FORMS[mode] ? FORMS[mode].rows.map((r) => r.id) : mode === 'create' ? ['account', 'password', 'verify'] : ['account', 'password']);

  return {
    get focus() { return focus; },
    get busy() { return busy; },
    get mode() { return mode; },
    get evidence() {
      return { region, mode, form: { ...fv, old: undefined, password: undefined, verify: undefined, oldLength: (fv.old ?? '').length,
          passwordLength: (fv.password ?? '').length, verifyLength: (fv.verify ?? '').length }, account, passwordLength: password.length, verifyLength: verify.length, focus, busy, message, notice, loginEnabled: enabled(),
        boxes: boxes().map(({ id, x, y, w, h }) => ({ id, x, y, w, h })), cursor: pointer };
    },
    // Raise retail's failure box for a rejected logon (the error carries `logonResult`).
    fail(error) { fail(error); },
    reset() { message = null; notice = null; busy = false; pressed = null; init(); },
    // Typing. Retail's edit box takes printable characters, Backspace, Tab (next box) and Enter
    // (LOG IN on the login form, OK on the create form: its boxes' Enter callback is D2Launch+0x19880).
    typeKey(code, key) {
      if (busy) return code === 'Tab' || code === 'Enter' || code === 'Backspace' || (key && key.length === 1);
      const enter = code === 'Enter' || code === 'NumpadEnter';
      if (message) { if (enter || code === 'Escape') activate({ id: 'ok' }); return true; }
      if (notice) { if (enter || code === 'Escape') activate({ id: 'notice-ok' }); return true; }
      if (mode === 'settings' || mode === 'read') return false;
      const ids = order();
      if (code === 'Tab') { focus = ids[(ids.indexOf(focus) + 1) % ids.length]; return true; }
      if (enter) {
        if (mode === 'create') createOk();
        else if (mode === 'cpass') void changePasswordOk();
        else if (mode === 'gpass') void getPasswordOk();
        else if (mode === 'cemail') void changeEmailOk();
        else if (focus === 'account' && !password) focus = 'password'; else void submit();
        return true;
      }
      if (code === 'Backspace') { setField(focus, fieldValue(focus).slice(0, -1)); return true; }
      if (!key || key.length !== 1) return false;
      if (fieldValue(focus).length >= limit(focus)) return true;
      if (focus === 'account') { if (accountCharAllowed(key)) setField('account', fieldValue('account') + key); return true; }
      if (key >= ' ' && key < '\x7f') setField(focus, fieldValue(focus) + key);
      return true;
    },
    targets(w, h) { const v = view(w, h); return Object.fromEntries(boxes().map((b) => [b.id, [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)]])); },
    hover(x, y, w, h) { const [sx, sy] = at(x, y, w, h); pointer = [Math.trunc(sx), Math.trunc(sy)]; },
    press(x, y, w, h) { const b = hit(x, y, w, h); if (b?.id === 'login' && !enabled()) return null; pressed = b?.id ?? null; return pressed; },
    release(token, x, y, w, h) { const b = hit(x, y, w, h); pressed = null; if (b && b.id === token) { activate(b); return true; } return false; },
    // Scenarios and tests: run a control by id, as a click on it would.
    activate(id) { const b = boxes().find((x) => x.id === id); if (b) activate(b); return Boolean(b); },
    cancel() { pressed = null; },
    escape() {
      if (busy) return;
      if (message) activate({ id: 'ok' });
      else if (notice) activate({ id: 'notice-ok' });
      else if (mode === 'login') onExit();
      else if (FORMS[mode]) openSettings();
      else init();
    },
    step() { tick++; },
    // Scenarios: hold the clock at `ms`, the screen built at `builtMs`, as the capture's clock is held.
    pinClock(ms, builtMs = 0) { pinned = ms; created = builtMs; },
    draw(out, w, h) {
      const v = view(w, h);
      out.fillStyle = '#000'; out.fillRect(0, 0, w, h); out.imageSmoothingEnabled = false;
      sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600);
      // PLEASE READ rebuilds the logos ahead of the background, and retail's capture holds them at
      // frame 0 over the four ticks it draws the screen.
      if (mode === 'read') drawLogos(true);
      drawBg(sctx, bg.path, bg.canvas); recordTiled(bg.path, bg, 0, 0);
      if (mode !== 'read') drawLogos();
      if (mode === 'create') drawCreateForm();
      else if (FORMS[mode]) drawForm();
      else if (mode === 'settings') drawSettings();
      else if (mode === 'read') drawRead();
      else drawLoginForm();
      if (message) drawMessage();
      if (notice) drawNotice();
      const f = cursorDc6.frames[0];
      const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
      sctx.drawImage(cursorCanvas, left, top);
      recordDc6(FRONTEND.cursor, 0, f, left, top);
      present(out, scene, v, bg.path);
    },
  };
}
