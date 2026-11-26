#!/usr/bin/env bash
set -euo pipefail

# Make a USB drive bootable by raw-writing a given ISO image
# Usage: make-bootable-usb.sh /path/to/image.iso /dev/sdX

usage() {
  echo "Usage: $0 <path-to.iso> <target-device>" >&2
  echo "Example: sudo $0 goose-os.iso /dev/sdX" >&2
}

if [[ ${EUID:-$(id -u)} -ne 0 ]]; then
  echo "This script must be run as root (use sudo)." >&2
  exit 1
fi

if [[ $# -ne 2 ]]; then
  usage
  exit 1
fi

ISO_PATH="$1"
TARGET_DEV="$2"

if [[ ! -f "$ISO_PATH" ]]; then
  echo "ISO not found: $ISO_PATH" >&2
  exit 1
fi

if [[ ! -b "$TARGET_DEV" ]]; then
  echo "Block device not found: $TARGET_DEV" >&2
  exit 1
fi

# Ensure target is a whole-disk device (TYPE=disk), not a partition
TYPE_OUT=$(lsblk -dn -o TYPE "$TARGET_DEV" 2>/dev/null || true)
if [[ "$TYPE_OUT" != "disk" ]]; then
  echo "Target must be a whole disk (TYPE=disk), got TYPE='$TYPE_OUT' for $TARGET_DEV" >&2
  exit 1
fi

echo "[*] Preparing to write ISO '$ISO_PATH' to '$TARGET_DEV'"

echo "[*] Unmounting any mounted partitions of $TARGET_DEV"
while read -r part mp; do
  if [[ -n "${mp:-}" ]]; then
    echo "  - umount $mp"
    umount -f "$mp" || true
  fi
done < <(lsblk -ln -o NAME,MOUNTPOINT "$TARGET_DEV" | awk 'NR>1 {print "/dev/"$1, $2}')

echo "[*] Writing image (this may take a while)"
dd if="$ISO_PATH" of="$TARGET_DEV" bs=4M status=progress oflag=sync

echo "[*] Syncing and rereading partition table"
sync || true
blockdev --rereadpt "$TARGET_DEV" 2>/dev/null || true
partprobe "$TARGET_DEV" 2>/dev/null || true

echo "[OK] Done. You can now remove the USB device safely and boot from it."
