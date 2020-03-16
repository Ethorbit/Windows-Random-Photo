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
		)
	{};

	BEGIN_FUNCTION_MAP
		FUNCTION_3("saveINI", saveINI)
		FUNCTION_2("getINI", getINI)
	END_FUNCTION_MAP

	LRESULT on_message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (uMsg == WM_GETMINMAXINFO)
		{
			// Maybe useful someday?
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

	return run();
}