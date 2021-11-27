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

#define IOCTL_MODE_DIRECT_IN  CTL_CODE(FILE_DEVICE_UNKNOWN,             \
                                       0x800,                           \
                                       METHOD_IN_DIRECT,                \
                                       FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MODE_DIRECT_OUT CTL_CODE(FILE_DEVICE_UNKNOWN,             \
                                       0x801,                           \
                                       METHOD_OUT_DIRECT,               \
                                       FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MODE_BUFFERED   CTL_CODE(FILE_DEVICE_UNKNOWN,             \
                                       0x802,                           \
                                       METHOD_BUFFERED,                 \
                                       FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MODE_NEITHER    CTL_CODE(FILE_DEVICE_UNKNOWN,             \
                                       0x803,                           \
                                       METHOD_NEITHER,                  \
                                       FILE_READ_DATA | FILE_WRITE_DATA)

static
BOOL
SendDeviceMessage(IN PCHAR InputMsg,
                  IN DWORD nInputMsgSize,
                  OUT PCHAR OutputMsg,
                  IN DWORD nOutputMsgSize)
{
    HANDLE hFile;
    DWORD dwReturn;
    BOOL Status;
    

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

    RtlZeroMemory(OutputMsg, nOutputMsgSize);

    Status = DeviceIoControl(hFile,
                             IOCTL_MODE_NEITHER,
                             InputMsg,
                             nInputMsgSize,
                             OutputMsg,
                             nOutputMsgSize,
                             &dwReturn,
                             NULL);
    if(Status != TRUE)
    {
        OutputDebugString("Could not send & receive device IO\n");
        return Status;
    }

    return CloseHandle(hFile);
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
        printf("q=Quit, s=Start,x=Stop, m=Message\n");
        switch (getchar())
        {
            case 'q':
                printf("Quitting\n");
                goto Cleanup;
            case 's':
            {
                if (StartService(hService, 0, NULL))
                {
                    printf("Successfully started scott service\n");
                }
                else
                {
                    printf("Failed to start scott service\n");
                }
                break;
            }
            case 'x':
            {
                if (ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus))
                {
                    printf("Successfully stopped scott service\n");
                }
                else
                {
                    printf("Failed to stop scott service\n");
                }
                break;
            }
            case 'm':
            {
                CHAR InMsg[128];
                CHAR OutMsg[128];

                fflush(stdin);
                printf("Message: ");
                fgets(InMsg, sizeof(InMsg) - 1, stdin);

                if (SendDeviceMessage(InMsg, strlen(InMsg), OutMsg, sizeof(OutMsg)))
                {
                    printf("Return: %s\n", OutMsg);
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
