//---------------------------------------------------------------------------

#ifndef APPLICATION_H
#define APPLICATION_H

//---------------------------------------------------------------------------

#include <windows.h>

#include <core/addition/Singleton.h>
#include <core/container/Array.h>
#include <core/action/Action.h>
#include <core/String.h>

#include "ThreadLoop.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	typedef function<int()> EntranceFunction;
	typedef function<void()> ExitFunction;

	wstring getDir(wstring path);

	class Application : public Singleton<Application>
	{
		friend Singleton<Application>;

		friend class ApplicationScope;
		friend class Entrance;
		friend class Exit;

	public:
		static void main(int argc, wchar_t * argv[])
		{
			auto & inst = instance();

			if(inst.hInstance != nullptr)
				throw std::exception("Application has been already loaded!");

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

		static HINSTANCE getWindowsInstance()
		{
			auto & inst = instance();
			return inst.hInstance;
		}

		static const wstring & getRootPath()
		{
			auto & inst = instance();
			return inst.rootPath;
		}

		static const vector<wstring> & getStartupArguments()
		{
			auto & inst = instance();
			return inst.startupArguments;
		}

		static int getShowCommand()
		{
			auto & inst = instance();
			return inst.showCommand;
		}

	protected:
		Application() {}
		virtual ~Application() {}

		static void load()
		{
			auto & inst = instance();

			for(auto & ent : inst.entrances)
				ent();
		}

		static wstring getExecutionPath(HINSTANCE hInstance)
		{
			wchar_t buffer[MAX_PATH];
			GetModuleFileNameW(hInstance, buffer, MAX_PATH);

			return {buffer, wcslen(buffer)};
		}

		HINSTANCE hInstance = nullptr;
		wstring rootPath;
		vector<wstring> startupArguments;
		int showCommand;

		vector<EntranceFunction> entrances;
	};

	class Entrance
	{
	public:
		Entrance(const EntranceFunction & func)
		{
			auto & inst = Application::instance();
			inst.entrances.push_back(func);
		}
	};
}

//---------------------------------------------------------------------------

#pragma comment(linker, "/ENTRY:wmainCRTStartup")

//---------------------------------------------------------------------------
#endif