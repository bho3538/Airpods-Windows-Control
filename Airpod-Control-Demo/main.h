#pragma once
#define INITGUID
#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <cfgmgr32.h>
// init connection
unsigned char request_init_con[] = { 00,00,04,00,01,00,02,00,00,00,00,00,00,00,00,00 };

// request battery status
unsigned char request_status_msg[] = { 04, 00, 04, 00, 0x0F, 00, 0xFF,0xFF,0xFF, 0xFF };

// request set mode
unsigned char change_mode_base_msg[] = { 04, 00, 04, 00, 0x9, 00, 0xD, 0xFF, 00, 00, 00 };

DEFINE_GUID(BTHECHOSAMPLE_DEVICE_INTERFACE, 0xfc71b33d, 0xd528, 0x4763, 0xa8, 0x6c, 0x78, 0x77, 0x7c, 0x7b, 0xcd, 0x7b);

DWORD
GetDevicePath(
    _In_ LPGUID InterfaceGuid,
    _Out_ LPWSTR* DevicePath
);

BOOL
InitConnection(
    HANDLE hDevice
);

BOOL
GetAirpodBatteryStatus(
    HANDLE hDevice
);

BOOL
SetANCMode(
    HANDLE hDevice,
    int mode
);