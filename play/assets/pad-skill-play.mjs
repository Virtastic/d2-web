// What the skill buttons do. The mapper (pad-skill-mapper.mjs) says which buttons went down; this turns
// that into the mouse and keyboard events a player would make, through an injected `io`, so the shell's
// own handlers issue the engine's commands. Nothing here decides a game rule: a skill goes into its hand
// with retail's skill select, and a click at the target casts it, exactly as the mouse does.
//
// io: { selectSkill(id, side), pointer(type, x, y, { button, shiftKey }), fnKey(name), keyHold(name, down) }
// host (read each step): { name, choices, selected: {left, right}, hotkeys: [{skill, left}], inTown, dead,
//   ranked (pad-targeting.mjs's rankTargets), hero: [x, y] canvas, ahead: [x, y] canvas }
//
// Rules carried over from the skill-button design, each in the place it applies:
//   * an aura button selects (or toggles back) the right-hand aura on the press and never clicks;
//   * a passive skill does nothing;
//   * the Town Portal tome casts on a fresh press, outside town only, from the right hand at the hero;
//   * a held skill keeps following a monster; it never moves onto loot, an object or a person, so
//     picking up or using those always needs a new press;
//   * Attack (skill 0) is also Use: a fresh press on an item, object or person clicks it once.
import {
  SLOTS, EMPTY, UNSET, INTERACT, SHOW_ITEMS, ACTIONS, WHEEL_SHOWN, PANELS,
  panelFn, slotOf, setSlot, triggerOf, initialSheet, stableWedge, nextPage, makeHoldSet, makeTriggerHold,
  auraChoice, makeSheetStore, actionLabel,
} from './pad-skill-sheet.mjs';

// 1.13c Skills.txt rows the wheel leaves out (the generic Kick, the identify scroll and tome) and the two
// Town Portal rows that cast through a carried scroll or tome.
export const KICK = 1, SCROLL_IDENTIFY = 217, BOOK_IDENTIFY = 218, SCROLL_PORTAL = 219, BOOK_PORTAL = 220;
const NOT_OFFERED = new Set([KICK, SCROLL_IDENTIFY, BOOK_IDENTIFY]);
const isPortal = (id) => id === SCROLL_PORTAL || id === BOOK_PORTAL;
const MAIN = SLOTS;   // the trigger's own hold index after the six slots

// The words the field hint uses for what Attack / Use would do to the picked target.
export function pickLabel(pick) {
  if (!pick) return 'Attack / Use';
  return { unit: 'Attack', item: 'Pick up', npc: 'Talk', object: 'Use', warp: 'Use', corpse: 'Pick up corpse', player: 'Attack / Use' }[pick.kind] || 'Attack / Use';
}
const SLOT_LABEL = ['A', 'B', 'X', 'Y', 'LB', 'RB'];

