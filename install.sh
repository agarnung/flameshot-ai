#!/usr/bin/env bash
# Instalación rápida de Flameshot IA.
# 1. Compila Flameshot (con Docker si está disponible).
# 2. Crea ~/.local/bin/flameshot-ia apuntando al binario.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_DIR="${HOME}/.local/bin"
BIN_NAME="flameshot-ia"

echo "=== Flameshot IA - Instalador ==="

# Compilar si no existe binario
if [[ ! -x "${SCRIPT_DIR}/build_user/src/flameshot" ]]; then
    if command -v docker >/dev/null 2>&1; then
        echo "Compilando con Docker..."
        "${SCRIPT_DIR}/build.sh"
    else
        echo "No se encontró Docker ni un binario compilado."
        echo "Instala Docker y ejecuta: ./build.sh"
        exit 1
    fi
else
    echo "Usando binario existente: ${SCRIPT_DIR}/build_user/src/flameshot"
fi

# Instalar binario en ~/.local/bin
# Se crean dos nombres: "flameshot-ia" y "flameshot" (este último tiene
# prioridad sobre /usr/bin/flameshot si ~/.local/bin está antes en el PATH).
mkdir -p "${BIN_DIR}"
ln -sf "${SCRIPT_DIR}/build_user/src/flameshot" "${BIN_DIR}/${BIN_NAME}"
ln -sf "${SCRIPT_DIR}/build_user/src/flameshot" "${BIN_DIR}/flameshot"

# Añadir ~/.local/bin a PATH si no está
if [[ ":${PATH}:" != *":${BIN_DIR}:"* ]]; then
    echo "Añade ${BIN_DIR} a tu PATH. Ejemplo:"
    echo "  export PATH=\"${BIN_DIR}:\$PATH\""
fi

echo ""
echo "✅ Instalación completa."
echo "   Binario: ${BIN_DIR}/${BIN_NAME}"
echo ""
echo "Configura la API en flameshot.ini:"
echo "  iaApiUrl=http://localhost:8000/v1/chat/completions"
echo "  iaApiToken="
echo "  iaModel=cpatonn/Qwen3-VL-32B-Instruct-AWQ-4bit"
echo ""
echo "Prueba con:"
echo "  export PATH=\"${BIN_DIR}:\$PATH\""
echo "  flameshot-ia --version"
echo "  flameshot-ia gui"
