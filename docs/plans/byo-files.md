# Bring your own files (BYO) -- plan

Goal: a public build that serves **zero Blizzard bytes**. The player points the page at their own Diablo II
MPQs, the page recognises them, and Play unlocks. Everything stays in the browser. Draft for review; the
legal section needs counsel before launch.

## 0. What the code gives us today (read, not assumed)

- `play/index.html` HEADs `retail/<name>` for each name in `RETAIL_ORDER` (11 MPQs), mounts each at
  `/retail/<name>` through `StreamFS.mount` (URL, Range requests), then `d2_vfs_mount` per archive. The
  engine only ever sees `/retail/<name>` paths.
- `play/streamfs.js` already has **`mountLocal(path, FileSystemFileHandle, size)`** and
  **`mountBlob(path, Blob, size)`** (a `File` is a `Blob`). Reads slice from disk on demand, nothing is
  copied into memory. Its header says it is "how user-supplied MPQs get read without hosting them". No
  engine or streamfs change is needed to read local files.
- `play/assets/mpq-index.mjs` parses an MPQ v0 header, hash table and block table in JS: the detector's base.
- `play/serve.mjs` serves `/retail/` only when `D2_RETAIL_DIR` is set; `tools/export-public.sh` already
  fails the public export on any `.mpq/.dll/.exe`. The public tree has no game data today.
- `play/entry.js` is the only reader of the page query; `play/assets/bnet-servers.json` is the precedent for
  "project-configured, read at boot, never a player-facing input".

So BYO is a **new front door plus a new byte source**, not an engine change.

## 1. The flag

`play/site-config.json`, fetched at boot and exposed by `entry.js` (not a URL param, so a player cannot flip it):

```json
{ "assets": "hosted" }     // LAN / dev / harness: today's behaviour, unchanged (default)
{ "assets": "byo" }        // public host: no /retail/ route exists, the file door is shown
```

- `ci/stage.sh` takes `D2_ASSETS=byo` and writes the file; `tools/export-public.sh` writes `byo` into the
  public tree, so a public checkout can never accidentally look for hosted data.
- `assets=byo` also makes `serve.mjs` refuse `/retail/` even if `D2_RETAIL_DIR` is set.
- A TEST-only `?assets=byo` (listed with the other kept test params in `entry.js`) lets us exercise the door
  on the LAN build and in the harness.

## 2. The front door

One card, nothing else on the page. No log, no HUD, no controls list until the game is running.

```
            DIABLO II  .  WEB

   Bring your own copy of Diablo II: Lord of Destruction.
   Your files never leave this device.

   [x] Diablo II data        d2data.mpq
   [x] Characters            d2char.mpq
   [x] Sound effects         d2sfx.mpq
   [x] Lord of Destruction   d2exp.mpq
   [ ] Patch 1.13c           Patch_D2.mpq          <- needs this
   [ ] Music                 d2music.mpq  d2xmusic.mpq   (optional)
   [ ] Cinematics / speech   d2video, d2xvideo, d2speech, d2xtalk   (optional)

   [ Choose files... ]     or drop a folder here

   [  Play  ]   (disabled until the required rows are ticked)
```

Behaviour:

- **One button** (`Choose files`, with a quiet "or choose a folder" link) and a drop target. Any mix of
  files, a whole install folder, or a disc ISO (phase 3) is accepted. Files are matched by **content, not
  name** (section 3), so `D2DATA.MPQ`, `d2data (1).mpq` and a renamed file all work.
- Rows tick as files are recognised, live. Required rows gate Play; optional rows only say what they unlock
  ("without these: no cinematics, no music"). Which archives are truly required is **measured** in phase 1
  (boot with each one removed), not guessed; the list above is the starting hypothesis.
- **Returning visitors:** on Chromium the file handles are kept in IndexedDB and the card collapses to
  `Play` plus a small "Change files". The first click re-grants permission (the Play click is the user
  gesture `requestPermission` needs).
- **No jargon, one sentence per problem.** Wrong or missing file messages name the file and the fix
  ("This is Patch_D2 from 1.10f; this build needs 1.13c"), never a hash or an error code. Detail lives behind
  a "Details" disclosure.
- Reuses `frontdoor-error.mjs` for errors and the existing unsupported-browser screen for missing
  SharedArrayBuffer / COOP-COEP; the door only appears once the browser probe passes.

## 3. Detecting what the user gave us

