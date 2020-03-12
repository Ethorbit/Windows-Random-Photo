#include <sciter-x.h>
#include <sciter-x-window.hpp>
static sciter::window* sWndPtr;

static sciter::value saveINI(sciter::value key, sciter::value value)
{
	sciter::string keyStr = key.get(L"");
	sciter::string valueStr = value.get(L"");

	if (!sWndPtr) return sciter::value();
	SCITER_VALUE res;
	SciterCall(sWndPtr->get_hwnd(), "SciterGetINIPath", 1, &SCITER_VALUE(NULL), &res);

	WritePrivateProfileStringW
	(
		L"Other",
		keyStr.c_str(),
		valueStr.c_str(),
		sciter::string(res.get(L"")).c_str()
	);

	return sciter::value();
}

static sciter::value getINI(sciter::value key)
{
	sciter::value value;
	return sciter::value(value);
}