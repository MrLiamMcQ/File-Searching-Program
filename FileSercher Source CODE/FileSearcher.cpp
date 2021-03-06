// FileSearcher.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FileSearcher.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>

#define Button1 99
int boxlen = 148;
wchar_t targ[100];
HWND hInputText;
HWND hComboBox1;
HWND hComboBox2;
HWND hComboBox3;
std::wstring old;
WCHAR szBuffer[MAX_PATH] = { 0 };
DWORD Drives = GetLogicalDriveStrings(MAX_PATH, szBuffer);
int DriveNumb = Drives / 4;
std::wstring drive[10];
std::wstring driveS[10];

bool notWindows = false;
int DIndex = 0;
int FileOut = 1;
int OutType = 0;
int ScanType;

class SerchSettings {
public:
	int F_1 = 15;
	int F_2 = 19;
	int S_1 = 15;//15
	int S_2 = 19;//19
}; SerchSettings Settings;

void SmartSerch() {
	Settings.F_1 = 15;
	Settings.F_2 = 18;
	Settings.S_1 = 15;
	Settings.S_2 = 18;
	notWindows = true;
}

void FolderOnly() {
	Settings.F_1 = 15;
	Settings.F_2 = 17;
	Settings.S_1 = 15;
	Settings.S_2 = 17;
	notWindows = true;
}

void AllDirects() {
	Settings.F_1 = 15;
	Settings.F_2 = 19;
	Settings.S_1 = 15;
	Settings.S_2 = 19;
	notWindows = false;
}

void FolderFind(std::wstring Folder) {
	if (Folder.find(targ) != std::string::npos && Folder != old)
	{
		old = Folder; std::wcout << Folder << std::endl;
	}
}

void serch(LPCWSTR FolderName) {
	WIN32_FIND_DATA file;
	HANDLE search_handle = FindFirstFile(FolderName, &file);
	do
	{
		std::wstring File = file.cFileName;
		std::wstring Folder = FolderName;
		Folder.erase(Folder.size() - 1);

		if (File.find(targ) != std::string::npos)
		{
			switch (OutType)
			{
			case 2: std::wcout << File << std::endl; break;
			case 0: std::wcout << Folder << File << std::endl;  break;
			case 1: FolderFind(Folder);  break;
			default:
				break;
			}
		}

		if (file.dwFileAttributes > Settings.S_1 && file.dwFileAttributes < Settings.S_2) {
			if (char(file.cFileName[0]) != '.') {
				std::wstring EditedFile = File;
				EditedFile.insert(0, Folder);
				EditedFile.append(L"\\*");
				LPCWSTR c = EditedFile.c_str();
				serch(c);
			}
		}

	} while (FindNextFile(search_handle, &file));
	FindClose(search_handle);
}

void GetDrives() {
	for (int y = 1; y < DriveNumb + 1; y++) {
		for (int i = 4 * y - 4; i < 4 * y; i++) {
			drive[y - 1] += szBuffer[i];
		}
	}
	int i = 0;
	do {
		drive[i].append(L"*");
		char star = drive[i][4];
		char end = drive[i][3];
		driveS[i] = drive[i];
		driveS[i][4] = end;
		driveS[i][3] = star;
		SendMessage(hComboBox2, CB_ADDSTRING, 0, (LPARAM)drive[i].c_str());//(LPARAM)
i++;
	} while (DriveNumb > i); 
	SendMessage(hComboBox2, CB_ADDSTRING, 0, (LPARAM)L"All Drives");
	SendMessage(hComboBox2, CB_SETCURSEL, 0, 0);
}

void MainSerch(int i) {
	WIN32_FIND_DATA file;
	HANDLE search_handle = FindFirstFile(driveS[i].c_str(), &file);
	do {

		if (file.dwFileAttributes > Settings.F_1 && file.dwFileAttributes < Settings.F_2) {
			std::wstring FileName(file.cFileName);

			std::wstring EditedFile = FileName;
			EditedFile.insert(0, drive[i].c_str());
			EditedFile.append(L"\\*");

			LPCWSTR c = EditedFile.c_str();

			//std::wcout << file.dwFileAttributes << ":::::::::" << file.cFileName << std::endl;
			if (notWindows == true && FileName.find(L"Windows") == NULL)
			{
				continue;
			}
			else { std::thread t2(serch, c); t2.join(); }

		}
	} while (FindNextFile(search_handle, &file));
	FindClose(search_handle);
}

