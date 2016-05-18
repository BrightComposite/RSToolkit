//---------------------------------------------------------------------------

#include <windows.h>
#include <application\Application.h>

#if defined(_MSC_VER) && (defined(_DEBUG) || defined(DEBUG))
//#include <vld.h>
#endif

//---------------------------------------------------------------------------

namespace Rapture
{
	void Application::main(int argc, wchar_t * argv[])
	{
		auto & inst = instance();

		if(inst.hInstance != nullptr)
			throw Exception("Application has been already loaded!");

		inst.hInstance = (HINSTANCE)GetModuleHandle(nullptr);

		for(int i = 0; i < argc; ++i)
			inst.startupArguments.emplace_back(argv[i]);

		STARTUPINFOW info;
		GetStartupInfoW(&info);

		inst.showCommand = check_flag(STARTF_USESHOWWINDOW, info.dwFlags) ? info.wShowWindow : SW_NORMAL;
		inst.rootPath = getDir(getDir(Application::getExecutionPath(inst.hInstance)));

	#ifdef _WIN64
		inst.rootPath = getDir(inst.rootPath);
	#endif
		load();
	}

	HINSTANCE Application::getWindowsInstance()
	{
		auto & inst = instance();
		return inst.hInstance;
	}

	const wstring & Application::getRootPath()
	{
		auto & inst = instance();
		return inst.rootPath;
	}

	const vector<wstring> & Application::getStartupArguments()
	{
		auto & inst = instance();
		return inst.startupArguments;
	}

	int Application::getShowCommand()
	{
		auto & inst = instance();
		return inst.showCommand;
	}

	void Application::load()
	{
		auto & inst = instance();

		for(auto & ent : inst.entrances)
			ent->enter();
	}

	wstring Application::getExecutionPath(HINSTANCE hInstance)
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(hInstance, buffer, MAX_PATH);

		return {buffer, wcslen(buffer)};
	}

	wstring getDir(wstring path)
	{
		return path.substr(0, path.find_last_of(L"/\\"));
	}
}

using namespace Rapture;

#ifdef __MINGW32__
#ifdef __cplusplus
	#define C_EXTERN "C"
#else
	#define C_EXTERN
#endif

	extern int _CRT_glob;
	extern C_EXTERN void __wgetmainargs(int*,wchar_t***,wchar_t***,int,int*);

	int wmain(int argc, wchar_t * argv[]);

	int main()
	{
		wchar_t **enpv, **argv;
		int argc, si = 0;
		__wgetmainargs(&argc, &argv, &enpv, _CRT_glob, &si);

		return wmain(argc, argv);
	}
#endif

int wmain(int argc, wchar_t * argv[])
{
	Application::main(argc, argv);
	return 0;
}
