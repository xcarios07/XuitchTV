
set -e

BUILD_DIR=cmake-build-switch


cd "$(dirname $0)/.."
git config --global --add safe.directory `pwd`

# Aggiorna i pacchetti nel container DevkitPro prima della build
dkp-pacman -Syu --noconfirm

BASE_URL="https://github.com/xfangfang/wiliwili/releases/download/v0.1.0/"

PKGS=(
    "switch-libass-0.17.1-1-any.pkg.tar.zst"
    "switch-ffmpeg-6.1-5-any.pkg.tar.zst"
    "switch-libmpv-0.36.0-2-any.pkg.tar.zst"
)
for PKG in "${PKGS[@]}"; do
    [ -f "${PKG}" ] || curl -LO ${BASE_URL}${PKG}
    dkp-pacman -U --noconfirm ${PKG}
done


M3U8_URL="${M3U8_URL:-https://raw.githubusercontent.com/iptv-org/iptv/master/streams/py.m3u}"

# GITHUB_TOKEN is optional but pass it if available
GITHUB_TOKEN_FLAG=""
if [ -n "${GITHUB_TOKEN:-}" ]; then
    GITHUB_TOKEN_FLAG="-DGITHUB_TOKEN=\"${GITHUB_TOKEN}\""
fi

# Disable unity build by default for stability on Switch
# Can be re-enabled with ENABLE_UNITY_BUILD=true environment variable
UNITY_BUILD_FLAG="-DBRLS_UNITY_BUILD=OFF"
if [ "${ENABLE_UNITY_BUILD:-false}" = "true" ]; then
    UNITY_BUILD_FLAG="-DBRLS_UNITY_BUILD=ON"
fi

cmake -B ${BUILD_DIR} \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILTIN_NSP=OFF \
  -DPLATFORM_SWITCH=ON \
  ${UNITY_BUILD_FLAG} \
  -DCMAKE_UNITY_BUILD_BATCH_SIZE=16 \
  -DANALYTICS=OFF \
  -DM3U8_URL="${M3U8_URL}" \
  ${GITHUB_TOKEN_FLAG} 

make -C ${BUILD_DIR} XuitchTV.nro -j$(nproc)
