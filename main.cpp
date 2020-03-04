#include "main.h"

class MainWindow : public sciter::window
{
public:
	MainWindow() :
		window
		(
			SW_TITLEBAR |
			SW_RESIZEABLE |
			SW_CONTROLS |
			SW_MAIN |
			SW_ENABLE_DEBUG,
			{ 0, 0, 500, 500 }
		){};

	LRESULT on_message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (uMsg == WM_GETMINMAXINFO)
		{
			MINMAXINFO* minMax = (MINMAXINFO*)lParam;
			minMax->ptMinTrackSize.y = 120;
		}

		return 0;
	}
}; 

int uimain(std::function<int()> run)
{
	SciterSetOption
	(
		NULL,
		SCITER_SET_SCRIPT_RUNTIME_FEATURES | SCITER_TRANSPARENT_WINDOW,
		ALLOW_FILE_IO |
		ALLOW_SOCKET_IO |
		ALLOW_SYSINFO |
		ALLOW_EVAL
	);
	
	sciter::archive::instance().open(aux::elements_of(resources)); 
	
	aux::asset_ptr<MainWindow> mainWin = new MainWindow;
	mainWin->load(L"this://app/index.html");
	mainWin->expand();

	sciter::dom::element root = mainWin->get_root();
	HWND wnd = mainWin->get_hwnd();

	//BEHAVIOR_EVENT_PARAMS params;
	//params.cmd = CUSTOM;
	//params.he = root;
	//params.heTarget = root;
	//params.name = L"mousestop";
	//root.fire_event(params, false);
	
	return run();
}