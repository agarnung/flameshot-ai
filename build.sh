#!/usr/bin/env bash
# Compila Flameshot IA usando Docker.
# Requiere: docker
# El binario resultante queda en build_user/src/flameshot

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="flameshot-builder"

if ! docker images -q "${IMAGE_NAME}" >/dev/null 2>&1; then
    echo "Creando imagen de build ${IMAGE_NAME}..."
    docker build -t "${IMAGE_NAME}" -f - "${SCRIPT_DIR}" <<'DOCKERFILE'
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qq && apt-get install -y -qq \
    cmake g++ qt6-base-dev qt6-tools-dev qt6-svg-dev qt6-base-private-dev \
    git libgl1-mesa-dev libxkbcommon-x11-dev libxcb-util-dev libxcb-cursor-dev \
    libxcb-keysyms1-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev \
    libxcb-image0-dev libxcb-xinerama0-dev libxcb-icccm4-dev libxcb-sync-dev \
    libxcb-xkb-dev libxcb-render-util0-dev libxcb-util0-dev libdbus-1-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
DOCKERFILE
fi

echo "Compilando Flameshot IA..."
docker run --rm \
    -v "${SCRIPT_DIR}:/src" \
    -w /src \
    -u "$(id -u):$(id -g)" \
    "${IMAGE_NAME}" \
    bash -c "mkdir -p build_user && cd build_user && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j\$(nproc)"

echo ""
echo "✅ Binario listo: ${SCRIPT_DIR}/build_user/src/flameshot"
