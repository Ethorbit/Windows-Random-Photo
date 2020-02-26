#include "main.h"
#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <sciter-win-main.cpp>

class MainWindow : public sciter::window 
{
public:
	MainWindow() : window(SW_TITLEBAR | SW_CONTROLS | SW_RESIZEABLE | SW_MAIN | SW_ENABLE_DEBUG, { 0, 0, 500, 500 }) {};

	BEGIN_FUNCTION_MAP
		//FUNCTION_0("customMessage", customFunc);
	END_FUNCTION_MAP

	sciter::string customFunc()
	{
		return L"Hi, mf..";
	}
};

#include "resources.cpp"

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
	return run();
}