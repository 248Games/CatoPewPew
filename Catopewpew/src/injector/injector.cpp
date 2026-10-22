#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

// Minimal injector: loads catopewpew.dll into a running process by name.
// With Extreme Injector you can skip this and just load the DLL directly.
int main() {
    const char* procName = "catopewpew.exe";
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe{}; pe.dwSize = sizeof(pe);
    if (Process32First(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"catopewpew.exe") == 0) { pid = pe.th32ProcessID; break; }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    if (!pid) { printf("process %s not found\n", procName); return 1; }

    char dll[512]; GetModuleFileNameA(nullptr, dll, sizeof(dll));
    *strrchr(dll, '\\') = 0;
    strcat_s(dll, "catopewpew.dll");

    HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    char* r = (char*)VirtualAllocEx(h, nullptr, strlen(dll)+1, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(h, r, dll, strlen(dll)+1, nullptr);
    HMODULE k32 = GetModuleHandleA("kernel32.dll");
    void* llib = (void*)GetProcAddress(k32, "LoadLibraryA");
    HANDLE t = CreateRemoteThread(h, nullptr, 0, (LPTHREAD_START_ROUTINE)llib, r, 0, nullptr);
    WaitForSingleObject(t, 8000);
    CloseHandle(t); CloseHandle(h);
    printf("injected into pid %lu\n", pid);
    return 0;
}
