#include <Windows.h>
#include <iostream>

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

STARTUPINFO psi;
PROCESS_INFORMATION pi;

int main()
{
    std::wstring path = ExePath() + L"\\TestConsole.exe";
    std::wstring dll = ExePath() + L"\\InterceptionDll.dll";

    CreateProcess(path.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &psi, &pi);

    HMODULE kernelModule = GetModuleHandle(L"Kernel32");
    if (kernelModule < (HMODULE)HINSTANCE_ERROR)
    {
        std::cout << "Cannot load Kernel32 module";
        return -1;
    }
    void* loadLibraryW = GetProcAddress(kernelModule, "LoadLibraryW");
    LPVOID hMem = VirtualAllocEx(pi.hProcess, NULL, dll.size() * sizeof(wchar_t) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (hMem == NULL)
    {
        std::cout << "Cannot allocate memory";
        return -1;
    }
    WriteProcessMemory(pi.hProcess, hMem, dll.c_str(), dll.size() * sizeof(wchar_t) + 1, NULL);

    HANDLE hRemoteThread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibraryW, hMem, NULL, NULL);
    if (hRemoteThread == NULL)
    {
        std::cout << "Cannot create remote thread";
        return -1;
    }
    WaitForSingleObject(hRemoteThread, INFINITE);
    CloseHandle(hRemoteThread);

    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);

    VirtualFreeEx(pi.hProcess, hMem, 0, MEM_RELEASE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
