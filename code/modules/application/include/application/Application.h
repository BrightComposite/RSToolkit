//---------------------------------------------------------------------------

#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

//---------------------------------------------------------------------------

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <core/addition/Singleton.h>
#include <container/ArrayList.h>
#include <core/function/Function.h>
#include <core/String.h>

#include "ThreadLoop.h"

//---------------------------------------------------------------------------

api(export) int wmain(int argc, wchar_t * argv[]);

namespace asd
{
	class Application;
	struct Entrance;

	using ApplicationArguments = data<wchar_t *>;
	using EntranceFunction = int(*)();
	using ExitFunction = void(*)();

	template struct api(application) Singleton<Application>;

	class Application : public Singleton<Application>
	{
		friend int ::wmain(int argc, wchar_t * argv[]);

		friend struct Singleton<Application>;
		friend struct Entrance;

	public:
		#ifdef WIN32
		static api(application) HINSTANCE getWindowsInstance();
		#endif
		
		static api(application) const wstring & getRootPath();
		static const array_list<wstring> api(application) & startupArguments();

		static api(application) int getShowCommand();

	private:
		static api(application) void main(int argc, wchar_t * argv[]);

		Application() {}

		void api(application) pause();

		static api(application) void load();
		
#ifdef WIN32
		static api(application) wstring getExecutionPath(HINSTANCE hInstance);

		HINSTANCE hInstance = nullptr;
#endif
		
		wstring rootPath;
		array_list<wstring> args;

		function<int()> entrance = nullptr;
		int showCommand;
	};

	struct Entrance
	{
		Entrance(EntranceFunction func) {
			if(Application::instance().entrance != nullptr)
				throw Exception("Can't set multiple entrances!");

			Application::instance().entrance = func;
		}

		Entrance(ExitFunction func) {
			if(Application::instance().entrance != nullptr)
				throw Exception("Can't set multiple entrances!");

			Application::instance().entrance = [func]() {
				func();

			#ifdef ASD_CONSOLE
				Application::instance().pause();
			#endif
				return 0;
			};
		}
	};
}

//---------------------------------------------------------------------------
#endif
