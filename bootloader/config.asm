; NASM build configuration for 64-bit UEFI
DEFAULT REL
CPU X64
BITS 64

; PE32+ header definitions for UEFI
%define IMAGE_FILE_MACHINE_X64 0x8664
%define IMAGE_SUBSYSTEM_EFI_APPLICATION 10