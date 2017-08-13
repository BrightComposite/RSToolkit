//---------------------------------------------------------------------------

#include <application/application.h>

#include <iostream>

#if defined(_MSC_VER) && (defined(_DEBUG) || defined(DEBUG))
//#include <vld.h>
#endif

//---------------------------------------------------------------------------

namespace asd
{
	wstring getDir(const wstring & path);

#ifdef WIN32
	void application::main(int argc, wchar_t * argv[]) {
		auto & inst = instance();

		if(inst.hInstance != nullptr)
			throw Exception("application has been already loaded!");

		inst.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
		
		for(int i = 0; i < argc; ++i)
			inst.args.push_back(argv[i]);

		STARTUPINFOW info;
		GetStartupInfoW(&info);

		inst.showCommand = check_flag(STARTF_USESHOWWINDOW, info.dwFlags) ? info.wShowWindow : SW_NORMAL;
		inst.rootPath = getDir(getDir(application::getExecutionPath(inst.hInstance)));
		load();
	}

	HINSTANCE application::getWindowsInstance() {
		auto & inst = instance();
		return inst.hInstance;
	}
#else
	void application::main(int argc, wchar_t * argv[]) {
		auto & inst = instance();
		
		for(int i = 0; i < argc; ++i)
			inst.args.push_back(wstring(argv[i]));
		
		inst.rootPath = getDir(getDir(argv[0]));
		load();
	}
#endif
	const wstring & application::getRootPath() {
		auto & inst = instance();
		return inst.rootPath;
	}

	const array_list<wstring> & application::startupArguments() {
		auto & inst = instance();
		return inst.args;
	}

	int application::getShowCommand() {
		auto & inst = instance();
		return inst.showCommand;
	}

	void application::load() {
		instance().entrance();
	}

#ifdef WIN32
	wstring application::getExecutionPath(HINSTANCE hInstance) {
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(hInstance, buffer, MAX_PATH);

		return {buffer, wcslen(buffer)};
	}
#endif
	
	void application::pause() {
		std::cout << std::endl;
		std::cout << "Press Enter to exit...";
		getchar();
	}

	wstring getDir(const wstring & path) {
		return path.substr(0, path.find_last_of(L"/\\"));
	}
}
