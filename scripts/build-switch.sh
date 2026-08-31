#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="$ROOT/build_switch"

if [[ ! -d "$ROOT/library/borealis" ]]; then
  "$ROOT/scripts/bootstrap-deps.sh"
fi

if [[ -n "${DEVKITPRO:-}" ]]; then
  if [[ -f "$DEVKITPRO/switchvars.sh" ]]; then
    # shellcheck disable=SC1090
    source "$DEVKITPRO/switchvars.sh"
  fi
  export PATH="$DEVKITPRO/devkitA64/bin:$PATH"
  export PKG_CONFIG_PATH="$DEVKITPRO/portlibs/switch/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
  cmake -S "$ROOT" -B "$BUILD" -DPLATFORM_SWITCH=ON -DCMAKE_BUILD_TYPE=Release
  cmake --build "$BUILD" --target XuitchTV.nro -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)"
  echo "NRO: $BUILD/XuitchTV.nro"
  exit 0
fi

if command -v docker >/dev/null 2>&1; then
  exec "$ROOT/scripts/build-switch-docker.sh"
fi

echo "No se encontro devkitPro (DEVKITPRO) ni Docker." >&2
echo "Tambien puedes usar el workflow .github/workflows/build-switch.yml." >&2
exit 2
