
#include <Windows.h>
#include <windows.h>
#include <tchar.h>
#include <Dbt.h>
#include <setupapi.h>
#include <iostream>
#include <cstring>
//#include <atlstr.h> // CString
using namespace std;
 
#pragma comment (lib, "Kernel32.lib")
#pragma comment (lib, "User32.lib")
 
#define THRD_MESSAGE_EXIT WM_USER + 1
const _TCHAR CLASS_NAME[] = _T("Sample Window Class");
 
HWND hWnd;
 
static const GUID GUID_DEVINTERFACE_LIST[] =
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }
};
 
void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
	if (DBT_DEVICEARRIVAL == wParam) \
		printf("add\n");
	else
		printf("del\n");
	/*
	CString szDevId = pDevInf->dbcc_name + 4;
	int idx = szDevId.ReverseFind(_T('#'));
	szDevId.Truncate(idx);
	szDevId.Replace(_T('#'), _T('\\'));
	szDevId.MakeUpper();
 
	CString szClass;
	idx = szDevId.Find(_T('\\'));
	szClass = szDevId.Left(idx);
 
	CString szTmp;
	if (DBT_DEVICEARRIVAL == wParam) \
		szTmp.Format(_T("Adding %s\r\n"), szDevId.GetBuffer());
	else
		szTmp.Format(_T("Removing %s\r\n"), szDevId.GetBuffer());
 
	_tprintf(szTmp);
	*/
}
 
LRESULT DeviceChange(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch (pHdr->dbch_devicetype)
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			UpdateDevice(pDevInf, wParam);
			break;
 
		case DBT_DEVTYP_HANDLE:
			pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			break;
 
		case DBT_DEVTYP_OEM:
			pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			break;
 
		case DBT_DEVTYP_PORT:
			pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			break;
 
		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			break;
		}
	}
	return 0;
}
 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		break;
	case WM_SIZE:
		break;
	case WM_DEVICECHANGE:
		return DeviceChange(message, wParam, lParam);
	}
 
	return DefWindowProc(hWnd, message, wParam, lParam);
}
 
ATOM MyRegisterClass()
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;
	return RegisterClass(&wc);
}
 
bool CreateMessageOnlyWindow()
{
	hWnd = CreateWindowEx(0, CLASS_NAME, _T(""), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,       // Parent window    
		NULL,       // Menu
		GetModuleHandle(NULL),  // Instance handle
		NULL        // Additional application data
		);
 
	return hWnd != NULL;
}
 
void RegisterDeviceNotify()
{
	HDEVNOTIFY hDevNotify;
	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
}
 
DWORD WINAPI ThrdFunc(LPVOID lpParam)
{
	if (0 == MyRegisterClass())
		return -1;
 
	if (!CreateMessageOnlyWindow())
		return -1;
 
	RegisterDeviceNotify();
 
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == THRD_MESSAGE_EXIT)
		{
			cout << "worker receive the exiting Message..." << endl;
			return 0;
		}
 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
 
	return 0;
}
 
int main(int argc, char** argv)
{
	DWORD iThread;
	HANDLE hThread = CreateThread(NULL, 0, ThrdFunc, NULL, 0, &iThread);
	if (hThread == NULL) {
		cout << "error" << endl;
		return -1;
	}
 
	char chQtNum;
	do
	{
		cout << "enter Q/q for quit: " << endl;
		cin >> chQtNum;
 
	} while (chQtNum != 'Q' && chQtNum != 'q');
 
	PostThreadMessage(iThread, THRD_MESSAGE_EXIT, 0, 0);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}
