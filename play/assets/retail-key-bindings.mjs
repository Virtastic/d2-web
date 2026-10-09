// Browser keydowns that must not reach CommandInput's legacy physical-key switch. A configured
// retail action is resolved and handled above this set; if that binding moves or is unbound, the
// old fixed shortcut must not remain active underneath it.
//
// Retail 1.13c's oracle for its own binding set is its Configure Controls function list, which is the
// engine's own data: `DataTbls/StringIds.h`'s `Cfg*` strings (3924-3985) name every function a player
// can bind -- Character, Inventory, Party, MessageLog, QuestLog, Chat, AutoMap, AutoMapCenter,
// MiniMap, Help, SkillTree, SkillPick, Skill1-8, skillup, skilldown, BeltShow, Belt1-12, Say0-6, Run,
// RunLock, StandStill, ShowItems, ClearScreen, Snapshot, TogglePortraits, AutoMapFade/Names/Party --
// and **there is no movement function among them**: retail walks by clicking, so no key walks the
// hero. OpenDiablo2's `d2game/d2player/key_map.go` cross-checks the rest (T is the skill tree, W is
// weapon swap), and its `game_controls.go` moves the CAMERA with the arrows, in FreeCam only.
//
// The drop happens in the shell's own keydown handler (`play/gameplay-shell.mjs`) rather than as a DOM
// filter: it has to sit BELOW the Esc menu, which keeps the arrows for nav, and it has to apply on the
// front-end screens too -- a key pressed on the title or select screen used to queue a command that
// ran the moment a game started. Nothing here is a test runner restriction: `__d2Shell.input` is an
// aiming surface scenarios drive (s92 walks a subtile with `input.key('KeyA', true)`), and it is
// untouched.
export const DROPPED_GAMEPLAY_KEYS = new Set([
  'KeyD',            // unbound in retail (and in the prior art)
  'KeyE',            // unbound in retail; this shell's old interact shortcut
  'KeyR',            // retail: CfgRunLock, the run/walk toggle -- the run action is another row (s630+), never a respawn
  // These keys have native Configure Controls actions, but CommandInput also carries old fixed
  // browser shortcuts for them. Their configured action is dispatched above this set; when a
  // player rebinds/unbinds that action, do not leave the physical key's former shortcut active.
  'KeyW', 'KeyA', 'KeyS', 'KeyT', 'Space',
  'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8',
  'Digit1', 'Digit2', 'Digit3', 'Digit4',
  // The configured automap functions above own these keys. A cleared or rebound action must not
  // expose a legacy physical-key path underneath it.
  'Tab', 'KeyV', 'F9', 'F10', 'F11', 'F12',
  'ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight',   // unbound in retail: no movement function exists
]);
// Bound actions are resolved from Configure Controls' committed retail function/key table before
// this drop list (`configure-controls.mjs`, F218). The shell connects the actions it implements;
// any physical key with no such action must still not become movement or an attack by falling into
// CommandInput. the comments carries the captured table and supported-action boundary.