void MainLogic()
{
	FreeConsole();
	AllocConsole();
	freopen("CONOUT$", "w+", stdout);
	freopen("CONOUT$", "w+", stderr);

	ScanType = SendMessage(hComboBox3, CB_GETCURSEL, 0, 0);
	DIndex = SendMessage(hComboBox2, CB_GETCURSEL, 0, 0);
	OutType = SendMessage(hComboBox1, CB_GETCURSEL, 0, 0);

	std::wcout << "Search Term: " << targ << std::endl << std::endl;

	switch (ScanType) {
	case 0:SmartSerch(); break;
	case 2: FolderOnly(); break;
	case 1: AllDirects(); break;
	default:
		break;
	}

	if (DIndex == 3) { for (int i = 0; DriveNumb > i; i++) { std::thread t9(MainSerch, i); t9.join(); } }
	else { int i = DIndex; std::thread t90(MainSerch, i); t90.join(); }

	std::cout << std::endl << "Search Compleate" << std::endl;

	FreeConsole();
}


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings  
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FILESEARCHER,szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FILESEARCHER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILESEARCHER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FILESEARCHER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      10, 0, 490, 170, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		
		CreateWindowW(L"static", L"Search For:", WS_VISIBLE | WS_CHILD, 10, 5, boxlen, 20, hWnd,0,0,0);
		CreateWindowW(L"static", L"Search Location:", WS_VISIBLE | WS_CHILD , 160, 5, boxlen, 20, hWnd, 0, 0, 0);
		CreateWindowW(L"static", L"Search type:", WS_VISIBLE | WS_CHILD  , 310, 5, boxlen, 20, hWnd, 0, 0, 0);
		
		hComboBox1 =CreateWindowW(L"combobox", 0, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_BORDER | WS_VISIBLE, 10, 25, boxlen, 300, hWnd, 0, 0, 0);
		hComboBox2 = CreateWindowW(L"COMBOBOX", 0, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_BORDER | WS_VISIBLE, 160, 25, boxlen, 300, hWnd, 0, 0, 0);
		hComboBox3 = CreateWindowW(L"COMBOBOX", 0, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_BORDER | WS_VISIBLE, 310, 25, boxlen, 300, hWnd, 0, 0, 0);
		
		GetDrives();
			// Add string to combobox.

			SendMessage(hComboBox1, CB_ADDSTRING, 0, (LPARAM)_T("Files & Folders"));
			SendMessage(hComboBox1, CB_ADDSTRING, 0, (LPARAM)_T("Folders Only"));
			SendMessage(hComboBox1, CB_ADDSTRING, 0, (LPARAM)_T("File Names Only"));
			SendMessage(hComboBox1, CB_SETCURSEL, 0, 0);

			SendMessage(hComboBox3, CB_ADDSTRING, 0, (LPARAM)_T("Smart Search"));
			SendMessage(hComboBox3, CB_ADDSTRING, 0, (LPARAM)_T("All Directories"));
			SendMessage(hComboBox3, CB_ADDSTRING, 0, (LPARAM)_T("Folder Type 16"));
			SendMessage(hComboBox3, CB_SETCURSEL, 0, 0);

		hInputText = CreateWindowW(L"edit", 0, WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 60, 250, 25, hWnd, 0, 0, 0);
		CreateWindowW(L"button", L"Search", WS_VISIBLE | WS_CHILD  | SS_CENTER, 270, 60, 190, 25, hWnd, (HMENU)Button1, 0, 0);
		
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_SAVE:
				DialogBoxW(hInst, MAKEINTRESOURCE(IDD_SAVEBOX), hWnd, About);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case Button1:
				GetWindowTextW(hInputText,targ,100);
				MainLogic();

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


