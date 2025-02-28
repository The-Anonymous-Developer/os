#include "Includes/efi.h"
#include "Includes/efilib.h"
#include <stddef.h>

// ✅ Global UEFI System Table
EFI_SYSTEM_TABLE *ST;

// ✅ UEFI Entry Point
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;  // Store the UEFI System Table globally
    Print(L"[Bootloader] UEFI Bootloader Loaded!\r\n");

    // ✅ Memory Map Variables
    UINTN MemoryMapSize = 0, MapKey, DescriptorSize;
    UINT32 DescriptorVersion;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    EFI_STATUS Status;

    // ✅ Get initial Memory Map size WITHOUT allocating memory yet
    Status = ST->BootServices->GetMemoryMap(&MemoryMapSize, NULL, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"[Bootloader] Failed to get initial MemoryMap size\r\n");
        return Status;
    }

    // ✅ Allocate buffer with extra space BEFORE calling GetMemoryMap()
    UINTN AllocSize = MemoryMapSize + (DescriptorSize * 16); // Extra space to handle changes
    Status = ST->BootServices->AllocatePool(EfiBootServicesData, AllocSize, (void**)&MemoryMap);
    if (EFI_ERROR(Status)) {
        Print(L"[Bootloader] Failed to allocate MemoryMap buffer\r\n");
        return Status;
    }

    // ✅ Ensure stable MemoryMap retrieval loop
    UINTN Attempts = 0;
    while (Attempts < 10) { // Prevent infinite looping
        UINTN TempSize = MemoryMapSize;
        Status = ST->BootServices->GetMemoryMap(&TempSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

        if (EFI_ERROR(Status)) {
            Print(L"[Bootloader] Failed to retrieve MemoryMap\r\n");
            ST->BootServices->FreePool(MemoryMap);
            return Status;
        }

        // ✅ Immediately attempt ExitBootServices
        Status = ST->BootServices->ExitBootServices(ImageHandle, MapKey);
        if (Status == EFI_SUCCESS) {
            Print(L"[Bootloader] Successfully exited boot services\r\n");
            break;
        }

        if (Status == EFI_INVALID_PARAMETER) {
            Print(L"[Bootloader] MemoryMap changed, retrying ExitBootServices...\r\n");
            Attempts++;

            // 🔥 FIX: Get MemoryMap AGAIN before retrying ExitBootServices()
            TempSize = MemoryMapSize;
            Status = ST->BootServices->GetMemoryMap(&TempSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
            if (EFI_ERROR(Status)) {
                Print(L"[Bootloader] Final retry MemoryMap retrieval failed!\r\n");
                ST->BootServices->FreePool(MemoryMap);
                return Status;
            }

            continue; // Retry `ExitBootServices()`
        }

        Print(L"[Bootloader] ExitBootServices() failed: %r\r\n", Status);
        ST->BootServices->FreePool(MemoryMap);
        return Status;
    }

    if (Attempts == 10) {
        Print(L"[Bootloader] MemoryMap kept changing, aborting exit.\r\n");
        ST->BootServices->FreePool(MemoryMap);
        return EFI_LOAD_ERROR;
    }

    // ✅ Now boot into the kernel (Placeholder for kernel jump)
    Print(L"[Bootloader] Jumping to kernel...\r\n");

    // 🎯 TODO: Kernel loading and jumping to kernel entry point goes here

    return EFI_SUCCESS;
}
