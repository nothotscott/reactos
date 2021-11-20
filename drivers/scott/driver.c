/*
 * PROJECT:     ReactOS Scott Driver
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Scott's test driver
 * COPYRIGHT:   Copyright 2021 Scott Maday <coldasdryice1@gmail.com>
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

static
VOID
NTAPI
DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING DosDeviceName, GlobalDeviceName;

    DPRINT("Scott's DriverUnload\n");

    RtlInitUnicodeString(&DosDeviceName, L"\\DosDevices\\scott");
    RtlInitUnicodeString(&GlobalDeviceName, L"\\??\\scott");

    IoDeleteSymbolicLink(&DosDeviceName);
    IoDeleteSymbolicLink(&GlobalDeviceName);

    IoDeleteDevice(DriverObject->DeviceObject);
}

static
NTSTATUS
NTAPI
DispatchCreate(IN PDEVICE_OBJECT DeviceObject,
               IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_IMPLEMENTED;
    DPRINT("Scott's DispatchCreate\n");
    return NtStatus;
}

static
NTSTATUS
NTAPI
DispatchClose(IN PDEVICE_OBJECT DeviceObject,
              IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_IMPLEMENTED;
    DPRINT("Scott's DispatchClose\n");
    return NtStatus;
}

static
NTSTATUS
NTAPI
DispatchCleanup(IN PDEVICE_OBJECT DeviceObject,
                IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_IMPLEMENTED;
    DPRINT("Scott's DispatchCleanup\n");
    return NtStatus;
}

static
NTSTATUS
NTAPI
DispatchRead(IN PDEVICE_OBJECT DeviceObject, 
             IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION IoStack = NULL;
    PUCHAR Buffer;

    DPRINT("Scott's DispatchRead\n");

    IoStack = IoGetCurrentIrpStackLocation(Irp);
    if (!IoStack)
    {
        return STATUS_BAD_STACK;
    }

    __try
    {
        ProbeForRead(Irp->UserBuffer, IoStack->Parameters.Read.Length, TYPE_ALIGNMENT(char));
        Buffer = Irp->UserBuffer;
        DPRINT("Read buffer (%p): %s\n", Buffer, Buffer);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        NtStatus = GetExceptionCode();
    }
    if (!Buffer)
    {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    return NtStatus;
}

static
NTSTATUS
NTAPI
DispatchWrite(IN PDEVICE_OBJECT DeviceObject,
              IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION IoStack = NULL;
    PUCHAR Buffer;

    DPRINT("Scott's DispatchWrite\n");

    IoStack = IoGetCurrentIrpStackLocation(Irp);
    if (!IoStack)
    {
        return STATUS_BAD_STACK;
    }
    
    __try {
        ProbeForRead(Irp->UserBuffer, IoStack->Parameters.Write.Length, TYPE_ALIGNMENT(char));
        Buffer = Irp->UserBuffer;
        DPRINT("Write buffer (%p): %s\n", Buffer, Buffer);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        NtStatus = GetExceptionCode();
    }
    if (!Buffer)
    {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    return NtStatus;
}

static
NTSTATUS
NTAPI
DispatchPnp(IN PDEVICE_OBJECT DeviceObject,
            IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_IMPLEMENTED;
    DPRINT("Scott's DispatchPnp\n");
    return NtStatus;
}

static
NTSTATUS
NTAPI
DispatchPower(IN PDEVICE_OBJECT DeviceObject,
              IN PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_IMPLEMENTED;
    DPRINT("Scott's DispatchPower\n");
    return NtStatus;
}

NTSTATUS
NTAPI
DriverEntry(IN PDRIVER_OBJECT DriverObject,
            IN PUNICODE_STRING RegPath)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    UNICODE_STRING DeviceName, DosDeviceName, GlobalDeviceName;
    PDEVICE_OBJECT DeviceObject;

    DPRINT("Scott's driver entry\n");

    RtlInitUnicodeString(&DeviceName, L"\\Device\\scott");
    RtlInitUnicodeString(&DosDeviceName, L"\\DosDevice\\scott");
    RtlInitUnicodeString(&GlobalDeviceName, L"\\??\\scott");

    NtStatus = IoCreateDevice(DriverObject,
                              0,
                              &DeviceName,
                              FILE_DEVICE_UNKNOWN,
                              FILE_DEVICE_SECURE_OPEN,
                              FALSE,
                              &DeviceObject);

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispatchCleanup;
    DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
    DriverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER] = DispatchPower;
    DriverObject->DriverUnload = DriverUnload;

    DeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);

    IoCreateSymbolicLink(&DosDeviceName, &DeviceName);
    IoCreateSymbolicLink(&GlobalDeviceName, &DeviceName);

    return NtStatus;
}
