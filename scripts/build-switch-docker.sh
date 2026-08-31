#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="${DEVKITPRO_IMAGE:-devkitpro/devkita64:20251117}"

if ! command -v docker >/dev/null 2>&1; then
  echo "Docker no esta instalado/disponible." >&2
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

    echo "=== Updating devkitPro packages ==="
    dkp-pacman -Syu --noconfirm

    echo "=== Installing base Switch dependencies ==="
    dkp-pacman -S --needed --noconfirm \
      switch-dev \
      switch-glfw \
      switch-libwebp \
      switch-curl

    echo "=== Installing deko3d MPV stack ==="
    cd /tmp

    BASE_URL="https://github.com/xfangfang/wiliwili/releases/download/v0.1.0"

    PKGS=(
      "deko3d-8939ff80f94d061dbc7d107e08b8e3be53e2938b-1-any.pkg.tar.zst"
      "libuam-f8c9eef01ffe06334d530393d636d69e2b52744b-1-any.pkg.tar.zst"
      "switch-libass-0.17.1-1-any.pkg.tar.zst"
      "switch-ffmpeg-7.1-1-any.pkg.tar.zst"
      "switch-libmpv_deko3d-0.36.0-2-any.pkg.tar.zst"
    )

    for PKG in "${PKGS[@]}"; do
      echo "Downloading ${PKG}"
      curl -fL "${BASE_URL}/${PKG}" -o "${PKG}"
      dkp-pacman -U --noconfirm "${PKG}"
    done

    cd /work

    source /opt/devkitpro/switchvars.sh

    export PATH=/opt/devkitpro/devkitA64/bin:$PATH
    export PKG_CONFIG_PATH=/opt/devkitpro/portlibs/switch/lib/pkgconfig:${PKG_CONFIG_PATH:-}

    echo "=== Checking deko3d MPV header ==="
    test -f /opt/devkitpro/portlibs/switch/include/mpv/render_dk3d.h

    echo "=== Configuring XuitchTV ==="
    rm -rf build_switch

    cmake -S . -B build_switch \
      -DPLATFORM_SWITCH=ON \
      -DCMAKE_BUILD_TYPE=Release

    echo "=== Building XuitchTV.nro ==="
    cmake --build build_switch \
      --target XuitchTV.nro \
      -j$(nproc)
  '

echo "NRO generado: $ROOT/build_switch/XuitchTV.nro"
