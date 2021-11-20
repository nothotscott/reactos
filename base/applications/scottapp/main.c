/*
 * PROJECT:     ReactOS Scott Driver Application
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Interfaces with the scott driver
 * COPYRIGHT:   Copyright 2021 Scott Maday <coldasdryice1@gmail.com>
 */

#include <stdio.h>
#include <windows.h>
#include <debug.h>

#define CRITICAL_ERROR (-1)

static
BOOL
SendDeviceMessage(PCHAR Msg)
{
    HANDLE hFile;
    DWORD dwReturn;
    BOOL Status = TRUE;

    OutputDebugString("Creating scott device file\n");

    hFile = CreateFile("\\\\.\\GlobalRoot\\Device\\scott",
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);
    if (!hFile) 
    {
        OutputDebugString("Could not create file to scott device\n");
        return FALSE;
    }

    Status &= WriteFile(hFile, Msg, sizeof(Msg), &dwReturn, NULL);
    if (!Status)
    {
        OutputDebugString("Failed to write to device file\n");
    }
    Status &= CloseHandle(hFile);

    return Status;
}

INT
CDECL
main(VOID)
{
    HANDLE hSCManager;
    HANDLE hService;
    SERVICE_STATUS ServiceStatus;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

    printf("Loading the Scott driver\n");

    if(!hSCManager)
    {
        printf("Failed to open service control manager\n");
        return CRITICAL_ERROR;
    }

    OutputDebugString("Opening scott service\n");

    hService = OpenService(hSCManager, 
                           "scott", 
                           SERVICE_START | SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!hService)
    {
        printf("Failed to open scott service\n");
        return CRITICAL_ERROR;
    }

    if (!QueryServiceStatus(hService, &ServiceStatus)) {
        printf("Failed to query scott service\n");
        return CRITICAL_ERROR;
    }

    if(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        printf("Scott service is currently running\n");
    }
    else
    {
        if(StartService(hService, 0, NULL))
        {
            printf("Successfully started scott service\n");
        }
        else
        {
            printf("Could not start scott service\n");
            return CRITICAL_ERROR;
        }
    }

    while(TRUE)
    {
        printf("q=Quit, s=Stop, m=Message\n");
        switch (getchar())
        {
            case 'q':
                printf("Quitting\n");
                goto Cleanup;
            case 's':
            {
                if (ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus))
                {
                    printf("Successfully stopped scott service\n");
                    goto Cleanup;
                }
                else
                {
                    printf("Failed to stop scott service\n");
                }
                break;
            }
            case 'm':
            {
                CHAR Msg[128];

                fflush(stdin);
                printf("Message: ");
                fgets(Msg, sizeof(Msg) - 1, stdin);

                if (SendDeviceMessage(Msg))
                {
                    printf("Message sent\n");
                }
                else
                {
                    printf("Message failed to send\n");
                }
                break;
            }
        }
    }

Cleanup:
    CloseServiceHandle(hService);

    CloseServiceHandle(hSCManager);

    return 0;
}