Pure JS, no engine, unit-testable in node (`play/assets/byo-detect.mjs`, built on `mpq-index.mjs`).

Per file, cheaply (no full-file hashing of a 400 MB MPQ):

1. Read the first 4 KB. Magic `MPQ\x1a` at offset 0 (also scan 512-byte boundaries, which is how an MPQ
   appended to a self-extracting installer is found). Not an MPQ -> classify the container (CASC `Data/`
   folder, `.exe`, `.iso`, `.zip`) and give the specific message.
2. Read header + hash table + block table (tens of KB). Check archive size in header vs file size
   (truncated download -> "this file is incomplete").
3. **Fingerprint = SHA-256 of the block table + header fields** (cheap, and it identifies the exact build
   of an archive). Look it up in `play/assets/known-mpqs.json`: `{ role, version, name, size, fingerprint }`.
   The list holds only hashes and sizes, which are facts about files, not Blizzard content.
4. Unknown fingerprint -> fall back to structure: probe (listfile)/hash for tell-tale internal names
   (`data\global\excel\Levels.txt`, `data\global\ui\FrontEnd\...`, `data\local\...`) to assign a *role*
   (data/char/sfx/exp/music/video/patch) with `version: unknown`.
5. Canonical name is chosen by role, so the mount path is always `/retail/<canonical>` regardless of what
   the user's file was called.

`tools/mpq-fingerprint.mjs <file...>` prints the entry; we seed the list from our own retail copy and accept
community contributions of fingerprints (hashes only).

## 4. The version problem (1.13c vs whatever they have)

Key observation to verify in phase 1: the base archives (`d2data`, `d2char`, `d2sfx`, `d2music`, `d2speech`,
`d2video`, `d2exp`, `d2x*`) are dated 2000-2001 and were not touched by later patches. **The version lives in
`Patch_D2.mpq`** (the patch overlay: tables and fixes), so "which version do you have" mostly reduces to "which
Patch_D2 do you have". The fingerprint list will confirm or refute that.

Outcome per patch fingerprint, in order:

| user has | result |
|---|---|
| Patch_D2 = 1.13c | Play. |
| a different MPQ-era patch (1.07-1.13d) | **Compatibility ladder**, below. |
| no Patch_D2 but base archives | "Add the 1.13c patch" with a link to Blizzard's own download page. |
| the Blizzard patch installer (`.exe`) | extract the embedded MPQ in the browser (section 3.1 scan) -> becomes a Patch_D2 row. **To verify:** the 1.13c installer embeds a plain MPQ; if it does, this is the cleanest path. |
| 1.14+ or D2R (CASC `Data/` folder, no MPQs) | clear message: this build reads MPQ-era installs; no workaround is offered. |

**Compatibility ladder** (so it "works anyways" without pretending):

1. **Verified** -- fingerprint is in `known-mpqs.json` with `compat: verified`. Set only when a recorded
   session passes against that patch (deterministic evidence, per the project rules). Today: 1.13c only.
2. **Likely** -- an MPQ-era patch we have not run. Play is enabled with one honest line ("This is patch 1.12;
   this build targets 1.13c, so some things may differ") and the engine boots in the same mode. The
   first player who reports a patch moves it up the ladder via a recorded session, not by claim.
3. **Unknown** -- structure matched a role but nothing else. "Play anyway (unverified)" as a quiet link,
   not the primary button.
4. **Incompatible** -- known-bad (CASC, wrong game, corrupt). No play.

Not done, deliberately: shipping binary patch deltas (xdelta/bsdiff) that turn 1.12's Patch_D2 into 1.13c's.
That is Blizzard-derived data on our host. If we ever want it, it is a separate decision for counsel.

## 5. How the bytes reach the engine

New `play/assets/byo-source.mjs` returns the same list shape the boot code already builds from HEAD
requests -- `{ f: canonicalName, size, source }` -- where `source` is one of:

- **Chromium (File System Access):** `showOpenFilePicker` / `showDirectoryPicker`; handles go to IndexedDB
  and mount through `StreamFS.mountLocal`. Zero copy, zero extra disk, persists across visits.
- **Firefox / Safari (no picker API):** `<input type=file multiple webkitdirectory>` and drag-drop give
  `File` objects, mounted through `StreamFS.mountBlob`. Session-only by default. Optional "Remember on this
  device" copies the files into OPFS (check `navigator.storage.estimate()`; the full set is ~2 GB, so offer
  only the required subset by default and show the size before copying).
