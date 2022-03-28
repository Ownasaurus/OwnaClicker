#ifndef UNICODE
#define UNICODE
#endif 

#include <string>
#include <vector>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"

// Various globals
bool clicking = false;
bool sleepTextChanged = false;
const int ID_BUTTON1 = 401;
const int IDC_SLEEP_TEXT = 4001;
const int IDC_SLEEP_TEXT_GT = 4002;
const int IDC_CHK_L = 40001;
const int IDC_CHK_M = 40002;
const int IDC_CHK_R = 40003;
HWND hWndComboBox = NULL;
HWND hWndButton = NULL;
HWND hWndMain = NULL;
HWND hSleepText = NULL;
HWND hSleepTextGT = NULL;
HWND hMinecraftWindow = NULL;

HANDLE hThread = NULL;
DWORD dwThreadId = NULL;

int clickPeriod = 1500; // ms
UINT clickDown = WM_LBUTTONDOWN;
UINT clickUp = WM_LBUTTONUP;
WPARAM clickButton = MK_LBUTTON;

// Color brushes for the status rectangles
HBRUSH solidColorRed = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH solidColorGreen = CreateSolidBrush(RGB(0, 255, 0));

std::vector <std::wstring> windowNames;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ClickWindow(HWND hWnd)
{
    PostMessage(hWnd, clickDown, clickButton, 0);
    PostMessage(hWnd, clickUp, 0, 0);
}

DWORD WINAPI ClickyThread(LPVOID lpParam)
{

    while (true)
    {
        if (clicking)
        {
            ClickWindow(hMinecraftWindow);
            Sleep(clickPeriod);
        }
        else
        {
            Sleep(100);
        }
    }

    return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    int textLength = GetWindowTextLength(hWnd);
    TCHAR* buff = new TCHAR[textLength + 1];
    GetWindowText(hWnd, buff, textLength + 1);
    std::wstring windowTitle(buff);

    if (IsWindowVisible(hWnd) && textLength != 0)
    {
        windowNames.push_back(windowTitle);
    }

    delete[] buff;

    return TRUE;
}

