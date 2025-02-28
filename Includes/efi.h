#ifndef EFI_H
#define EFI_H

#include <stdint.h>

#define EFIAPI __attribute__((ms_abi)) // Required for calling conventions

// ✅ Standard EFI Types
typedef uint16_t CHAR16;
typedef uint64_t EFI_STATUS;
typedef void* EFI_HANDLE;
typedef void* EFI_EVENT;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint64_t EFI_VIRTUAL_ADDRESS;
typedef uint64_t UINTN;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;

// ✅ EFI Error Codes (Only essential ones)
#define EFI_SUCCESS 0
#define EFI_LOAD_ERROR 0x8000000000000001
#define EFI_INVALID_PARAMETER 0x8000000000000002
#define EFI_UNSUPPORTED 0x8000000000000003
#define EFI_BUFFER_TOO_SMALL 0x8000000000000005
#define EFI_ABORTED 0x8000000000000010
#define EFI_ERROR(Status) (((EFI_STATUS)(Status)) & 0x8000000000000000)

// ✅ EFI Table Header
typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

// ✅ UEFI Allocate Type
typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress
} EFI_ALLOCATE_TYPE;

// ✅ EFI Memory Types
typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory
} EFI_MEMORY_TYPE;

// ✅ Forward Declare UEFI Tables
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct _EFI_BOOT_SERVICES;
struct _EFI_RUNTIME_SERVICES;

// ✅ EFI System Table
typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    uint32_t FirmwareRevision;
    void *ConsoleInHandle;
    void *ConIn;
    void *ConsoleOutHandle;
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    void *StandardErrorHandle;
    void *StdErr;
    struct _EFI_RUNTIME_SERVICES *RuntimeServices;
    struct _EFI_BOOT_SERVICES *BootServices;
} EFI_SYSTEM_TABLE;

// ✅ EFI Simple Text Output Protocol
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_STATUS (*Reset)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, uint8_t);
    EFI_STATUS (*OutputString)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, CHAR16*);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// ✅ EFI Boot Services
typedef struct _EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *RaiseTPL;
    void *RestoreTPL;
    EFI_STATUS (*AllocatePages)(uint64_t, uint64_t, uint64_t, EFI_PHYSICAL_ADDRESS*);
    EFI_STATUS (*FreePages)(EFI_PHYSICAL_ADDRESS, uint64_t);
    EFI_STATUS (*GetMemoryMap)(UINTN*, void*, UINTN*, UINTN*, UINT32*);
    EFI_STATUS (*AllocatePool)(EFI_MEMORY_TYPE, UINTN, void**);
    EFI_STATUS (*FreePool)(void*);
    EFI_STATUS (*ExitBootServices)(EFI_HANDLE, UINTN);
} EFI_BOOT_SERVICES;

// ✅ EFI Runtime Services
typedef struct _EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *GetTime;
    void *SetTime;
    void *GetWakeupTime;
    void *SetWakeupTime;
    void *SetVirtualAddressMap;
    void *ConvertPointer;
    void *GetVariable;
    void *SetVariable;
    void *GetNextVariableName;
    (ResetSystem);
} EFI_RUNTIME_SERVICES;

// ✅ EFI Memory Descriptor
typedef struct {
    uint32_t Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    uint64_t NumberOfPages;
    uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

// ✅ Declare `ST` at the end
extern EFI_SYSTEM_TABLE *ST;

#endif // EFI_H


