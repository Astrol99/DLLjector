#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetPID(const char* windowName)
{
	// Find pid of process through window name
	HWND windowHandle = FindWindow(NULL, windowName);

	DWORD* pid = new DWORD;
	if (!GetWindowThreadProcessId(windowHandle, pid))
		return NULL;

	std::cout << windowName << " PID: " << *pid << std::endl;
	return *pid;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "Usage: DLLjector.exe \"[window name]\" [path of dll]" << std::endl;
		return 1;
	}

	const char* windowName = argv[1];
	const char* dllPath = argv[2];

	const DWORD pid = GetPID(windowName);

	if (!pid)
	{
		std::cout << "Unable to find window..." << std::endl;
		return 1;
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		// Allocate memory in process for dll
		void* allocAddr = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		std::cout << "Allocated " << MAX_PATH << " bytes at 0x" << std::hex << allocAddr << std::endl;

		if (!allocAddr)
		{
			std::cout << "Invalid allocated address..." << std::endl;
			return 1;
		}

		// Inject dll path into allocated mem
		WriteProcessMemory(hProc, allocAddr, dllPath, strlen(dllPath) + 1, nullptr);
		std::cout << "Wrote " << strlen(dllPath) + 1 << " bytes into allocated address" << std::endl;

		// Call LoadLibrary on injected dll and create child thread
		// from parent process, running the injected DLL as a result
		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocAddr, 0, 0);
		std::cout << "Loaded DLL library into remote thread" << std::endl;

		// Cleanup
		if (hThread)
			CloseHandle(hThread);

		CloseHandle(hProc);

		std::cout << "Successfully injected " << dllPath << " into " << windowName << std::endl;
	}

	return 0;
}