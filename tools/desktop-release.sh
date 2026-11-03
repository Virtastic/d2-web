#!/usr/bin/env bash
# Build the desktop app from the staged site: check the site, copy it with the shipping excludes into
# electron/app-site (git-ignored), then run electron-builder for one OS. Unsigned on every OS.
#   tools/desktop-release.sh [linux|mac|win|--all] [target ...]
#   --all builds linux, win and mac one after another (the cross-build from a Linux host).
#   mac on a non-macOS host defaults to one zip per arch (x64, arm64): a dmg and a universal merge need macOS (see .github/workflows/desktop.yml).
#   Linux rpm needs rpmbuild on PATH; the others need nothing beyond node (the win exe edit uses wine if present).
#   default OS: this host. Targets default to every target electron/builder.yml names for that OS
#   (linux: AppImage, deb, rpm; mac: dmg; win: nsis, portable). Example: tools/desktop-release.sh linux AppImage
# Run ci/build.sh (or ci/stage.sh) first. Needs node and the electron/ dependencies (npm install, run here once).
set -euo pipefail
cd "$(dirname "$0")/.."
SITE=build-wasm/site
APP=electron/app-site

ALL=0
case "${1:-}" in
  --all) ALL=1; OS=linux; shift ;;
  linux|mac|win) OS=$1; shift ;;
  "") case "$(uname -s)" in Linux) OS=linux ;; Darwin) OS=mac ;; *) OS=win ;; esac ;;
  *) echo "usage: desktop-release.sh [linux|mac|win|--all] [target ...]" >&2; exit 2 ;;
esac
TARGETS=("$@")

# The staged site must be a real engine build and carry the browser page's own files.
ENGINE=$(cat "$SITE/engine-version.txt" 2>/dev/null || true)
[ -n "$ENGINE" ] && [ -f "$SITE/e/$ENGINE/d2.wasm" ] && [ -f "$SITE/index.html" ] && [ -f "$SITE/br-check.mjs" ] \
  || { echo "FATAL: no staged site -- run ci/build.sh (or ci/stage.sh)" >&2; exit 1; }
[ "$(ls "$SITE"/freeres/*.dc6 2>/dev/null | wc -l)" -eq 16 ] && [ -f "$SITE/freeres/LICENSE.md" ] \
  || { echo "FATAL: no widescreen HUD art staged (freeres/: 16 DC6 + LICENSE.md)" >&2; exit 1; }
[ ! -L "$SITE/widebg" ] && [ ! -L "$SITE/door-art" ] || { echo "FATAL: a staged art folder is a link; restage with ci/stage.sh" >&2; exit 1; }
[ -f "$SITE/widebg/manifest.json" ] || echo "WARNING: no widescreen menu backgrounds staged (widebg/) -- the menus use the game's 4:3 backgrounds; stage with .widebg/ or D2_WIDEBG_DIR" >&2

# What ships: the same excludes as tools/docker-release.sh. The title-screen art (door-art/) and the 1.13c patch
# shim are cut from the player's game files, so the app makes them at run time from the player's own archives;
# the widescreen menu backgrounds (widebg/) ship, as in the Docker image.
# -h copies files, never links, so a retail link can never be followed into the package.
rm -rf "$APP"; mkdir -p "$APP"
tar -h -C "$SITE" \
  --exclude=./vectors --exclude=./diag --exclude=./retail \
  --exclude=./door-art --exclude=./assets/patch-113c.patch \
  --exclude=./assets/combat-sound-queue.mjs --exclude=./assets/item-cursor-art.mjs \
  --exclude=./assets/waypoint-arrival-evidence.mjs --exclude=./assets/weather-evidence.mjs \
  -cf - . | tar -C "$APP" -xf -
# The app answers site-config.json itself (hosted with a folder, byo without); this copy is the byo default.
echo '{"assets":"byo"}' > "$APP/site-config.json"
# The run-time art generator (electron/lib/derive.mjs runs it on the player's own archives): the stage-time CLI
# and the MPQ reader it uses, with the import re-pointed at the site's assets/. No retail data in either.
mkdir -p "$APP/derive"
cp tools/mpq-node.mjs "$APP/derive/mpq-node.mjs"
sed "s#'../play/assets/door-art-build.mjs'#'../assets/door-art-build.mjs'#" tools/door-art.mjs > "$APP/derive/door-art.mjs"
grep -q "'../assets/door-art-build.mjs'" "$APP/derive/door-art.mjs" || { echo "FATAL: derive/door-art.mjs import not rewritten" >&2; exit 1; }
for leak in vectors diag retail door-art assets/patch-113c.patch; do
  [ ! -e "$APP/$leak" ] || { echo "FATAL: $leak leaked into $APP" >&2; exit 1; }
done
echo "==> app-site: $(du -sh "$APP" | cut -f1) (engine $ENGINE)"

# The Linux packages want a PNG icon; it is rendered from the project's own SVG (never committed: the repo keeps
# no images outside docs/screenshots). electron/build/icon.ico and icon.icns (Windows, macOS) are committed.
if [ "$OS" = linux ] || [ $ALL -eq 1 ]; then
  if [ ! -f electron/build/icon.png ]; then
    if command -v rsvg-convert >/dev/null; then rsvg-convert -w 1024 -h 1024 play/assets/icon.svg -o electron/build/icon.png
    elif command -v magick >/dev/null; then magick -background none -density 1200 play/assets/icon.svg -resize 1024x1024 electron/build/icon.png
    else echo "FATAL: need rsvg-convert or ImageMagick to render electron/build/icon.png from play/assets/icon.svg" >&2; exit 1; fi
  fi
fi

cd electron
[ -d node_modules/electron ] || npm install --no-audit --no-fund
build_os() {
  local os=$1; shift
  local flag="--$os"; [ "$os" = mac ] && flag=--mac
  if [ "$os" = mac ] && [ "$(uname -s)" != Darwin ] && [ $# -eq 0 ]; then set -- zip --x64 --arm64; fi
  if [ $# -eq 0 ]; then npx electron-builder --config builder.yml "$flag"; else npx electron-builder --config builder.yml "$flag" "$@"; fi
}
if [ $ALL -eq 1 ]; then for o in linux win mac; do build_os $o; done; else build_os "$OS" ${TARGETS[@]+"${TARGETS[@]}"}; fi
echo "==> built into electron/dist:"; ls -la dist
