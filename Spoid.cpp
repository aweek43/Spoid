#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>

LRESULT CALLBACK WindowProcMainWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcFakeWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcPreviewWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
std::wstring s2ws(const std::string& s);
std::string colorFormat(std::string s);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t MainWindow_CLASS_NAME[] = L"MainWindow";
    const wchar_t FakeWindow_CLASS_NAME[] = L"FakeWindow";
    const wchar_t PreviewWindow_CLASS_NAME[] = L"PreviewWindow";

    // regist wc
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProcMainWindow;
    wc.hInstance = hInstance;
    wc.lpszClassName = MainWindow_CLASS_NAME;
    RegisterClass(&wc);

    wc = { };
    wc.lpfnWndProc = WindowProcFakeWindow;
    wc.hInstance = hInstance;
    wc.lpszClassName = FakeWindow_CLASS_NAME;
    RegisterClass(&wc);

    wc = { };
    wc.lpfnWndProc = WindowProcPreviewWindow;
    wc.hInstance = hInstance;
    wc.lpszClassName = PreviewWindow_CLASS_NAME;
    RegisterClass(&wc);

    // Main Window.
    HWND hwndMainWindow = CreateWindowEx(
        0,                              // Optional window styles.
        MainWindow_CLASS_NAME,                     // Window class
        L"Spoid",    // Window text
        // 최대화, 최소화 버튼 제거
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 250, 120,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    if (hwndMainWindow == NULL)
    {
        return 0;
    }

    HWND hwndFakeWindow = CreateWindowEx(
        0,                              // Optional window styles.
        FakeWindow_CLASS_NAME,                     // Window class
        L"",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // 최대화
        0, 0, 99999, 99999,

        hwndMainWindow,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    if (hwndFakeWindow == NULL)
    {
        return 0;
    }

    // Preview Window.
    HWND hwndPreviewWindow = CreateWindowEx(
        0,                              // Optional window styles.
        PreviewWindow_CLASS_NAME,                     // Window class
        L"Preview",    // Window text
        // title bar 제거
        WS_OVERLAPPEDWINDOW & WS_BORDER,            // Window style

        // Size and position
        0, 0, 200, 100,

        hwndMainWindow,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    if (hwndPreviewWindow == NULL)
    {
        return 0;
    }

    // Fake Window에 투명도 설정 추가
    SetWindowLong(hwndFakeWindow, GWL_EXSTYLE, GetWindowLong(hwndFakeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwndFakeWindow, 0, 1, LWA_ALPHA);

    // show main window
    ShowWindow(hwndMainWindow, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LPCWSTR colorText = L"RGB = ";
std::wstring stemp;
COLORREF color = NULL;
HCURSOR normalCursor = LoadCursor(NULL, IDC_ARROW);
HCURSOR spoidCursor = LoadCursor(NULL, IDC_CROSS);

LRESULT CALLBACK WindowProcMainWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    LPCWSTR szMessage = L"우클릭하면 스포이드 시작";

    switch (uMsg)
    {
    case WM_MOUSEMOVE:
    {
        SetCursor(normalCursor);
        return 0;
    }
    case WM_RBUTTONDOWN: // 스포이드 활성화
    {
        // find fake&preview window
        HWND fakeWindow = FindWindow(L"FakeWindow", NULL);
        HWND previewWindow = FindWindow(L"PreviewWindow", NULL);

        // hide main window
        ShowWindow(hwnd, SW_HIDE);
        // show fake&preview window
        ShowWindow(fakeWindow, SW_SHOW);
        ShowWindow(previewWindow, SW_SHOW);

        // set cursor
        SetCursor(spoidCursor);

        return 0;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 20, 20, szMessage, wcslen(szMessage));
        TextOut(hdc, 20, 40, colorText, wcslen(colorText));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProcFakeWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN: // 색상 선택
    {
        // find main&preview window
        HWND mainWindow = FindWindow(L"MainWindow", NULL);
        HWND previewWindow = FindWindow(L"PreviewWindow", NULL);

        // hide fake&preview window
        ShowWindow(hwnd, SW_HIDE);
        ShowWindow(previewWindow, SW_HIDE);
        // show main window
        ShowWindow(mainWindow, SW_SHOW);

        return 0;
    }
    case WM_MOUSEMOVE:
    {
        // set cursor
        SetCursor(spoidCursor);

        HDC hdc;
        POINT p;

        hdc = GetDC(NULL);
        GetCursorPos(&p);
        color = GetPixel(hdc, p.x, p.y);

        int red = (int)GetRValue(color);
        int green = (int)GetGValue(color);
        int blue = (int)GetBValue(color);

        std::string _color = "RGB = " + colorFormat(std::to_string(red)) + ", " +
                                        colorFormat(std::to_string(green)) + ", " +
                                        colorFormat(std::to_string(blue));
        stemp = s2ws(_color);
        colorText = stemp.c_str();

        // draw preview window
        InvalidateRect(FindWindow(L"PreviewWindow", NULL), NULL, TRUE);

        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProcPreviewWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect = { 120,25,160,60 };
    HBRUSH brush = CreateSolidBrush(color);
    switch (uMsg)
    {
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 5, 5, colorText, wcslen(colorText));
        FillRect(hdc, &rect, brush);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

// 자리수가 안맞으면 이전에 paint된 text가 그대로 남아있음
std::string colorFormat(std::string s)
{
    switch (s.length())
    {
    case 1:
        s = "00" + s;
        break;
    case 2:
        s = "0" + s;
        break;
    default:
        break;
    }
    return s;
}