void RefreshWindowList()
{
    windowNames.clear();
    EnumWindows(EnumWindowsProc, 0);
    size_t startIndex = 0;

    for (size_t x = 0; x < windowNames.size(); x++)
    {
        SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)windowNames.at(x).c_str());

        // by default, select something that has "Minecraft" in its name
        if (windowNames.at(x).find(L"Minecraft") != std::string::npos)
        {
            startIndex = x;
        }
    }

    SendMessage(hWndComboBox, CB_SETCURSEL, startIndex, 0);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const TCHAR CLASS_NAME[] = L"OwnaClicker";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClass(&wc);

    // main window
    hWndMain = CreateWindowEx(
        0,
        CLASS_NAME,
        L"OwnaClicker",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 250,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWndMain == NULL)
    {
        return 0;
    }

    // group box
    CreateWindow(
        L"Button",
        L"Settings",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, 5,
        220,200,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    // text label
    CreateWindow(
        L"STATIC",
        L"Window to click:",
        WS_CHILD | WS_VISIBLE,
        20, 30,
        200, 20,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );
    
    // drop down menu
    hWndComboBox = CreateWindow(
        WC_COMBOBOX,
        L"",
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
        20, 50,
        200, 200,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    // text label
    CreateWindow(
        L"STATIC",
        L"Enter time to sleep:",
        WS_CHILD | WS_VISIBLE,
        20, 90,
        200, 20,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    // Edit box (sleep time)
    hSleepText = CreateWindow(
        L"EDIT",
        L"1500",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
        20, 110,
        60, 20,
        hWndMain,
        (HMENU)IDC_SLEEP_TEXT,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL);

    // text label
    CreateWindow(
        L"STATIC",
        L"ms",
        WS_CHILD | WS_VISIBLE,
        82, 112,
        20, 20,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    // Edit box (sleep time)
    hSleepTextGT = CreateWindow(
        L"EDIT",
        L"30",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
        130, 110,
        60, 20,
        hWndMain,
        (HMENU)IDC_SLEEP_TEXT_GT,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL);

    // text label
    CreateWindow(
        L"STATIC",
        L"gt",
        WS_CHILD | WS_VISIBLE,
        192, 112,
        20, 20,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    // text label
    CreateWindow(
        L"STATIC",
        L"Mouse button:",
        WS_CHILD | WS_VISIBLE,
        20, 150,
        200, 20,
        hWndMain,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    // radio buttons for mouse click type
    CreateWindowEx(WS_EX_WINDOWEDGE,
        L"BUTTON",
        L"Left",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
        20, 170,
        60, 20,
        hWndMain,
        (HMENU)IDC_CHK_L,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL);

    // radio buttons for mouse click type
    CreateWindowEx(WS_EX_WINDOWEDGE,
        L"BUTTON",
        L"Middle",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        80, 170,
        65, 20,
        hWndMain,
        (HMENU)IDC_CHK_M,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL);

    // radio buttons for mouse click type
    CreateWindowEx(WS_EX_WINDOWEDGE,
        L"BUTTON",
        L"Right",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        160, 170,
        60, 20,
        hWndMain,
        (HMENU)IDC_CHK_R,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL);

    SendDlgItemMessage(hWndMain, IDC_CHK_L, BM_SETCHECK, 1, 0);

    // the best button ever
    hWndButton = CreateWindow(
        L"BUTTON",
        L"Start",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        250, 20,
        100, 30,
        hWndMain,
        (HMENU)ID_BUTTON1,
        (HINSTANCE)GetWindowLongPtr(hWndMain, GWLP_HINSTANCE),
        NULL
    );

    ShowWindow(hWndMain, nCmdShow);

    // update our drop down menu
    RefreshWindowList();

    // start up our background clicking thread
    hThread = CreateThread(
        NULL,
        0,
        ClickyThread,
        0,
        0,
        &dwThreadId);

    if (!hThread) {
        MessageBox(hWndMain, L"Could not start thread! Exiting!", L"ERROR", NULL);
        return 1;
    }

    // message loop
    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
            if (wParam == ID_BUTTON1) // button clicked
            {
                if (!clicking) { // prepare to turn it on
                    const int index = SendMessage(hWndComboBox, CB_GETCURSEL, NULL, NULL);
                    const int len = SendMessage(hWndComboBox, CB_GETLBTEXTLEN, (WPARAM)index, NULL);

                    std::vector<TCHAR> buffer(len + 1);
                    SendMessage(hWndComboBox, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());

                    hMinecraftWindow = FindWindow(NULL, buffer.data()); // (no need to close this handle)

                    if (!hMinecraftWindow) {
                        MessageBox(hWndMain, L"Could not get a handle to target window! Exiting!", L"ERROR", NULL);
                        exit(1);
                    }

                    SendMessage(hWndButton, WM_SETTEXT, 0, (LPARAM)L"Stop");
                }
                else // prepare to turn it off
                {
                    SendMessage(hWndButton, WM_SETTEXT, 0, (LPARAM)L"Start");
                }

                clicking = !clicking;

                // force redraw
                InvalidateRect(hWnd, NULL, FALSE);
                UpdateWindow(hWnd);
            }
            else if (wParam == IDC_CHK_L)
            {
                clickDown = WM_LBUTTONDOWN;
                clickUp = WM_LBUTTONUP;
                clickButton = MK_LBUTTON;
            }
            else if (wParam == IDC_CHK_M)
            {
                clickDown = WM_MBUTTONDOWN;
                clickUp = WM_MBUTTONUP;
                clickButton = MK_MBUTTON;
            }
            else if (wParam == IDC_CHK_R)
            {
                clickDown = WM_RBUTTONDOWN;
                clickUp = WM_RBUTTONUP;
                clickButton = MK_RBUTTON;
            }
            else if (LOWORD(wParam) == IDC_SLEEP_TEXT && HIWORD(wParam) == EN_CHANGE) // sleep text field changed
            {
                if (!sleepTextChanged)
                {
                    sleepTextChanged = true;
                    TCHAR buff[128];
                    GetWindowText(hSleepText, buff, sizeof(buff));
                    int conversion = _wtoi(buff);
                    if (conversion > 0) // 0 indicates error, negative is invalid
                    {
                        clickPeriod = conversion;
                    }

                    conversion /= 50;
                    _itow_s(conversion, buff, 128, 10);
                    SetWindowText(hSleepTextGT, buff);
                }
                else
                {
                    sleepTextChanged = false;
                }
            }
            else if (LOWORD(wParam) == IDC_SLEEP_TEXT_GT && HIWORD(wParam) == EN_CHANGE) // sleep text field changed
            {
                if (!sleepTextChanged)
                {
                    sleepTextChanged = true;
                    TCHAR buff[128];
                    GetWindowText(hSleepTextGT, buff, sizeof(buff));
                    int conversion = _wtoi(buff);

                    conversion *= 50;
                    if (conversion > 0) // 0 indicates error, negative is invalid
                    {
                        clickPeriod = conversion;
                    }

                    _itow_s(conversion, buff, 128, 10);
                    SetWindowText(hSleepText, buff);
                }
                else
                {
                    sleepTextChanged = false;
                }
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

            RECT square;
            square.left = 250;
            square.right = 350;
            square.top = 75;
            square.bottom = 175;

            if (clicking) {
                FillRect(hdc, &square, solidColorGreen);
            }
            else {
                FillRect(hdc, &square, solidColorRed);
            }

            EndPaint(hWnd, &ps);
            break;

    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
