#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="${DEVKITPRO_IMAGE:-devkitpro/devkita64:20251117}"

if ! command -v docker >/dev/null 2>&1; then
  echo 'Docker no esta instalado/disponible.' >&2
  exit 2
fi

if [[ ! -d "$ROOT/library/borealis" ]]; then
  "$ROOT/scripts/bootstrap-deps.sh"
fi

docker run --rm \
  -v "$ROOT:/work" \
  -w /work \
  "$IMAGE" bash -lc '
    set -euo pipefail
    dkp-pacman -S --needed --noconfirm switch-dev switch-glfw switch-libwebp switch-curl switch-libmpv
    source /opt/devkitpro/switchvars.sh
    export PATH=/opt/devkitpro/devkitA64/bin:$PATH
    export PKG_CONFIG_PATH=/opt/devkitpro/portlibs/switch/lib/pkgconfig:${PKG_CONFIG_PATH:-}
    rm -rf build_switch
    cmake -S . -B build_switch -DPLATFORM_SWITCH=ON -DCMAKE_BUILD_TYPE=Release
    cmake --build build_switch --target XuitchTV.nro -j$(nproc)
  '

echo "NRO generado: $ROOT/build_switch/XuitchTV.nro"
