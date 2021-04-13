#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetPID(const char* windowName)
{
	HWND windowHandle = FindWindow(NULL, windowName);

	DWORD* pid = new DWORD;
	if (!GetWindowThreadProcessId(windowHandle, pid))
		return NULL;

	std::cout << windowName << " PID: " << *pid << std::endl;
	return *pid;
}

int main()
{
	std::string windowName = "AssaultCube";
	DWORD pid = GetPID(windowName.c_str());

	if (!pid)
	{
		std::cout << "Unable to find window..." << std::endl;
		return 1;
	}
}