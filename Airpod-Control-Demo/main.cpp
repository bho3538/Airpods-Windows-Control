#include "main.h"

#pragma comment(lib,"Cfgmgr32.lib")

int
__cdecl
wmain()
{
    printf("Airpods Control For Windows DEMO\n");
    printf("Created by byungho. https://github.com/bho3538\n");

    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL echo = TRUE;
    LPWSTR devicePath = NULL;
    DWORD err = GetDevicePath((LPGUID)&BTHECHOSAMPLE_DEVICE_INTERFACE, &devicePath);

    if (ERROR_SUCCESS != err) {
        printf("Failed to find the Airpods AAP Server driver!!\n");
        Sleep(3000);
        exit(1);
    }

    hDevice = CreateFile(devicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to open device. Error %d\n", GetLastError());
        LocalFree(devicePath);
        Sleep(3000);
        exit(1);
    }

    printf("Opened device successfully\n");

    echo = InitConnection(hDevice);

    printf("Enter number\n");
    printf("1 : Get Battery status\n");
    printf("2 : off Active Noise Cancelling\n");
    printf("3 : on Active Noise Cancelling\n");
    printf("4 : set Transparent mode\n");
    printf("0 : connection close\n");

    for (;;) {
        int options = 0;
        scanf_s("%d", &options);

        switch (options) {
        case 1: {
            echo = GetAirpodBatteryStatus(hDevice);
        }; break;
        case 2: {
            echo = SetANCMode(hDevice, 1);
        }; break;
        case 3: {
            echo = SetANCMode(hDevice, 2);
        }; break;
        case 4: {
            echo = SetANCMode(hDevice, 3);
        }; break;
        default: {
            goto closeArea;
        }
        }
        printf("Enter number\n");
    }

closeArea:


    if (INVALID_HANDLE_VALUE != hDevice) {
        CloseHandle(hDevice);
        printf("Closed device\n");
    }

    if (NULL != devicePath) {
        LocalFree(devicePath);
    }
}


BOOL
InitConnection(
    HANDLE hDevice
)
{
    BOOL retval = FALSE;
    DWORD cbWritten = 0;
    DWORD cbRead = 0;
    char recvBuffer[512] = { 0, };

    BOOL bRet = WriteFile(hDevice, request_init_con, sizeof(request_init_con), &cbWritten, NULL);

    if (!bRet) {
        printf("Write failed. Error: %d\n", GetLastError());
        goto exit;
    }

    for (;;) {
        bRet = ReadFile(hDevice, recvBuffer, sizeof(recvBuffer),
            &cbRead,
            NULL
        );

        //TODO parse connection info (ex : model, device name)
        break;
    }

exit:
    return retval;
}

BOOL
GetAirpodBatteryStatus(
    HANDLE hDevice
)
{
    BOOL retval = FALSE;
    DWORD cbWritten = 0;
    DWORD cbRead = 0;
    char recvBuffer[512] = { 0, };

    BOOL bRet = WriteFile(hDevice, request_status_msg, sizeof(request_status_msg), &cbWritten, NULL);

    if (!bRet) {
        printf("Write failed. Error: %d\n", GetLastError());
        goto exit;
    }

    for (;;) {
        bRet = ReadFile(hDevice, recvBuffer, sizeof(recvBuffer),
            &cbRead,
            NULL
        );

        if (!bRet) {
            break;
        }
        else {
            // battery status packet
            if (cbRead == 22) {
                printf("Battery Status(%%1 percent) : L : %d , R : %d, Case : %d\n", recvBuffer[9], recvBuffer[14], recvBuffer[19]);
                printf("Left unit in case : %s\n", recvBuffer[10] == 0x1 ? "true" : "false");
                printf("Right unit in case : %s\n", recvBuffer[15] == 0x1 ? "true" : "false");
                break;
            }

            ZeroMemory(recvBuffer, cbRead);
        }
    }

exit:
    return retval;
}

BOOL
SetANCMode(
    HANDLE hDevice,
    int mode
) {
    BOOL retval = FALSE;
    DWORD cbWritten = 0;

    change_mode_base_msg[7] = (unsigned char)mode;

    BOOL bRet = WriteFile(hDevice, change_mode_base_msg, sizeof(change_mode_base_msg), &cbWritten, NULL);

    if (!bRet) {
        printf("Write failed. Error: %d\n", GetLastError());
        goto exit;
    }

exit:
    return retval;
}

DWORD
GetDevicePath(
    _In_ LPGUID InterfaceGuid,
    _Out_ LPWSTR* DevicePath
)
{
    CONFIGRET cmRet = CR_SUCCESS;
    ULONG interfaceListSize = 0;

    *DevicePath = NULL;

    cmRet = CM_Get_Device_Interface_List_Size_Ex(&interfaceListSize, InterfaceGuid, 0, CM_GET_DEVICE_INTERFACE_LIST_PRESENT, NULL);
    if (cmRet != CR_SUCCESS) {
        goto exit;
    }

    *DevicePath = (LPWSTR)LocalAlloc(LMEM_ZEROINIT, interfaceListSize * sizeof((*DevicePath)[0]));
    if (NULL == *DevicePath) {
        cmRet = CR_OUT_OF_MEMORY;
        goto exit;
    }

    cmRet = CM_Get_Device_Interface_List_Ex(InterfaceGuid, 0, *DevicePath, interfaceListSize, CM_GET_DEVICE_INTERFACE_LIST_PRESENT, NULL);

exit:

    if (cmRet != CR_SUCCESS) {
        if (NULL != *DevicePath) {
            LocalFree(*DevicePath);
            *DevicePath = NULL;
        }
    }

    return CM_MapCrToWin32Err(cmRet, ERROR_UNIDENTIFIED_ERROR);
}