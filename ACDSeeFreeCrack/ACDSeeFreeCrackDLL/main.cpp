#include <string>
#include <iostream>
#include <iomanip>
#include <thread>
#include <Windows.h>

// ACDSee窗口的标题
#define ACDSEEFREE_WINDOWTITLE              "ACDSee"
// ACDSee窗口的类名(图片窗口、查看窗口、编辑窗口)
#define ACDSEEFREE_WINDOWCLASSNAME_PICTURE  "Afx:00400000:0:00000000:00900015:"
#define ACDSEEFREE_WINDOWCLASSNAME_VIEW     "Afx:00400000:b:00010003:00000006:"
#define ACDSEEFREE_WINDOWCLASSNAME_EDIT     "Afx:00400000:0:00010003:00900006:"
// ACDSee广告对话框类名
#define ACDSEEFREE_ADDIALOGCLASSNAME        "#32770"
// ACDSee广告窗口类名
#define ACDSEEFREE_ADWINDOWCLASSNAME        "MainBannerBar:400000:8:10003:10"


HANDLE gThread = 0;
HWINEVENTHOOK gEventHook = 0;
BOOL gIsExit = FALSE;

void Log(std::string msg)
{
    msg = "HookProc:\t" + msg;
    OutputDebugStringA(msg.c_str());
}

// 枚举子窗口过程
BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM)
{
    std::string className, windowTitle;
    {
        char cn[1024] = {0};
        ::GetClassNameA(hWnd, cn, sizeof(cn));

        char wt[1024] = {0};
        ::GetWindowTextA(hWnd, wt, sizeof(wt));

        className   = cn;
        windowTitle = wt;
    }

    if ((className.find(ACDSEEFREE_ADWINDOWCLASSNAME) != std::string::npos) && windowTitle.empty())
    {
        ::EnableWindow(hWnd, false);
        ::ShowWindow(hWnd, SW_HIDE);
    }

    return TRUE;
}

// 禁用ACDSee免费版的广告窗口
void DisableACDSeeADWindow(HWND hACDSeeWindow)
{
    ::EnumChildWindows(hACDSeeWindow, EnumChildWindowsProc, LPARAM(0));

    RECT rc;
    ::GetWindowRect(hACDSeeWindow, &rc);
    ::MoveWindow(hACDSeeWindow, rc.left, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top - 1, false);
    ::MoveWindow(hACDSeeWindow, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
}

// 禁用ACDSee免费版的广告对话框
void DisableACDSeeADDialog(HWND hACDSeeDialog)
{
    ::EnableWindow(hACDSeeDialog, FALSE);
    ::ShowWindow(hACDSeeDialog, SW_HIDE);
    ::MoveWindow(hACDSeeDialog, 0, 0, 0, 0, true);
}

//// 枚举窗口过程
//BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
//{
//    do
//    {
//        DWORD processID;
//        GetWindowThreadProcessId(hWnd, &processID);

//        std::string windowTitle, className;
//        {
//            char wt[1024] = {0};
//            GetWindowTextA(hWnd, wt, sizeof(wt));

//            char cn[1024] = {0};
//            GetClassNameA(hWnd, cn, sizeof(cn));

//            windowTitle = wt;
//            className   = cn;
//        }

//        bool isPicture = className.find(ACDSEEFREE_WINDOWCLASSNAME_PICTURE) != std::string::npos;
//        bool isView    = className.find(ACDSEEFREE_WINDOWCLASSNAME_VIEW) != std::string::npos;
//        bool isEdit    = className.find(ACDSEEFREE_WINDOWCLASSNAME_EDIT) != std::string::npos;
//        bool isDialog  = className.find(ACDSEEFREE_DIALOGCLASSNAME) != std::string::npos;

//        if ((windowTitle.find(ACDSEEFREE_WINDOWTITLE) != std::string::npos) &&
//            (isPicture || isView || isEdit))
//        {
//            DisableACDSeeADWindow(hWnd);
//        }
//        else if (windowTitle.empty() && isDialog)
//        {
//            DisableACDSeeADDialog(hWnd);
//        }
//        else
//        {
//        }

//    } while (0);

//    return TRUE;
//}

//VOID CALLBACK WinEventsProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG object, LONG child, DWORD eventThread, DWORD eventTime )
//{
//    do
//    {
//         Log("Enter");

//        std::string className, windowTitle;
//        {
//            char cn[1024] = {0};
//            GetClassNameA(hWnd, cn, sizeof(cn));

//            char wt[1024] = {0};
//            GetWindowTextA(hWnd, wt, sizeof(wt));

//            className   = cn;
//            windowTitle = wt;
//        }

//        bool isPicture = (className.find(ACDSEEFREE_WINDOWCLASSNAME_PICTURE) != std::string::npos) &&
//                         (windowTitle.find(ACDSEEFREE_WINDOWTITLE) != std::string::npos);
//        bool isView = (className.find(ACDSEEFREE_WINDOWCLASSNAME_VIEW) != std::string::npos) &&
//                      (windowTitle.find(ACDSEEFREE_WINDOWTITLE) != std::string::npos);
//        bool isEdit = (className.find(ACDSEEFREE_WINDOWCLASSNAME_EDIT) != std::string::npos) &&
//                      (windowTitle.find(ACDSEEFREE_WINDOWTITLE) != std::string::npos);
//        if (isPicture || isView || isEdit)
//        {
//            DisableACDSeeADWindow(hWnd);
//            break;
//        }

//        bool isADDialog = (className.find(ACDSEEFREE_ADDIALOGCLASSNAME) != std::string::npos) &&
//                          windowTitle.empty();
//        if (isADDialog)
//        {
//            DisableACDSeeADDialog(hWnd);
//            break;
//        }

//    } while (0);
//}

VOID CALLBACK WinEventsProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG object, LONG child, DWORD eventThread, DWORD eventTime )
{
    HWND hAdWindow = FindWindowExA(hWnd, NULL, ACDSEEFREE_ADWINDOWCLASSNAME, NULL);
    if (!hAdWindow)
        return;

    ::MoveWindow(hAdWindow, 0, 0, 0, 0, TRUE);
    ::DestroyWindow(hAdWindow);
}

DWORD WINAPI ThreadFunc(LPVOID)
{
    gEventHook = SetWinEventHook(EVENT_MIN, EVENT_MAX,
                                 GetModuleHandleA("ACDSeeFreeCrackDLL.dll"), WinEventsProc,
                                 GetCurrentProcessId(), 0, WINEVENT_INCONTEXT);

    MSG msg;
    while(!gIsExit && ::GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ::UnhookWinEvent(gEventHook);
    gEventHook = 0;
    gThread = 0;

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DWORD threadID = 0;
        gThread = CreateThread(NULL, 0, ThreadFunc, 0, 0, &threadID);
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        gIsExit = TRUE;
        break;
    }
    case DLL_THREAD_ATTACH:
    {
        break;
    }
    case DLL_THREAD_DETACH:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    return TRUE;
}
