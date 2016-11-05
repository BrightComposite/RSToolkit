//---------------------------------------------------------------------------

#include <windows.h>
#include <application\Application.h>

#if defined(_MSC_VER) && (defined(_DEBUG) || defined(DEBUG))
//#include <vld.h>
#endif

//---------------------------------------------------------------------------

namespace asd
{
	wstring getDir(const wstring & path);

	static array_list<Entrance *> & entrances()
	{
		static array_list<Entrance *> e;
		return e;
	}

	void Application::main(int argc, wchar_t * argv[])
	{
		auto & inst = instance();

		if(inst.hInstance != nullptr)
			throw Exception("Application has been already loaded!");

		inst.hInstance = (HINSTANCE)GetModuleHandle(nullptr);

		for(int i = 0; i < argc; ++i)
			inst.args.push_back(argv[i]);

		STARTUPINFOW info;
		GetStartupInfoW(&info);

		inst.showCommand = check_flag(STARTF_USESHOWWINDOW, info.dwFlags) ? info.wShowWindow : SW_NORMAL;

		wstring root = getDir(getDir(Application::getExecutionPath(inst.hInstance)));

	#ifdef _WIN64
		root = getDir(root);
	#endif

		inst.rootPath = {root.c_str(), root.size()};
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

	const array_list<wstring> & Application::startupArguments()
	{
		auto & inst = instance();
		return inst.args;
	}

	int Application::getShowCommand()
	{
		auto & inst = instance();
		return inst.showCommand;
	}

	void Application::load()
	{
		instance().entrance();
	}

	wstring Application::getExecutionPath(HINSTANCE hInstance)
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(hInstance, buffer, MAX_PATH);

		return {buffer, wcslen(buffer)};
	}

	Entrance::Entrance(EntranceFunction func)
	{
		if(Application::instance().entrance != nullptr)
			throw Exception("Can't set multiple entrances!");

		Application::instance().entrance = func;
	}

	wstring getDir(const wstring & path)
	{
		return path.substr(0, path.find_last_of(L"/\\"));
	}
}
