#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
NRO="$ROOT/build_switch/XuitchTV.nro"
IP="${1:-${SWITCH_IP:-}}"

if [[ -z "$IP" ]]; then
  echo "Uso: $0 <IP_DE_LA_SWITCH>  (o define SWITCH_IP)" >&2
  exit 2
fi
if [[ ! -f "$NRO" ]]; then
  echo "No existe $NRO. Compila primero XuitchTV.nro." >&2
  exit 2
fi
if ! command -v nxlink >/dev/null 2>&1; then
  echo "nxlink no esta disponible. Instala las herramientas devkitPro." >&2
  exit 2
fi

echo "Enviando XuitchTV a $IP ..."
nxlink -a "$IP" -p XuitchTV/XuitchTV.nro -s "$NRO" --args -d -v
