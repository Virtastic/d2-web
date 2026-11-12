// The skill-button layout's button reading: one pad sample in, one set of skill actions out. Pure; the
// caller supplies the sample, where the player is (the field, the wheel, the panel list) and the time
// since the last sample.
//
//   A B X Y LB RB  the six skill slots; LT held is their second set (and LT+RT the second main skill)
//   RT             the main skill (the left-click skill a mouse player holds)
//   LT tapped      Swap Weapons, only when no skill fired while it was down, whichever was let go first
//   Back (View)    the skill wheel; Start the panel list; R3 Interact (use, talk, pick up)
//   in the wheel   d-pad up/down pages, left/right switches Skills / Actions, the right stick picks,
//                  a slot button or RT assigns, Back or Start closes
//   in the list    d-pad up/down steps, A opens, B or Start closes
// Leaving the wheel or the list drops every button until all of them are up, so the press that closed
// it never casts in the field.

export const PAD = { A: 0, B: 1, X: 2, Y: 3, LB: 4, RB: 5, LT: 6, RT: 7, BACK: 8, START: 9, L3: 10, R3: 11, UP: 12, DOWN: 13, LEFT: 14, RIGHT: 15 };
export const SLOT_BUTTONS = [PAD.A, PAD.B, PAD.X, PAD.Y, PAD.LB, PAD.RB];
export const TRIGGER_PRESS = 0.5, TRIGGER_RELEASE = 0.3;
export const SWAP_TAP_SECONDS = 0.20;
export const DPAD_REPEAT_DELAY = 0.35, DPAD_REPEAT_EVERY = 0.12;
export const WHEEL_AIM_DEADZONE = 0.30;

const value = (b) => (b ? (typeof b.value === 'number' && b.value > 0 ? b.value : b.pressed ? 1 : 0) : 0);

export function makeSkillMapper({ deadzone = 0.25 } = {}) {
  let prev = [], lt = false, rt = false;
  let swapHold = 0, swapCast = false, swapDown = false, handoff = false;
  const dpadHold = [0, 0, 0, 0];
  const slotOf = (t) => (t < DPAD_REPEAT_DELAY ? 0 : 1 + Math.floor((t - DPAD_REPEAT_DELAY) / DPAD_REPEAT_EVERY));

  // sample: { buttons: [{pressed, value}], axes: [lx, ly, rx, ry] }; ui: 'none' | 'wheel' | 'strip';
  // field: the game screen with nothing open over the world.
  return function step(sample, { ui = 'none', field = false, seconds = 0 } = {}) {
    lt = lt ? value(sample.buttons[PAD.LT]) > TRIGGER_RELEASE : value(sample.buttons[PAD.LT]) >= TRIGGER_PRESS;
    rt = rt ? value(sample.buttons[PAD.RT]) > TRIGGER_RELEASE : value(sample.buttons[PAD.RT]) >= TRIGGER_PRESS;
    const held = sample.buttons.map((b) => Boolean(b?.pressed));
    held[PAD.LT] = lt; held[PAD.RT] = rt;
    const pressed = (i) => held[i] && !prev[i];
    const wheel = ui === 'wheel', strip = ui === 'strip';
    const out = {
      modifierHeld: false, main: false, mainDown: false, slot: -1, slotDown: Array(6).fill(false),
      interact: false, swapTap: false, openWheel: false, openStrip: false,
      wheelStep: 0, wheelTab: 0, aim: null, stripStep: 0, confirm: false, cancel: false, busy: false,
    };

    const dpad = [false, false, false, false];   // up, down, left, right: the edge, then the hold's repeat
    [PAD.UP, PAD.DOWN, PAD.LEFT, PAD.RIGHT].forEach((b, i) => {
      if (!(wheel || strip) || !held[b]) { dpadHold[i] = 0; return; }
      if (!prev[b]) { dpadHold[i] = 0; dpad[i] = true; return; }   // the hold is timed from its press
      const before = dpadHold[i];
      if (seconds > 0) dpadHold[i] += seconds;
      if (slotOf(dpadHold[i]) > slotOf(before)) dpad[i] = true;
    });

    if (field || wheel) {
      SLOT_BUTTONS.forEach((b, i) => { out.slotDown[i] = held[b]; if (pressed(b)) out.slot = i; });
      out.mainDown = rt; out.main = pressed(PAD.RT);
      if (field) {
        out.interact = pressed(PAD.R3);
        out.openWheel = pressed(PAD.BACK);
        out.openStrip = pressed(PAD.START);
      }
    }
    if (wheel) {
      const rx = sample.axes?.[2] || 0, ry = sample.axes?.[3] || 0, len = Math.hypot(rx, ry);
      if (len > Math.max(WHEEL_AIM_DEADZONE, deadzone)) out.aim = [rx / len, ry / len];
      out.wheelStep = Number(dpad[1]) - Number(dpad[0]);
      out.wheelTab = Number(dpad[3]) - Number(dpad[2]);
      out.cancel = pressed(PAD.START) || pressed(PAD.BACK);
    } else if (strip) {
      out.stripStep = Number(dpad[1]) - Number(dpad[0]);
      out.confirm = pressed(PAD.A);
      out.cancel = pressed(PAD.B) || pressed(PAD.START);
    }

    // LT: a hold is the second set, a short tap with no skill during it swaps weapons. A chord counts even
    // when LT went down after the skill, or came up on the same sample as the skill's press.
    const watching = field || wheel;
    if (watching && (lt || swapDown) && (out.mainDown || out.main || out.slot >= 0 || out.slotDown.some(Boolean))) swapCast = true;
    if (watching && lt) { swapHold += Math.max(seconds, 0); out.modifierHeld = true; }
    if (swapDown && !lt && field && swapHold > 0 && swapHold <= SWAP_TAP_SECONDS && !swapCast) out.swapTap = true;
    if (!lt || !watching) { swapHold = 0; swapCast = false; }
    swapDown = watching && lt;

    if (ui !== 'none') handoff = true;
    else if (handoff) {
      if (!held.some(Boolean)) handoff = false;
      out.slot = -1; out.slotDown.fill(false);
      out.main = out.mainDown = out.interact = out.swapTap = out.openWheel = out.openStrip = false;
      swapHold = 0; swapCast = lt;   // an LT already down when the handoff began is no tap
    }
    out.busy = out.modifierHeld || out.mainDown || out.slot >= 0 || out.interact || out.openStrip || out.openWheel || out.swapTap || out.slotDown.some(Boolean);
    prev = held;
    return out;
  };
}
