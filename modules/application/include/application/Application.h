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

int api(export) wmain(int argc, wchar_t * argv[]);

namespace Rapture
{
	class Application;
	struct Entrance;

	using ApplicationArguments = data<wchar_t *>;
	using EntranceFunction = int(*)();
	using ExitFunction = void(*)();

	class Application : public Singleton<Application>
	{
		friend int ::wmain(int argc, wchar_t * argv[]);

		friend struct Singleton<Application>;
		friend struct Entrance;

	public:
		static HINSTANCE api(application) getWindowsInstance();

		static const wstring api(application) & getRootPath();
		static const array_list<wstring> api(application) & startupArguments();

		static int api(application) getShowCommand();

	private:
		static void api(application) main(int argc, wchar_t * argv[]);

		Application() {}

		static void load();
		static wstring getExecutionPath(HINSTANCE hInstance);

		HINSTANCE hInstance = nullptr;
		wstring rootPath;

		array_list<wstring> args;
		EntranceFunction entrance = nullptr;
		int showCommand;
	};

	template struct api(application) Singleton<Application>;

	struct api(application) Entrance
	{
		Entrance(EntranceFunction func);

		EntranceFunction func;
	};
}

//---------------------------------------------------------------------------
#endif
