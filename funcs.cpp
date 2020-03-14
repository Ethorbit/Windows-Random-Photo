#include <sciter-x.h>
#include <sciter-x-window.hpp>

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