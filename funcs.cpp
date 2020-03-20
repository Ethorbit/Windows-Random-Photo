#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <string>

#define BringWinToTop 4928
#define OpenImage 4930

static sciter::dom::element root;
static std::wstring iniPath;

static sciter::value saveINI(sciter::value section, sciter::value key, sciter::value value)
{
	WritePrivateProfileStringW
	(
		sciter::string(section.get(L"")).c_str(),
		sciter::string(key.get(L"")).c_str(),
		sciter::string(value.get(L"")).c_str(),
		iniPath.c_str()
	);

	return sciter::value();
}

static sciter::value getINI(sciter::value section, sciter::value key)
{
	wchar_t returnedVal[200];
	GetPrivateProfileStringW
	(
		sciter::string(section.get(L"")).c_str(),
		sciter::string(key.get(L"")).c_str(),
		L"",
		returnedVal,
		sizeof(returnedVal) / sizeof(wchar_t),
		iniPath.c_str()
	);

	return sciter::value(returnedVal);
}

// Save the HWND owned by the original instance's PID:
static HWND originalWnd = NULL;
static BOOL CALLBACK WindowIteratorCB(HWND hWnd, LPARAM lParam)
{
	int comparePID = (int)lParam;
	DWORD thisPID = 0x0;

	GetWindowThreadProcessId(hWnd, &thisPID);

	if ((DWORD)comparePID == thisPID)
	{
		originalWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}

static void SendPath(HWND hWnd, const wchar_t* imgPath)
{
	COPYDATASTRUCT cStruct;
	cStruct.dwData = 1;
	cStruct.lpData = (PVOID)imgPath;
	cStruct.cbData = sizeof(imgPath) * wcslen(imgPath) + 1;
	SendMessageW(hWnd, WM_COPYDATA, NULL, (LPARAM)&cStruct);
}