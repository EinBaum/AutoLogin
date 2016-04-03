
#include <windows.h>
#include <stdio.h>

DWORD processId;
char *account;
char *password;

// https://batchloaf.wordpress.com/2012/04/17/simulating-a-keystroke-in-win32-c-or-c-using-sendinput/
void SendKey(HANDLE hWnd, char key)
{
	INPUT ip;
	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press
	ip.ki.wVk = VkKeyScan(key);
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &ip, sizeof(INPUT));
}

void SendAndExit(HANDLE hWnd)
{
	char *pKey = account;
	while (*pKey != '\0') {
		SendKey(hWnd, *pKey);
		pKey++;
	}

	SendKey(hWnd, '\t');

	pKey = password;
	while (*pKey != '\0') {
		SendKey(hWnd, *pKey);
		pKey++;
	}

	SendKey(hWnd, '\n');

	exit(0);
}

BOOL CALLBACK Window_Callback(HWND hWnd, LONG lParam) {
    if (IsWindowVisible(hWnd)) {
    	DWORD testId;
	GetWindowThreadProcessId(hWnd, &testId);
	if (testId == processId) {
		SendAndExit(hWnd);
	}
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("Usage: %s GAMEPATH ACCOUNT PASSWORD\n", argv[0]);
		return 1;
	}

	STARTUPINFO si; ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	BOOL bResult = CreateProcess(argv[1],
					argv[1],
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					NULL,
					&si,
					&pi);

	if (!bResult) {
		printf("Failed to start WoW.\n");
		return 1;
	}

	processId = pi.dwProcessId;
	account = argv[2];
	password = argv[3];

	for (;;) {
		Sleep(200);
		EnumWindows(Window_Callback, 0);
	}

	return 0;
}
