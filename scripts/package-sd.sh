#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
NRO="$ROOT/build_switch/XuitchTV.nro"
DIST="$ROOT/dist"
APP="$DIST/switch/XuitchTV"

if [[ ! -f "$NRO" ]]; then
  echo "No existe $NRO. Compila primero XuitchTV.nro." >&2
  exit 2
fi

rm -rf "$DIST/switch" "$DIST/XuitchTV-SD.zip"
mkdir -p "$APP"
cp "$NRO" "$APP/XuitchTV.nro"
cp "$ROOT/config.example.json" "$APP/config.json"
cp "$ROOT/README.md" "$APP/README.txt"
(
  cd "$DIST"
  if command -v zip >/dev/null 2>&1; then
    zip -qr XuitchTV-SD.zip switch
  else
    python3 - <<'PY'
from pathlib import Path
import zipfile
root=Path('switch')
with zipfile.ZipFile('XuitchTV-SD.zip','w',zipfile.ZIP_DEFLATED) as z:
    for p in root.rglob('*'):
        if p.is_file(): z.write(p, p.as_posix())
PY
  fi
)
echo "Paquete SD: $DIST/XuitchTV-SD.zip"
