#!/usr/bin/env bash
set -euo pipefail

fail=0
check_cmd() {
  if command -v "$1" >/dev/null 2>&1; then
    printf '[OK] %-24s %s\n' "$1" "$(command -v "$1")"
  else
    printf '[--] %-24s no encontrado\n' "$1"
    fail=1
  fi
}

echo 'XuitchTV Nintendo Switch build preflight'
echo '----------------------------------------'
printf 'DEVKITPRO: %s\n' "${DEVKITPRO:-<no definido>}"
check_cmd cmake
check_cmd git

if [[ -n "${DEVKITPRO:-}" ]]; then
  check_cmd aarch64-none-elf-g++
  check_cmd nacptool
  check_cmd elf2nro
  if command -v pkg-config >/dev/null 2>&1; then
    export PKG_CONFIG_PATH="${DEVKITPRO}/portlibs/switch/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
    pkg-config --exists libcurl && echo '[OK] switch libcurl pkg-config' || { echo '[--] switch libcurl faltante'; fail=1; }
    if pkg-config --exists mpv; then
      echo '[OK] switch libmpv pkg-config'
      mpv_inc="$(pkg-config --variable=includedir mpv 2>/dev/null || true)"
      if [[ -n "$mpv_inc" && -f "$mpv_inc/mpv/render_dk3d.h" ]]; then
        echo '[OK] mpv deko3d render header'
      else
        echo '[--] mpv/render_dk3d.h faltante (se necesita backend deko3d)'
        fail=1
      fi
    else
      echo '[--] switch libmpv faltante'
      fail=1
    fi
  fi
elif command -v docker >/dev/null 2>&1; then
  echo '[OK] Docker disponible; puedes usar scripts/build-switch-docker.sh'
  fail=0
else
  echo '[--] No hay devkitPro ni Docker en este equipo.'
fi

if [[ $fail -ne 0 ]]; then
  echo
  echo 'Instala devkitPro + switch-dev/switch-curl/switch-libmpv/switch-glfw/switch-libwebp,'
  echo 'o usa Docker/GitHub Actions.'
fi
exit "$fail"
