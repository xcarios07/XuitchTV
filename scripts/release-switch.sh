#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

"$ROOT/scripts/build-switch.sh"
"$ROOT/scripts/package-sd.sh"

echo
echo "Release preparada:" 
echo "  NRO: $ROOT/build_switch/XuitchTV.nro"
echo "  SD : $ROOT/dist/XuitchTV-SD.zip"
