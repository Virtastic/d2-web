#!/usr/bin/env bash
# Build the Android app (android/) from the staged site: copy the site with the shipping excludes into the app's
# assets (git-ignored), render the launcher icons, build with Gradle, then align and sign the APK.
#   tools/android-release.sh [version]          version defaults to electron/package.json's
# Run ci/build.sh (or ci/stage.sh) first. Needs Java 17+, the Android SDK (ANDROID_HOME, default ~/Android/Sdk,
# with platforms;android-35 and build-tools;35.0.0), Gradle 8.9+ on PATH (or GRADLE=<path>), and rsvg-convert or
# ImageMagick for the icons.
# Signing: D2_ANDROID_KEYSTORE (default ~/.config/d2-web/android-preview.keystore) is made on first use with a
# throwaway password; it is a preview key, never committed. Set D2_ANDROID_KEYSTORE_PASS to use your own.
set -euo pipefail
cd "$(dirname "$0")/.."
SITE=build-wasm/site
ASSETS=android/app/src/main/assets/site
RES=android/app/src/main/res
SDK=${ANDROID_HOME:-$HOME/Android/Sdk}
BT=$SDK/build-tools/35.0.0
GRADLE=${GRADLE:-$(command -v gradle || echo "$HOME/Android/gradle-8.11.1/bin/gradle")}
KS=${D2_ANDROID_KEYSTORE:-$HOME/.config/d2-web/android-preview.keystore}
KSPASS=${D2_ANDROID_KEYSTORE_PASS:-d2-web-preview}
VERSION=${1:-$(node -p "require('./electron/package.json').version")}
IFS=. read -r MA MI PA <<< "$VERSION"
CODE=$(( ${MA:-0} * 1000000 + ${MI:-0} * 1000 + ${PA:-0} ))

ENGINE=$(cat "$SITE/engine-version.txt" 2>/dev/null || true)
[ -n "$ENGINE" ] && [ -f "$SITE/e/$ENGINE/d2.wasm" ] && [ -f "$SITE/index.html" ] \
  || { echo "FATAL: no staged site -- run ci/build.sh (or ci/stage.sh)" >&2; exit 1; }
[ -x "$BT/apksigner" ] && [ -x "$GRADLE" ] || { echo "FATAL: need $BT (build-tools 35.0.0) and gradle ($GRADLE)" >&2; exit 1; }

# What ships: the desktop app's excludes, plus the server-side files and the precompressed copies (the APK
# compresses on its own). The page runs in bring-your-own-files mode: the player picks their own archives.
rm -rf "$ASSETS"; mkdir -p "$ASSETS"
tar -h -C "$SITE" \
  --exclude=./vectors --exclude=./diag --exclude=./retail \
  --exclude=./door-art --exclude=./assets/patch-113c.patch \
  --exclude=./assets/combat-sound-queue.mjs --exclude=./assets/item-cursor-art.mjs \
  --exclude=./assets/waypoint-arrival-evidence.mjs --exclude=./assets/weather-evidence.mjs \
  --exclude=./serve.mjs --exclude='*.br' --exclude='*.gz' \
  -cf - . | tar -C "$ASSETS" -xf -
echo '{"assets":"byo"}' > "$ASSETS/site-config.json"
for leak in vectors diag retail door-art assets/patch-113c.patch; do
  [ ! -e "$ASSETS/$leak" ] || { echo "FATAL: $leak leaked into $ASSETS" >&2; exit 1; }
done
bad=$(find "$ASSETS" -type f \( -iname '*.dll' -o -iname '*.exe' -o -iname '*.iso' -o -iname '*.cab' -o -iname '*.mpq' \) ! -path "$ASSETS/smoke.mpq")
[ -z "$bad" ] || { echo "FATAL: game files in the package:"$'\n'"$bad" >&2; exit 1; }
echo "==> site: $(du -sh "$ASSETS" | cut -f1) (engine $ENGINE)"

# Launcher icons from the project's SVG (never committed: the repo keeps no PNGs outside docs/screenshots).
for d in mdpi:48 hdpi:72 xhdpi:96 xxhdpi:144 xxxhdpi:192; do
  dir=$RES/mipmap-${d%%:*}; px=${d##*:}; mkdir -p "$dir"
  if command -v rsvg-convert >/dev/null; then rsvg-convert -w "$px" -h "$px" play/assets/icon.svg -o "$dir/ic_launcher.png"
  elif command -v magick >/dev/null; then magick -background none -density 600 play/assets/icon.svg -resize "${px}x${px}" "$dir/ic_launcher.png"
  else echo "FATAL: need rsvg-convert or ImageMagick for the launcher icons" >&2; exit 1; fi
done

( cd android && ANDROID_HOME=$SDK "$GRADLE" --no-daemon -q assembleRelease -Pd2VersionCode="$CODE" -Pd2VersionName="$VERSION" )
UNSIGNED=android/app/build/outputs/apk/release/app-release-unsigned.apk
[ -f "$UNSIGNED" ] || { echo "FATAL: gradle produced no $UNSIGNED" >&2; exit 1; }

if [ ! -f "$KS" ]; then
  mkdir -p "$(dirname "$KS")"
  keytool -genkeypair -keystore "$KS" -storepass "$KSPASS" -keypass "$KSPASS" -alias d2web -keyalg RSA -keysize 2048 \
    -validity 10000 -dname "CN=d2-web preview" >/dev/null
  echo "==> made preview keystore $KS"
fi
mkdir -p android/dist
OUT=android/dist/d2-web-$VERSION-android.apk
"$BT/zipalign" -f -p 4 "$UNSIGNED" "$OUT.aligned"
"$BT/apksigner" sign --ks "$KS" --ks-pass "pass:$KSPASS" --ks-key-alias d2web --out "$OUT" "$OUT.aligned"
rm -f "$OUT.aligned" "$OUT.idsig"
"$BT/apksigner" verify "$OUT"
echo "==> $OUT ($(du -h "$OUT" | cut -f1))"
