// Browser keyboard keys a game must not answer, because retail does not bind them or binds them to
// something this client has no action for.
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
  'KeyA', 'KeyD',    // unbound in retail (and in the prior art)
  'KeyE',            // unbound in retail; this shell's old interact shortcut
  'KeyR',            // retail: CfgRunLock, the run/walk toggle -- the run action is another row (s630+), never a respawn
  'ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight',   // unbound in retail: no movement function exists
]);
// Bound now, so they are NOT dropped: `W` is retail's Swap Weapons (its own Configure Controls
// screen binds it; the swap is the engine's `d2_session_command_swap_weapons`, F3), `Space` is
// retail's CfgClearScreen (the HUD hides, the world fills the screen), `KeyS` is its right-hand
// skill picker and `KeyH` opens retail's Help screen --
// OpenDiablo2's key_map.go ties ClearScreen to KeySpace, ToggleRightSkillSelector to KeyS and
// ToggleHelpScreen to KeyH, and all three actions live in the shell's keydown handler above this
// list. the comments carries the table and the per-entry evidence; the rest of retail's
// Configure Controls list (Q quest log, P party, M message log, V mini map, the wheel's skill cycle)
// is still dropped rather than mis-bound -- F3c.
