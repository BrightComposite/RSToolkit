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
	int application::main(int argc, wchar_t * argv[]) {
		auto & inst = instance();

		if(inst.hInstance != nullptr)
			throw Exception("application has been already loaded!");

		inst.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
		
		for(int i = 0; i < argc; ++i)
			inst.args.push_back(argv[i]);

		STARTUPINFOW info;
		GetStartupInfoW(&info);

		inst._show_command = check_flag(STARTF_USESHOWWINDOW, info.dwFlags) ? info.wShowWindow : SW_NORMAL;
		inst._root_path = getDir(getDir(application::getExecutionPath(inst.hInstance)));
		return load();
	}

	HINSTANCE application::getWindowsInstance() {
		auto & inst = instance();
		return inst.hInstance;
	}
#else
	int application::main(int argc, wchar_t ** argv) {
		auto & inst = instance();
		
		for(int i = 0; i < argc; ++i)
			inst.args.push_back(wstring(argv[i]));
		
		inst._root_path = getDir(getDir(argv[0]));
		return load();
	}
#endif
	const wstring & application::root_path() {
		auto & inst = instance();
		return inst._root_path;
	}

	const array_list<wstring> & application::arguments() {
		auto & inst = instance();
		return inst.args;
	}

	int application::show_command() {
		auto & inst = instance();
		return inst._show_command;
	}

	int application::load() {
		return instance().entrance();
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
