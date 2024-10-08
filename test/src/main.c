#include <ntddk.h>
#include "logging.h"
#include "vmxon.h"


NTSTATUS entry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    vmm_init();

    return STATUS_SUCCESS;
}
