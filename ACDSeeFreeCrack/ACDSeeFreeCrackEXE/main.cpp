#include <string>
#include <iostream>
#include <iomanip>
#include <Windows.h>

int main()
{
    HWND hStd = GetConsoleWindow();
    ShowWindow(hStd, SW_HIDE);

    STARTUPINFOA si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    CreateProcessA("../ACDSeeGFMF.exe",
                   NULL, NULL, NULL, FALSE, 0, NULL,
                   "../", &si, &pi);

    const char* pCrackDll = "./Crack/ACDSeeFreeCrackDLL.dll";
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);
    LPVOID lpSpace = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(pCrackDll), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, lpSpace, pCrackDll, strlen(pCrackDll), NULL);

    HMODULE hModule = GetModuleHandleA("kernel32.dll");
    LPVOID lpBaseAddress = (LPVOID)GetProcAddress(hModule,"LoadLibraryA");
    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpBaseAddress, lpSpace, NULL, NULL);

    return 0;
}