- **iOS Safari:** large-file memory limits are the risk; ship Chromium/desktop-Firefox first, measure
  iOS, and state support in the unsupported screen like every other capability.

Boot changes in `play/index.html` (small, one seam): in `assets=byo` the HEAD loop is replaced by
`await byoSource.ready()` and the `preRun` mounts use `mountLocal` / `mountBlob` instead of `mount`. The
`tablesIn` prefetch is skipped for local sources (a local slice is not a network stall) and re-added only if
a measurement says the first-frame cost needs it. Engine init does not start until files are accepted, so a
visitor with no files loads only the small door, not the wasm.

## 6. Legal and DMCA posture (not legal advice; counsel before launch)

What keeps the host clean:

- The server hosts code we wrote and nothing from Blizzard: no MPQ, no patch, no installer, no delta, no
  sample data, no screenshots of retail art in the repo or the page. Fingerprints are hashes and sizes.
- Files never leave the device: the selection UI says "Choose files", never "Upload"; no network request
  carries a file, a filename or a fingerprint. CSP `connect-src 'self'` (plus the project-configured realm
  list) makes that checkable, and a test asserts it.
- No telemetry on what was selected. If analytics exist at all they record "BYO door shown / Play pressed".
- The page tells people to use their own copy, links only to Blizzard's official pages for the patch, and
  contains no instructions for obtaining files elsewhere and no key or copy-protection bypass.
- `/legal` page: not affiliated with Blizzard; trademark notice; a published takedown contact and a registered
  DMCA agent for the host.

Counsel should be told plainly about the larger exposure, which is not the BYO flow: the code itself is
**decompile-derived** (the project never says clean-room). That is the question a rights holder would raise
about the hosted code; the BYO design only removes the "you are distributing the game data" claim.

## 7. Phases (each lands with its test; no screenshots as evidence)

1. **Detector + fingerprints.** `byo-detect.mjs`, `known-mpqs.json`, `tools/mpq-fingerprint.mjs`; node tests
   over `tools/mkmpq.mjs` synthetic archives (valid, truncated, wrong magic, renamed, CASC folder, exe-embedded)
   plus a retail-gated test that our own set resolves to the right roles. Measure the minimum required set
   (boot with each archive withheld) and fill the required/optional tiers from that.
2. **Flag + source seam.** `site-config.json`, `entry.js`, `stage.sh`/`export-public.sh` wiring, `serve.mjs`
   refusal, `byo-source.mjs` (handles + Files), boot seam in `index.html`. **Gate:** a BYO boot from `File`
   objects of our retail dir produces records identical to the hosted boot (same first-N-tick records), so the
   new path provably changes nothing the parity rows compare.
3. **The door.** Markup/CSS, live ticking rows, drop target, remembered handles, error copy, a scenario in
   the style of `s940` driving it with `setInputFiles` (states: empty, partial, wrong file, wrong version,
   ready, permission re-grant). Disc ISO read (ISO9660 in JS, no extraction) if the discs we hold contain the
   MPQs as plain files.
4. **Compat ladder content.** Patch-installer extraction (after verifying the embedded-MPQ hypothesis),
   version messages, `compat` field driven by recorded-session results as other patches become available.
5. **Public hosting.** COOP/COEP headers on the host, `/legal`, CSP, the DMCA agent registration, README and
   announce-doc updates (they currently say "you need your own copy"; this makes that true in the UI),
   export-gate additions (fail on `site-config.json` != byo, fail on any fingerprint list entry with a name
   that is a path into retail content).

## 8. Decisions made (so nothing waits on a question)

- Mode lives in a served config, not the URL (player cannot enable hosted mode).
- Identify by content fingerprint, name-agnostic; canonicalise the mount path.
- Patch deltas are out; installer extraction and the compat ladder are in.
- CASC / 1.14 / D2R are explicitly unsupported with a clear message.
- Chromium-first persistence; Firefox/Safari session-only with optional OPFS copy.
- Required vs optional archives are decided by measurement in phase 1, not by this document.

## 9. Open items to verify (facts I did not confirm)

- That `Patch_D2.mpq` is the only version-dependent archive (fingerprints across 1.07-1.13d will tell).
- That Blizzard's 1.13c patch installer embeds a plain MPQ readable by the 512-byte scan.
- Whether our discs/ISOs expose the MPQs as plain files for phase 3.
- iOS Safari behaviour with ~2 GB of `File` slices.