export function makeSkillPlay({ io, storage = globalThis.localStorage, prefix = 'd2.padskills' } = {}) {
  const store = makeSheetStore(storage, prefix);
  const slotHold = makeHoldSet(), triggerHold = makeTriggerHold();
  const used = Array(SLOTS + 1).fill(false);
  const active = Array(SLOTS + 1).fill(null);    // per button: the held pointer or key it owns
  let auraReturn = 0;
  const ui = { wheel: false, actions: false, at: 0, fromTree: false, feedback: '', strip: -1, hint: '' };

  const choiceOf = (host, id) => (host.choices || []).find((c) => c.id === id) || null;
  const known = (host, id) => { const c = choiceOf(host, id); return Boolean(c && !c.passive && !NOT_OFFERED.has(id)); };
  const wheelIds = (host) => (ui.actions ? ACTIONS : (host.choices || []).filter((c) => !c.passive && !NOT_OFFERED.has(c.id)).map((c) => c.id));

  function sheetFor(host) {
    return store.ensure(host.name, () => {
      if (!host.choices?.length) return null;   // nothing read yet: seed on a later step, not from nothing
      const eligible = (id) => (known(host, id) ? id : EMPTY);
      const hot = (host.hotkeys || []).map((h) => eligible(h.skill)).filter((id) => id >= 0);
      return initialSheet(eligible(host.selected?.left ?? -1), eligible(host.selected?.right ?? -1), hot);
    });
  }

  function release(b) {
    const a = active[b];
    if (!a) return;
    if (a.type === 'pointer') io.pointer('pointerup', a.x, a.y, { button: a.button, shiftKey: a.shift });
    else io.keyHold(a.fn, false);
    active[b] = null;
  }
  const monsters = (host) => (host.ranked || []).filter((t) => t.kind === 'unit');
  const clickAt = (x, y, button, shiftKey = false) => {
    io.pointer('pointermove', x, y, { button, shiftKey });
    io.pointer('pointerdown', x, y, { button, shiftKey });
    io.pointer('pointerup', x, y, { button, shiftKey });
  };
  function interact(host) {
    const t = (host.ranked || []).find((r) => r.kind !== 'unit');
    if (t) clickAt(t.screen[0], t.screen[1], 0);
  }

  // One button's skill, on its press or while it is held.
  function cast(host, id, press, b) {
    const info = choiceOf(host, id);
    if (!info || info.passive) return;
    if (info.aura) {
      if (!press) return;
      const current = host.selected?.right ?? -1, currentAura = Boolean(choiceOf(host, current)?.aura);
      const choice = auraChoice(currentAura ? current : 0, id, current, currentAura, auraReturn);
      auraReturn = choice.remembered;
      if (choice.next >= 0) io.selectSkill(choice.next, 'right');
      return;
    }
    if (isPortal(id)) {
      if (press && !host.inTown && host.hero) { io.selectSkill(id, 'right'); clickAt(host.hero[0], host.hero[1], 2); }
      return;
    }
    if (!press && host.inTown) return;
    const left = id === 0 || info.leftSkill, button = left ? 0 : 2, side = left ? 'left' : 'right';
    if (press) {
      if ((left ? host.selected?.left : host.selected?.right) !== id) io.selectSkill(id, side);
      const pick = (host.ranked || [])[0];
      if (id === 0 && pick && pick.kind !== 'unit') { used[b] = true; clickAt(pick.screen[0], pick.screen[1], 0); return; }
      const foe = monsters(host)[0];
      const at = foe ? foe.screen : host.ahead;
      if (!at) return;
      if (id === 0 && !foe) return;   // Attack on empty ground would walk; the press does nothing
      const shift = !foe && left;     // a left-hand skill at the ground casts in place, as shift-click
      io.pointer('pointermove', at[0], at[1], { button, shiftKey: shift });
      io.pointer('pointerdown', at[0], at[1], { button, shiftKey: shift });
      active[b] = { type: 'pointer', x: at[0], y: at[1], button, shift, key: foe ? `unit${foe.id}` : null };
      return;
    }
    // Held: follow the monster that was hit, else the next one; never onto anything else.
    const a = active[b];
    if (!a || a.type !== 'pointer') return;
    const foes = monsters(host);
    const foe = foes.find((t) => `unit${t.id}` === a.key) || foes[0];
    if (foe) {
      a.key = `unit${foe.id}`;
      if (foe.screen[0] !== a.x || foe.screen[1] !== a.y) { a.x = foe.screen[0]; a.y = foe.screen[1]; io.pointer('pointermove', a.x, a.y, { button: a.button, shiftKey: a.shift }); }
    } else if (id === 0 || a.key) { release(b); used[b] = true; }
  }

  function perform(host, id, press, down, b) {
    if (press || !down) used[b] = false;
    if (used[b]) return;
    const fn = panelFn(id);
    if (fn) { if (press) io.fnKey(fn); return; }
    if (id === INTERACT) { if (press) interact(host); return; }
    if (id === SHOW_ITEMS) { if (press) { release(b); io.keyHold('showItems', true); active[b] = { type: 'key', fn: 'showItems' }; } return; }
    if (id < 0) return;
    cast(host, id, press, b);
  }

  function openAssignment(host, id, fromTree) {
    ui.fromTree = Boolean(fromTree);
    ui.actions = id < 0;
    const ids = wheelIds(host);
    ui.at = Math.max(0, ids.indexOf(id));
    ui.wheel = true;
    ui.feedback = '';
    if (fromTree) io.fnKey('skillTree');   // the tree closes under the wheel and comes back on Done
  }
  function closeWheel() {
    ui.wheel = false;
    if (ui.fromTree) io.fnKey('skillTree');
    ui.fromTree = false;
  }

  function driveWheel(host, act) {
    const sheet = sheetFor(host);
    if (act.wheelTab) { ui.actions = !ui.actions; ui.at = 0; }
    const ids = wheelIds(host), count = ids.length;
    if (!count || !sheet) { ui.wheel = false; return; }
    if (act.cancel) { closeWheel(); return; }
    if (ui.at >= count) ui.at = 0;
    if (act.wheelStep) ui.at = nextPage(ui.at, count, act.wheelStep);
    const start = Math.floor(ui.at / WHEEL_SHOWN) * WHEEL_SHOWN;
    if (!act.wheelStep && act.aim) {
      const picked = stableWedge(act.aim[0], act.aim[1], Math.min(WHEEL_SHOWN, count - start), ui.at - start);
      if (picked >= 0) ui.at = start + picked;
    }
    const chosen = ids[ui.at];
    let label = '';
    if (act.slot >= 0) { setSlot(sheet, act.modifierHeld, act.slot, chosen); label = (act.modifierHeld ? 'LT+' : '') + SLOT_LABEL[act.slot]; }
    if (act.main) { if (act.modifierHeld) sheet.altMain = chosen; else sheet.main = chosen; label = act.modifierHeld ? 'LT+RT' : 'RT'; }
    if (!label) return;
    if (sheet.main === UNSET) sheet.main = host.selected?.right ?? EMPTY;
    ui.feedback = `${label} . ${chosen >= 0 ? `skill ${chosen}` : actionLabel(chosen, true)}`;
    store.save(host.name, sheet);
  }

  function driveStrip(act) {
    const n = PANELS.length;
    if (act.cancel) { ui.strip = -1; return; }
    if (act.stripStep) ui.strip = (ui.strip + (act.stripStep > 0 ? 1 : n - 1)) % n;
    if (!act.confirm) return;
    io.fnKey(PANELS[ui.strip][1]);
    ui.strip = -1;
  }

  function releaseAll() { for (let b = 0; b <= SLOTS; b++) release(b); }

  return {
    ui,
    store,
    get uiMode() { return ui.wheel ? 'wheel' : ui.strip >= 0 ? 'strip' : 'none'; },
    sheet: (host) => sheetFor(host),
    wheelIds: (host) => wheelIds(host),
    openAssignment,
    releaseAll,
    // One sample's actions. `field` is the world with nothing open over it.
    step(host, act, { field }) {
      if (ui.wheel) { releaseAll(); driveWheel(host, act); return; }
      if (ui.strip >= 0) { releaseAll(); driveStrip(act); return; }
      if (!field || host.dead) { releaseAll(); ui.hint = ''; return; }
      const sheet = sheetFor(host);
      if (act.openWheel) { releaseAll(); openAssignment(host, sheet ? slotOf(sheet, false, 0) : 0, false); return; }
      if (act.openStrip) { releaseAll(); ui.strip = 0; return; }
      if (act.swapTap && host.expansion !== false) io.fnKey('swap');
      if (act.interact) interact(host);
      if (!sheet) return;
      const setT = triggerHold(act.main, act.mainDown, act.modifierHeld);
      if (setT >= 0) {
        let id = triggerOf(sheet, setT === 1);
        if (id === UNSET) id = host.selected?.right ?? EMPTY;
        perform(host, id, act.main, act.mainDown, MAIN);
      }
      for (let i = 0; i < SLOTS; i++) {
        const set = slotHold(i, act.slot === i, act.slotDown[i], act.modifierHeld);
        if (set >= 0) perform(host, slotOf(sheet, set === 1, i), act.slot === i, act.slotDown[i], i);
      }
      // A button let go releases whatever it held.
      for (let i = 0; i < SLOTS; i++) if (!act.slotDown[i]) release(i);
      if (!act.mainDown) release(MAIN);
      // The Attack / Use hint: the button that holds Attack, else RT, else R3 for Use.
      const alt = act.modifierHeld, pick = (host.ranked || [])[0];
      const slot = [0, 1, 2, 3, 4, 5].find((i) => slotOf(sheet, alt, i) === 0);
      ui.hint = slot !== undefined ? `${(alt ? 'LT+' : '') + SLOT_LABEL[slot]} . ${pickLabel(pick)}`
        : triggerOf(sheet, alt) === 0 ? `${alt ? 'LT+RT' : 'RT'} . ${pickLabel(pick)}`
          : `R3 . ${pickLabel((host.ranked || []).find((r) => r.kind !== 'unit'))}`;
    },
  };
}
