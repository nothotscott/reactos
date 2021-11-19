/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         Scott's Test Driver
 * PURPOSE:         Scott's driver loading/unloading
 */

#include "driver.h"

static DRIVER_UNLOAD DriverUnload;
static DRIVER_DISPATCH DispatchCreate;
static DRIVER_DISPATCH DispatchClose;
static DRIVER_DISPATCH DispatchCleanup;
static DRIVER_DISPATCH DispatchPnp;
static DRIVER_DISPATCH DispatchPower;
DRIVER_INITIALIZE DriverEntry;

/* FUNCTIONS ****************************************************************/

static VOID NTAPI
DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
    DPRINT("Scott's DriverUnload\n");
    return STATUS_SUCCESS;
}

static NTSTATUS NTAPI
DispatchCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchCreate\n");
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS NTAPI
DispatchClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchClose\n");
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS NTAPI
DispatchCleanup(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchCleanup\n");
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS NTAPI
DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchRead\n");
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS NTAPI
DispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchWrite\n");
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS NTAPI
DispatchPnp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchPnp\n");
    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS NTAPI
DispatchPower(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    DPRINT("Scott's DispatchPower\n");
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS NTAPI
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegPath)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING usDriverName, usDosDeviceName;
    PDEVICE_OBJECT DeviceObject;

    DPRINT("Scott's driver entry\n");

    RtlInitUnicodeString(&usDriverName, L"\\Device\\Scott");
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\Scott");
    status = IoCreateDevice(
        DriverObject, 0, &usDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispatchCleanup;
    DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
    DriverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER] = DispatchPower;

    return status;
}