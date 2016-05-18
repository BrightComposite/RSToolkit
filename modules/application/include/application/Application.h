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
		friend struct Singleton<Application>;

		friend class Entrance;
		friend class Exit;

	public:
		static void main(int argc, wchar_t * argv[]);

		static HINSTANCE getWindowsInstance();

		static const wstring & getRootPath();
		static const vector<wstring> & getStartupArguments();
		static int getShowCommand();

	protected:
		Application() {}

		static void load();
		static wstring getExecutionPath(HINSTANCE hInstance);

		HINSTANCE hInstance = nullptr;
		wstring rootPath;
		vector<wstring> startupArguments;
		int showCommand;

		vector<Entrance *> entrances;
	};

	class Entrance
	{
	public:
		Entrance(const EntranceFunction & func, int order = 0) : Entrance(Application::instance(), func, order)
		{
			app.entrances.push_back(this);
			std::sort(app.entrances.begin(), app.entrances.end(), sort);
		}

		Application & app;
		EntranceFunction enter;
		int order;

	protected:
		Entrance(Application & app, const EntranceFunction & func, int order) : app(app), enter(func), order(order) {}

		static bool sort(Entrance * e1, Entrance * e2)
		{
			return e1->order < e2->order;
		}
	};
}

//---------------------------------------------------------------------------

#pragma comment(linker, "/ENTRY:wmainCRTStartup")

//---------------------------------------------------------------------------
#endif
