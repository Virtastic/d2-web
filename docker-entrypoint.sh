#!/bin/sh
# D2_ASSETS picks how players get the game files: byo (default) -- each player chooses their own MPQs in the
# browser and the server holds none; hosted -- the server serves the MPQs mounted at /retail.
set -e
case "${D2_ASSETS:-byo}" in
  byo) echo '{"assets":"byo"}' > /app/site-config.json ;;
  hosted) [ -f /retail/d2data.mpq ] || [ -f /retail/D2DATA.MPQ ] || echo "d2-web: D2_ASSETS=hosted but no d2data.mpq in /retail -- mount your D2 folder there" >&2
          echo '{"assets":"hosted"}' > /app/site-config.json ;;
  *) echo "d2-web: D2_ASSETS must be byo or hosted" >&2; exit 1 ;;
esac
exec node serve.mjs "${PORT:-8080}" /app
