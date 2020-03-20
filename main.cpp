#include <sciter-win-main.cpp>
#include "resources.cpp"
#include <thread>
#include "funcs.cpp"

class MainWindow : public sciter::window
{
public:
	MainWindow() :
		window
		(
			SW_CONTROLS |
			SW_ENABLE_DEBUG |
			SW_TITLEBAR |
			SW_MAIN |
			SW_RESIZEABLE,
			{ 0, 0, 500, 500 }
		){};
	
	BEGIN_FUNCTION_MAP
		FUNCTION_3("saveINI", saveINI)
		FUNCTION_2("getINI", getINI)
	END_FUNCTION_MAP

	LRESULT on_message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (uMsg == WM_COMMAND)
		{
			if (wParam == BringWinToTop)
			{	
				sciter::value isMaxed = ::root.eval(L"view.windowState", 20);
				if (isMaxed == 2) // Main window is minimized
					::root.eval(L"view.windowState = View.WINDOW_SHOWN;", 40); 

				::root.eval(L"self.state.focus = true;", 30); // Bring window to top
			}
		}

		if (uMsg == WM_COPYDATA)
		{
			COPYDATASTRUCT* cpStruct = (COPYDATASTRUCT*)lParam;
			const wchar_t* imgPath = (wchar_t*)cpStruct->lpData;
			sciter::value imgPathVal = imgPath;
			SciterCallScriptingFunction(::root, "directImage", &imgPathVal, 1, NULL);
		}

		return 0;
	}
}; 

class EventHandler : public sciter::event_handler
{
public:
	EventHandler() 
	{
		m_bCheckingCursor = false;
	};
	
	bool handle_mouse(HELEMENT he, MOUSE_PARAMS& params)
	{ 
		if (params.cmd == MOUSE_MOVE)
		{
			if (!m_bCheckingCursor)
			{
				m_bCheckingCursor = true;
				POINT prevPoint{ 0 };
				GetCursorPos(&prevPoint);
				auto checkCursor = [prevPoint]()
				{
					Sleep(100);
					POINT nextPoint{ 0 };
					GetCursorPos(&nextPoint);

					if (prevPoint.x == nextPoint.x && prevPoint.y == nextPoint.y)
					{
						BEHAVIOR_EVENT_PARAMS params{ 0 };
						params.cmd = CUSTOM;
						params.he = root;
						params.heTarget = root;
						params.name = L"mousestop";
						root.fire_event(params, true);
					}
				};
				std::thread t(checkCursor);
				t.detach();
				m_bCheckingCursor = false;
				return false;
			}
		}
		return false;
	}

private:
	bool m_bCheckingCursor;
};

int uimain(std::function<int()> run)
{
	LPVOID uniqueID = nullptr; // Only used if this is a different instance to identify the original

	// Save first argument as it will be img path due to opening image with program:
	const wchar_t* openImage = L"";
	if (sciter::application::argv().size() >= 2)
		openImage = sciter::application::argv().at(1).c_str();
	
	// Save PID to string:
	wchar_t mutexName[100];
	swprintf_s(mutexName, sizeof(mutexName) / sizeof(wchar_t), L"%i", _getpid());

	//////////////////////////////////////////////////////////////////////////////////
	// Save PID to memory, if it's been saved already then this is a second instance.
	// From here we can stop multiple instances from running:
	HANDLE openMap = OpenFileMappingW(FILE_MAP_READ, false, L"sRandomPhotoMemMap");
	if (openMap == NULL) // It has not been saved yet, this is the program's first instance
	{
		openMap = CreateFileMappingW
		(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			120,
			L"sRandomPhotoMemMap"
		);

		if (openMap == NULL) MessageBox(NULL, L"It failed first?", L"", MB_OK);

		LPVOID mapPtr = MapViewOfFile(openMap, FILE_MAP_ALL_ACCESS, 0, 0, 108);
		CopyMemory(mapPtr, &mutexName, 108);
		if (mapPtr == NULL)
		{
			MessageBox(NULL, L"It worked?", L"", MB_OK);
		}
	}
	else // The PID has been saved already, this is a second instance of the program
	{
		openMap = OpenFileMappingW(FILE_MAP_READ, false, L"sRandomPhotoMemMap");
		if (openMap != NULL)
		{
			uniqueID = MapViewOfFile(openMap, FILE_MAP_READ, 0, 0, 108);
			if (uniqueID != nullptr) // Successfully read the saved memory
			{
				int matchPID = -1; // Original instance's process ID
				matchPID = _wtoi((wchar_t*)uniqueID);				
				EnumWindows(WindowIteratorCB, (LPARAM)matchPID); // Save the original process's window handle

				if (openImage != L"") // The user is trying to open an image directly with a new instance, close the original instance
				{
					SendPath(originalWnd, openImage);
				}

				SendMessageW(originalWnd, WM_COMMAND, BringWinToTop, NULL);
				ExitProcess(0); 
			}
		}
	}

	SciterSetOption
	(
		NULL,
		SCITER_SET_SCRIPT_RUNTIME_FEATURES,
		ALLOW_FILE_IO |
		ALLOW_SOCKET_IO |
		ALLOW_SYSINFO |
		ALLOW_EVAL
	);

	sciter::archive::instance().open(aux::elements_of(resources)); 
	
	aux::asset_ptr<MainWindow> mainWin = new MainWindow;
	mainWin->load(L"this://app/index.html");
	mainWin->expand();	

	::root = mainWin->get_root();

	EventHandler eh;
	root.attach_event_handler(&eh);

	sciter::value res = ::root.eval(L"System.home('settings.ini')", 30);
	iniPath = res.get(L"").c_str();

	if (openImage != L"")
		SendPath(mainWin->get_hwnd(), openImage);

	return run();
}