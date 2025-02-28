#ifndef EFILIB_H
#define EFILIB_H

#include "efi.h"
#include <stddef.h>  // ✅ Fix NULL issue

// ✅ EFI Reset Types
typedef enum {
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown
} EFI_RESET_TYPE;

// ✅ Print a Unicode String
EFI_STATUS Print(CHAR16 *String, ...) {
    return ST->ConOut->OutputString(ST->ConOut, String);
}

// ✅ Allocate memory pool
EFI_STATUS AllocatePool(EFI_MEMORY_TYPE PoolType, UINTN Size, void **Buffer) {
    return ST->BootServices->AllocatePool(PoolType, Size, Buffer);
}

// ✅ Free allocated memory pool
EFI_STATUS FreePool(void *Buffer) {
    return ST->BootServices->FreePool(Buffer);
}

// ✅ Retrieve UEFI Memory Map (wrapper to reduce code duplication)
EFI_STATUS GetMemoryMap(UINTN *MemoryMapSize, EFI_MEMORY_DESCRIPTOR **MemoryMap, UINTN *MapKey, UINTN *DescriptorSize, UINT32 *DescriptorVersion) {
    return ST->BootServices->GetMemoryMap(MemoryMapSize, *MemoryMap, MapKey, DescriptorSize, DescriptorVersion);
}

// ✅ Exit Boot Services
EFI_STATUS ExitBootServices(EFI_HANDLE ImageHandle, UINTN MapKey) {
    return ST->BootServices->ExitBootServices(ImageHandle, MapKey);
}

// ✅ System Reboot (in case of failure)
void RebootSystem() {
    ST->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
}

#endif // EFILIB_H
