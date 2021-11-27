/*
 * PROJECT:     ReactOS Scott Driver
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Scott's test driver
 * COPYRIGHT:   Copyright 2021 Scott Maday <coldasdryice1@gmail.com>
 */

#pragma once

#include <ntddk.h>
#include <stdio.h>

#include <debug.h>

#define IO_METHOD_FROM_CTL_CODE(CtlCode) (CtlCode & 0x00000003)

/************************************ io.c */

NTSTATUS
HandleIoRead(IN PIRP Irp,
             IN PIO_STACK_LOCATION IoStack,
             OUT PULONG_PTR pInformation);

NTSTATUS
HandleIoWrite(IN PIRP Irp,
              IN PIO_STACK_LOCATION IoStack,
              OUT PULONG_PTR pInformation);
