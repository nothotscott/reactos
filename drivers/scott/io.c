/*
 * PROJECT:     ReactOS Scott Driver
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Scott's test driver
 * COPYRIGHT:   Copyright 2021 Scott Maday <coldasdryice1@gmail.com>
 */

#include "driver.h"

#include <debug.h>

/* FUNCTIONS ****************************************************************/

static
NTSTATUS
GetIoReadBuffer(IN PIRP Irp,
                IN PIO_STACK_LOCATION IoStack,
                OUT PULONG_PTR pInformation,
                OUT PUCHAR* pInputBuffer,
                OUT PULONG lpInputBufferLength)
{
    *lpInputBufferLength = IoStack->Parameters.DeviceIoControl.InputBufferLength;

    switch (IO_METHOD_FROM_CTL_CODE(IoStack->Parameters.DeviceIoControl.IoControlCode))
    {
        case METHOD_IN_DIRECT:
        case METHOD_OUT_DIRECT:
            INFO_(SCOTT, "Using Direct IO\n");
            *pInputBuffer = Irp->AssociatedIrp.SystemBuffer;
            break;
        case METHOD_BUFFERED:
            INFO_(SCOTT, "Using Buffered IO\n");
            *pInputBuffer = Irp->AssociatedIrp.SystemBuffer;
            break;
        case METHOD_NEITHER:
            INFO_(SCOTT, "Using Neither IO method\n");
            *pInputBuffer = IoStack->Parameters.DeviceIoControl.Type3InputBuffer;
            if (!*pInputBuffer)
            {
                break;
            }
            __try
            {
                ProbeForRead(*pInputBuffer, *lpInputBufferLength, TYPE_ALIGNMENT(char));
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return GetExceptionCode();
            }
            break;
        default:
            return STATUS_UNSUCCESSFUL;
    }
    if (!*pInputBuffer)
    {
        return STATUS_INVALID_USER_BUFFER;
    }
    
    return STATUS_SUCCESS;
}

static
NTSTATUS
GetIoWriteBuffer(IN PIRP Irp,
                 IN PIO_STACK_LOCATION IoStack,
                 OUT PULONG_PTR pInformation,
                 OUT PUCHAR* pOutputBuffer,
                 OUT PULONG lpOutputBufferLength)
{
    *lpOutputBufferLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (IO_METHOD_FROM_CTL_CODE(IoStack->Parameters.DeviceIoControl.IoControlCode))
    {
        case METHOD_IN_DIRECT:
        case METHOD_OUT_DIRECT:
            INFO_(SCOTT, "Using Direct IO\n");
            *pOutputBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
            break;
        case METHOD_BUFFERED:
            INFO_(SCOTT, "Using Buffered IO\n");
            *pOutputBuffer = Irp->AssociatedIrp.SystemBuffer;
            break;
        case METHOD_NEITHER:
            INFO_(SCOTT, "Using Neither IO method\n");
            *pOutputBuffer = Irp->UserBuffer;
            if (!*pOutputBuffer)
            {
                break;
            }
            __try
            {
                ProbeForWrite(*pOutputBuffer, *lpOutputBufferLength, TYPE_ALIGNMENT(char));
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return GetExceptionCode();
            }
            break;
        default:
            return STATUS_UNSUCCESSFUL;
    }
    if (!*pOutputBuffer)
    {
        return STATUS_INVALID_USER_BUFFER;
    }
    
    return STATUS_SUCCESS;
}

NTSTATUS
HandleIoRead(IN PIRP Irp,
             IN PIO_STACK_LOCATION IoStack,
             OUT PULONG_PTR pInformation)
{
    PUCHAR InputBuffer;
    ULONG InputBufferLength;
    NTSTATUS Status = STATUS_SUCCESS;

    Status = GetIoReadBuffer(Irp, IoStack, pInformation, &InputBuffer, &InputBufferLength);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    DPRINT("Read buffer (%p): %s\n", InputBuffer, InputBuffer);

    return Status;
}

NTSTATUS
HandleIoWrite(IN PIRP Irp,
              IN PIO_STACK_LOCATION IoStack,
              OUT PULONG_PTR pInformation)
{
    PUCHAR InputBuffer;
    ULONG InputBufferLength;
    PUCHAR OutputBuffer;
    ULONG OutputBufferLength;
    STRING OutputString;
    NTSTATUS Status = STATUS_SUCCESS;

    Status = GetIoReadBuffer(Irp, IoStack, pInformation, &InputBuffer, &InputBufferLength);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    Status = GetIoWriteBuffer(Irp, IoStack, pInformation, &OutputBuffer, &OutputBufferLength);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    RtlCopyMemory(OutputBuffer, InputBuffer, InputBufferLength);
    RtlInitString(&OutputString, OutputBuffer);
    RtlUpperString(&OutputString, &OutputString);
    
    return Status;
}