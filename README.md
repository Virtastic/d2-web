# d2-web

**Play Diablo II: Lord of Destruction in your browser.** The 1.13c engine, reimplemented and compiled to WebAssembly, by [Virtastic](https://virtastic.app).

<p align="center">
  <a href="https://diablo2.virtastic.app"><img src="docs/screenshots/act1.png" alt="A Barbarian on a path through the Blood Moor in Act 1, running in a browser tab at diablo2.virtastic.app" width="850"></a>
</p>

<p>
  <a href="https://diablo2.virtastic.app"><b>▶ Play now at diablo2.virtastic.app</b></a> ·
  <a href="https://virtastic.app">Website</a> ·
  <a href="https://discord.gg/PzFfDkbSue">Discord</a> ·
  <a href="https://www.youtube.com/@Virtastic-Apps">YouTube</a> ·
  <a href="https://github.com/Virtastic/d2-web/issues">Issues</a>
</p>

[![Discord](https://img.shields.io/badge/Discord-join%20the%20server-5865F2?logo=discord&logoColor=white)](https://discord.gg/PzFfDkbSue)
[![Support on Patreon](https://img.shields.io/badge/Patreon-support-F96854?logo=patreon&logoColor=white)](https://www.patreon.com/virtastic)
[![Support on Ko-fi](https://img.shields.io/badge/Ko--fi-buy%20us%20a%20coffee-FF5E5B?logo=ko-fi&logoColor=white)](https://ko-fi.com/virtastic)
[![YouTube](https://img.shields.io/badge/YouTube-Virtastic-FF0000?logo=youtube&logoColor=white)](https://www.youtube.com/@Virtastic-Apps)
[![Reddit](https://img.shields.io/badge/Reddit-r%2Fvirtastic-FF4500?logo=reddit&logoColor=white)](https://www.reddit.com/r/virtastic)
![License: MIT](https://img.shields.io/badge/license-MIT-blue)
![Platform: Chrome desktop](https://img.shields.io/badge/browser-Chrome%20%2F%20Chromium-brightgreen)

An unofficial reimplementation of the Diablo II: Lord of Destruction **1.13c** engine, compiled to
**WebAssembly** and played in the browser. No game data is included: you bring your own legally
owned copy.

> **Status: playable.** Create a character and play: towns, wilderness and dungeons, combat and
> skills, items, vendors, waypoints and saves, in a browser tab with nothing to install. Some
> details still differ from the original game. If you find one, please
> [report it](https://github.com/Virtastic/d2-web/issues).

## Contents

- [Before you start](#before-you-start)
- [Choose how to play](#choose-how-to-play)
- [Option A: Play in your browser](#option-a-play-in-your-browser)
- [Option B: Run your own server with Docker](#option-b-run-your-own-server-with-docker)
- [Option C: Build from source](#option-c-build-from-source)
- [Play on other devices on your network](#play-on-other-devices-on-your-network)
- [Widescreen menu backgrounds (optional)](#widescreen-menu-backgrounds-optional)
- [Build and publish your own Docker image](#build-and-publish-your-own-docker-image)
- [Troubleshooting](#troubleshooting)
- [Tests](#tests) · [Layout](#layout) · [Contributing](#contributing) · [Credits](#credits) · [Legal](#legal)

## Before you start

You need two things:

1. **Your own copy of Diablo II and Lord of Destruction**, installed on your computer.
   d2-web ships **no Blizzard files**: no art, sound, text, maps or programs. It reads them from
   your install.
2. **A desktop Chrome, Edge or other Chromium browser.**

The game files are the `.mpq` files in the folder Diablo II is installed in:

| file | what it holds |
|---|---|
| `d2data.mpq` | the world: maps, monsters, items |
| `d2exp.mpq` | Lord of Destruction |
| `d2char.mpq` | the heroes |
| `d2sfx.mpq` | sound effects |
| `d2speech.mpq`, `d2xtalk.mpq` | speech |
| `d2music.mpq`, `d2xmusic.mpq` | music |
| `d2video.mpq`, `d2xvideo.mpq` | cinematics |
| `patch_d2.mpq` | the patch |

> [!TIP]
> **Where is my Diablo II folder?** It is the folder that holds `d2data.mpq`. It is usually
> `C:\Program Files (x86)\Diablo II` on Windows and `/Applications/Diablo II` on macOS. From
> WSL2, the Windows folder is `/mnt/c/Program Files (x86)/Diablo II`.

## Choose how to play

| | you need | best for |
|---|---|---|
| **[A. Play in your browser](#option-a-play-in-your-browser)** | a browser | just playing |
| **[B. Run your own server with Docker](#option-b-run-your-own-server-with-docker)** | Docker | hosting it yourself, at home or on a server |
| **[C. Build from source](#option-c-build-from-source)** | Docker, Node.js 24+, git, and a game **patched to 1.13c** | changing the code |

## Option A: Play in your browser

1. Open **<https://diablo2.virtastic.app>** in Chrome.
2. Click **CHOOSE FOLDER** and pick your Diablo II folder. You can also click **CHOOSE FILES**,
   or drag the `.mpq` files onto the page.
3. When the page says **All files found.**, click **ENTER SANCTUARY**.
4. On the main menu, click **SINGLE PLAYER**, pick a hero, type a name and click **OK**.

| the main menu | creating a hero |
|---|---|
| ![The Diablo II: Lord of Destruction main menu, with Single Player, Battle.net and Other Multiplayer buttons](docs/screenshots/title.png) | ![Select Hero Class: the heroes standing around the campfire, the Barbarian selected](docs/screenshots/class-select.png) |

*Screenshots are rendered from the player's own game files; the art is Blizzard's.*

> [!NOTE]
> **Your files never leave your computer.** The page reads them inside your browser; nothing is
> uploaded. Next time, click **Use the files from last time** and skip step 2.

> [!TIP]
> **Any patch level works here.** The page supplies the 1.13c patch data itself, so an
> unpatched install, a 1.14 install or a 1.13c install all play the same.

Hover over a file on the file screen to see whether it was found. Saves are kept in your
browser's storage on that computer. Battle.net is off; Single Player is the way in.

## Option B: Run your own server with Docker

The image `ghcr.io/virtastic/d2-web` holds the engine and the web page, and **no game data**.

**Step 1.** Install [Docker](https://docs.docker.com/get-docker/) and start it.

**Step 2.** Start the server:

```bash
docker run -d --name d2-web --restart unless-stopped -p 8080:8080 ghcr.io/virtastic/d2-web:latest
```

**Step 3.** Open <http://localhost:8080/> and continue from
[Option A, step 2](#option-a-play-in-your-browser): each player chooses their own game folder in
their own browser.

> [!IMPORTANT]
> **Use HTTPS for anything other than `localhost`.** The engine needs a browser feature
> (`SharedArrayBuffer`) that browsers only allow on `https://` or `localhost`. Put HTTPS in front
> of the container, for example with Caddy, nginx or a Cloudflare Tunnel.

<details>
<summary><b>Serve the game files from the server instead (<code>D2_ASSETS=hosted</code>)</b></summary>

By default (`D2_ASSETS=byo`, "bring your own") the server holds no game files and each player
picks their own. With `D2_ASSETS=hosted`, the server serves the `.mpq` files from a folder you
mount at `/retail`, and players just open the page:

```bash
docker run -d --name d2-web --restart unless-stopped -p 8080:8080 \
  -e D2_ASSETS=hosted -v "/path/to/Diablo II:/retail:ro" ghcr.io/virtastic/d2-web:latest
```

Or with Compose:

```yaml
services:
  d2-web:
    image: ghcr.io/virtastic/d2-web:latest
    restart: unless-stopped
    ports: ["8080:8080"]
    environment:
      D2_ASSETS: hosted                         # or byo, and drop the volume
    volumes: ["/path/to/Diablo II:/retail:ro"]
```

> [!WARNING]
> Anyone who can open the page can download those files. Use hosted mode only for players who
> own the game, for example behind a login such as Cloudflare Access.

</details>

## Option C: Build from source

You need Linux, macOS, or Windows with WSL2, plus:

| tool | why | get it |
|---|---|---|
| Docker | the engine compiles inside a container, so you do not install a compiler | <https://docs.docker.com/get-docker/> |
| Node.js 24 or newer | runs the setup, the build helpers and the local server | <https://nodejs.org> |
| git | downloads the code | <https://git-scm.com> |
| Diablo II + Lord of Destruction, **patched to 1.13c** | used to create the delta patch for users, to ensure they are on 1.13c | your own copy |

### Step 1. Download the code

```sh
git clone --recursive https://github.com/Virtastic/d2-web.git
cd d2-web
```

`--recursive` also downloads StormLib, the library that reads `.mpq` files. If you forget it, the
next step fetches it for you.

### Step 2. Point it at your game

```sh
node tools/setup.mjs "/path/to/Diablo II"
```

This one command does all of the game-file setup:

- finds your `.mpq` files, whatever their capitalisation;
- links them into `retail/D2/`, where the build and the local server look for them, without
  copying (git ignores that folder, so game files can never be committed);
- checks that `patch_d2.mpq` is the 1.13c patch;
- checks that Docker is running and that StormLib is present.

When everything is in place it prints:

```text
d2-web setup

Game files from /path/to/Diablo II
  ✓ patch_d2.mpq     The 1.13c patch  linked
  ✓ d2exp.mpq        Lord of Destruction  linked
  ✓ d2data.mpq       Maps, monsters, items  linked
  ✓ d2char.mpq       The heroes  linked
  ...
  ✓ d2xvideo.mpq     Expansion cinematics  linked

Build tools
  ✓ Node.js          24.11.0
  ✓ Docker           28.5.1
  ✓ StormLib         third_party/StormLib

✓ Ready to build. Next:

    bash ci/build.sh
    D2_RETAIL_DIR=retail/D2 node play/serve.mjs 8920 build-wasm/site

  then open http://localhost:8920 in Chrome.
```

Fix any line marked **✗**, then run the command again; it is safe to repeat. A line marked **!**
is a file the game needs to play but the build can do without.

> [!TIP]
> **Your patch is not 1.13c?** Setup tells you. Either update the game to 1.13c, or keep your
> install as it is and point setup at a 1.13c `Patch_D2.mpq` you have elsewhere:
> `node tools/setup.mjs "/path/to/Diablo II" --patch /path/to/1.13c/Patch_D2.mpq`

### Step 3. Build

```sh
bash ci/build.sh
```

This builds a compiler image, compiles the engine to WebAssembly and writes the finished site to
`build-wasm/site/`. The first run takes a while; later runs reuse what is already built. Near the
end, the log prints:

```text
patch shim: retail/D2/patch_d2.mpq (8229959 bytes, 1.13c) -> build-wasm/site/assets/patch-113c.patch
door-art: ... images -> build-wasm/site/door-art
```

> [!NOTE]
> Changed only JavaScript in `play/`? `bash ci/stage.sh` restages the site in seconds without
> recompiling the engine.

### Step 4. Play

```sh
D2_RETAIL_DIR=retail/D2 node play/serve.mjs 8920 build-wasm/site
```

Open <http://localhost:8920/> in Chrome.

`play/serve.mjs` is a small Node server with no dependencies. It serves the site, serves your
`.mpq` files from `D2_RETAIL_DIR`, and sends the cross-origin isolation headers (COOP/COEP) that
the engine's threads need.

> [!IMPORTANT]
> **A plain static file server will not work**: it does not send those headers. Use
> `play/serve.mjs` or the Docker image.

<details>
<summary><b>What the build takes from your game files, and why</b></summary>

The front door (the title screen a player sees before choosing their game files) needs two things
that are made from **your** game files every time you build. Neither is ever committed, which is
why a fresh clone does not have them.

| staged path | what it is | made from | made by | without it |
|---|---|---|---|---|
| `assets/patch-113c.patch` | your 1.13c `patch_d2.mpq`, as base64 text | `patch_d2.mpq` | `tools/patch-shim.mjs` | **the build stops** |
| `door-art/` | the title-screen art, drawn before any files are chosen | `patch_d2.mpq`, `d2exp.mpq`, `d2data.mpq` | `tools/door-art.mjs` | warning; the title screen uses a plain web font until the player's files load |
| `freeres/` | the widescreen bottom bar and border panels: 16 `.dc6` files plus their licence | `play/freeres/` in git (SGD2FreeRes art, under its own licence) | `ci/stage.sh` copies it | the build stops |
| `widebg/` | optional widescreen menu backgrounds | images you supply | `ci/stage.sh` copies them | the menus use the game's own 4:3 backgrounds |

**Why the patch file exists.** Players can bring a game at any patch level, but the engine only
reads 1.13c data. The page therefore mounts `patch-113c.patch` as `Patch_D2.mpq` on top of
whatever patch the player chose. The base archives are the same at every patch level, so the
player only has to supply those.

**The patch must be the exact `Patch_D2.mpq` the 1.13c update installs.** `tools/patch-shim.mjs`
compares its size and SHA-256 and refuses anything else, including other patch versions and
modified copies:

| | |
|---|---|
| size | 8,229,959 bytes |
| SHA-256 | `41a5fc95977369e3d5f32127cc4c0d571daad9cf706b6b6523935b705afc5a6d` |

**What `door-art/` contains:**

- the burning logo, in four PNGs: `logo_blackL`, `logo_fireL`, `logo_blackR`, `logo_fireR`;
- the wide button (`wide`), the hand cursor (`cursor`) and the loading screen (`loading`);
- font sheets cut from the game's bitmap fonts:
  - `f_font16_<colour>` in white, red, gold, grey and tan;
  - `f_font30_gold`;
  - `f_fontexocet10_white`;
- `meta.json`, which gives the width of every character in each font.

**The text is not baked into the images.** Each font sheet is a grid with one cell per character.
The words are ordinary strings in `play/assets/byo-door.mjs`, and the door copies one character at
a time from the sheet onto the canvas, the way the game draws text. To change the wording, edit the
code; you never need to edit the art.

The art comes from `data\global\ui\FrontEnd\`, `data\local\font\latin\` and the palettes in
`data\global\palette\`. When a file is in more than one archive, the first one wins, in the order
`patch_d2.mpq`, `d2exp.mpq`, `d2data.mpq`.

`tools/setup.mjs` is the easy way in, but both tools also run on their own:

```sh
node tools/patch-shim.mjs "/path/to/Diablo II/Patch_D2.mpq" build-wasm/site/assets/patch-113c.patch
node tools/door-art.mjs   "/path/to/Diablo II"              build-wasm/site/door-art
```

Instead of `retail/D2/`, the build can also read the patch from `D2_PATCH_SHIM_SRC=/path/to/patch_d2.mpq`,
and then looks for `d2exp.mpq` and `d2data.mpq` in the same folder.

</details>

## Play on other devices on your network

`play/serve.mjs` can listen on your home network too:

| variable | effect |
|---|---|
| `D2_BIND=0.0.0.0` | listen on every interface (default `127.0.0.1` only) |
| `D2_TLS_DIR=<dir>` | serve HTTPS using `<dir>/key.pem` and `<dir>/cert.pem` |
| `D2_RETAIL_DIR=<dir>` | the folder holding your `.mpq` files |
| `PORT` | port, if not given as the first argument |

```sh
D2_BIND=0.0.0.0 D2_TLS_DIR=~/certs D2_RETAIL_DIR=retail/D2 \
  node play/serve.mjs 8443 build-wasm/site
```

> [!IMPORTANT]
> Other devices need **HTTPS**, because browsers only allow the engine's threads on `localhost` or
> over HTTPS. A self-signed certificate, or one from your own certificate authority, is fine on a
> home network.

> [!WARNING]
> The server has **no login**. Whoever can reach it can download the `.mpq` files you point it
> at, and redistributing Blizzard's files is not allowed. Keep it on a private network; never
> expose it to the internet.

Saves live in each device's browser storage. Battle.net is off: d2-web does not run or connect to
any official service.

## Widescreen menu backgrounds (optional)

d2-web opens in a widescreen view by default. Each menu screen (title, game select, character
select, character creation, credits) normally draws the game's own 800×600 background, centred
with black bars on either side. To fill the whole screen instead, supply larger painted
backgrounds:

1. Put a JPG or PNG for each screen in `.widebg/` at the root of the main checkout, or in any
   folder named by `D2_WIDEBG_DIR`. `.widebg/` is gitignored.
2. Add a `manifest.json` in the same folder. Each screen is keyed by the lower-case name of the
   game background it replaces:
   - `trademarkscreenexp` (title screen);
   - `gameselectscreenexp` (game select);
   - `characterselectscreenexp` (character select);
   - `charactercreationscreenexp` (character creation);
   - `creditsbckgexpand` (credits).

   ```json
   {
     "trademarkscreenexp": { "file": "title.jpg", "w": 2752, "h": 1536, "s": 1.94, "ox": 585.31, "oy": 283.5 },
     "gameselectscreenexp": { "file": "gameselect.jpg", "w": 2752, "h": 1536, "s": 2.5171, "sy": 2.5685, "ox": 373.69, "oy": -3.38 }
   }
   ```

   | field | meaning |
   |---|---|
   | `file` | the image file, in the same folder as `manifest.json` |
   | `w`, `h` | the image's width and height in pixels |
   | `ox`, `oy` | where the game screen's top-left pixel (0,0) sits in the image |
   | `s` | how many image pixels one game pixel covers, horizontally |
   | `sy` | the same, vertically; defaults to `s` (set it only for an image stretched slightly differently in each direction) |

   These values line your image up with the game's 800×600 screen. Buttons, text and other
   overlays are still drawn at the game's own positions, so the painting has to match the
   original background where the overlays sit.
3. Run `ci/stage.sh` (or `ci/build.sh`). It copies the folder to `build-wasm/site/widebg/`,
   following links.

A screen with no entry, or a missing `widebg/` folder, uses the game's own background. The repo
ships no backgrounds. Use only images you have the right to use.

## Build and publish your own Docker image

After [Option C](#option-c-build-from-source), package the built site as an image:

```sh
tools/docker-release.sh v0.1.1 --no-push  # build the image on this machine only
tools/docker-release.sh v0.1.1            # build, then push <tag> and latest
```

<details>
<summary><b>What <code>docker-release.sh</code> checks before it builds</b></summary>

| staged path | if it is missing |
|---|---|
| `e/<version>/d2.wasm`, `serve.mjs` | stops: run `ci/build.sh` first |
| `assets/patch-113c.patch` | stops: run `node tools/setup.mjs`, then `ci/build.sh`. It must also pass the 1.13c check (`node tools/patch-shim.mjs --check`) |
| `door-art/meta.json` | stops: run `node tools/setup.mjs`, then `ci/build.sh` |
| `freeres/` (16 `.dc6` files and `LICENSE.md`) | stops: run `ci/stage.sh`; these files are in git |
| `widebg/manifest.json` | warns and builds without it: the menus use the game's own 4:3 backgrounds |

The image always leaves out `vectors/`, `diag/` and any link to your game folder.

</details>

## Troubleshooting

| what you see | what to do |
|---|---|
| A blank page, or `SharedArrayBuffer is not defined` | The page is not cross-origin isolated. Use `play/serve.mjs` or the Docker image, and open it on `localhost` or over HTTPS. |
| The file screen says files are missing | Choose the folder that holds `d2data.mpq`. Hover over a file on that screen to see which ones were not found. |
| Setup: `✗ patch_d2.mpq ... not the 1.13c patch` | Update the game to 1.13c, or run setup again with `--patch /path/to/1.13c/Patch_D2.mpq`. |
| Setup: `✗ Docker ... not running` | Start Docker Desktop (or the Docker service) and run setup again. On Linux, make sure your user can run `docker` without `sudo`. |
| Build: `FATAL: no retail/D2/patch_d2.mpq` | Run `node tools/setup.mjs "/path/to/Diablo II"` first. |
| The game will not start when built from source | Start the server with `D2_RETAIL_DIR=retail/D2`, and run `node tools/setup.mjs` to check that every file is in place. |

Still stuck? Ask on [Discord](https://discord.gg/PzFfDkbSue) or
[open an issue](https://github.com/Virtastic/d2-web/issues).

## Tests

```sh
node --test test/*.test.mjs
```

## Layout

| path | what |
|---|---|
| `src/core/` | the engine, by retail DLL (common, game, client, gfx, net, ...) |
| `src/core/d2moo/` | code adapted from D2MOO (MIT) |
| `play/` | the browser client and the dev server |
| `tools/` | setup and build helpers, and the realm protocol tools |
| `tools/setup.mjs` | the one-step game-file setup |
| `ci/` | build scripts |

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Security issues: [SECURITY.md](SECURITY.md).

## Credits

- **D2MOO** (ThePhrozenKeep): part of `src/core/d2moo` is adapted from it.
- **StormLib** (Ladislav Zezula): MPQ archive reading.
- **libd2**, **d2mapapi**, **PlugY**, **d2bs**, **D2Ex2** and the **Phrozen Keep** guides: prior
  art and cross-checks.
- **OpenDiablo2** and **OpenD2**: cross-checks for front-end screen layouts, key bindings and
  file formats (DC6/DCC/COF/PL2/TBL). Neither project's code is used. d2-web is not a fork of
  either one and not based on them. Its engine is reimplemented from the retail 1.13c program.
- **unbikit**: Bink video decoding.

See [NOTICE](NOTICE) for licences. Much of the code was written with heavy help from AI coding
tools, directed and reviewed by a human maintainer.

## Legal

d2-web is a fan project. It is not affiliated with, endorsed by, or sponsored by Blizzard
Entertainment. Diablo and Blizzard Entertainment are trademarks or registered trademarks of
Blizzard Entertainment, Inc. in the U.S. and/or other countries. All game content belongs to its
owners; you must own the game to use this project. The engine was written by studying the retail
1.13c program (it was not written clean-room), and it ships no Blizzard files or data.

The d2-web source code is released under the [MIT License](LICENSE).
