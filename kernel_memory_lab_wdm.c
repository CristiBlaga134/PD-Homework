#include <ntddk.h>

/*
Q1: User vs Kernel memory
- User: izolat pe proces.
- Kernel: acces global in nucleu; eroare poate da BSOD.

Q2: Driver + adresa invalida
- Poate produce bugcheck (ex: IRQL_NOT_LESS_OR_EQUAL).

Q3: Rol Memory Manager
- Gestioneaza maparea memoriei si pool-urile paged/non-paged.
*/

#define TAG1 '1GAT'
#define TAG2 '2GAT'
#define UNSAFE_TEST 0

static PVOID gBuf = NULL;
static KDPC gDpc;

VOID DpcRoutine(_In_ struct _KDPC* Dpc, _In_opt_ PVOID Ctx, _In_opt_ PVOID A1, _In_opt_ PVOID A2) {
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(Ctx);
    UNREFERENCED_PARAMETER(A1);
    UNREFERENCED_PARAMETER(A2);

#if UNSAFE_TEST
    
    PVOID p = ExAllocatePoolWithTag(PagedPool, 64, TAG2);
    if (p) { RtlFillMemory(p, 64, 0xBB); ExFreePoolWithTag(p, TAG2); }
#else
    DbgPrint("[Lab] Unsafe test disabled.\n");
#endif
}

VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    if (gBuf) ExFreePoolWithTag(gBuf, TAG1);
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    DriverObject->DriverUnload = DriverUnload;

   
    gBuf = ExAllocatePoolWithTag(NonPagedPoolNx, 256, TAG1);
    if (!gBuf) return STATUS_INSUFFICIENT_RESOURCES;

   
    RtlFillMemory(gBuf, 256, 0xAA);

    
    KeInitializeDpc(&gDpc, DpcRoutine, NULL);
    KeInsertQueueDpc(&gDpc, NULL, NULL);

   
    return STATUS_SUCCESS;
}
