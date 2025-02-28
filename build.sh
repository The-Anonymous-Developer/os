#!/bin/bash



clear
rm -rf disk output
mkdir -p disk output/efi disk/mnt

# ✅ Compile Bootloader (PE/COFF Format)
clang -target x86_64-windows -ffreestanding -fshort-wchar -fno-stack-protector \
      -Wall -Wextra -Werror -nostdlib -c bootloader/bootloader.c -o output/bootloader.o

# ✅ Link Bootloader (UEFI PE Format)
lld-link /entry:efi_main /subsystem:efi_application /machine:x64 /out:output/efi/BOOTX64.EFI output/bootloader.o

# ✅ Create FAT32 Image
qemu-img create -f raw disk/uefi_disk.img 200M
mkfs.vfat disk/uefi_disk.img

# ✅ Mount Image
sudo mount -o loop disk/uefi_disk.img disk/mnt || { echo "Mount failed"; exit 1; }

# ✅ Ensure Directory Structure
sudo mkdir -p disk/mnt/EFI/BOOT

# ✅ Copy Bootloader
sudo cp output/efi/BOOTX64.EFI disk/mnt/EFI/BOOT/BOOTX64.EFI

# ✅ Unmount
sudo umount disk/mnt

# ✅ Run QEMU
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive file=disk/uefi_disk.img,format=raw -m 512 -serial stdio
