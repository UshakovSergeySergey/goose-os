#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

#######################
# Configure variables #
#######################

THIRD_PARTY_DIR="${ROOT_DIR}/third-party"
LIMINE_DIR="${THIRD_PARTY_DIR}/limine"

LIMINE_VERSION="12.11.1"
LIMINE_BINARIES_ARCHIVE_URL="https://github.com/limine-bootloader/limine/releases/download/v${LIMINE_VERSION}/limine-binary.tar.gz"
LIMINE_SOURCES_ARCHIVE_URL="https://github.com/limine-bootloader/limine/releases/download/v${LIMINE_VERSION}/limine-${LIMINE_VERSION}.tar.gz"
LIMINE_BIN_ARCHIVE="${ROOT_DIR}/third-party/limine-bin.tar.gz"
LIMINE_SRC_ARCHIVE="${ROOT_DIR}/third-party/limine-src.tar.gz"

###########################################
# Download Limine binary and source files #
###########################################

echo "Build Limine bootloader..."

mkdir -p "${THIRD_PARTY_DIR}"
if [ ! -d "${LIMINE_DIR}" ]; then
    mkdir -p "${LIMINE_DIR}"

    curl -fsSL "${LIMINE_BINARIES_ARCHIVE_URL}" -o "${LIMINE_BIN_ARCHIVE}"
    tar -xzf "${LIMINE_BIN_ARCHIVE}" --strip-components=1 -C "${LIMINE_DIR}"
    make -C "${LIMINE_DIR}"
    rm -f "${LIMINE_BIN_ARCHIVE}"

    curl -fsSL "${LIMINE_SOURCES_ARCHIVE_URL}" -o "${LIMINE_SRC_ARCHIVE}"
    tar -xzf "${LIMINE_SRC_ARCHIVE}" --strip-components=1 -C "${LIMINE_DIR}"
    rm -f "${LIMINE_SRC_ARCHIVE}"

    echo "Limine bootloader built successfully."
else
    echo "'${LIMINE_DIR}' directory exists, skip."
fi
