//---------------------------------------------------------------------------

#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

//---------------------------------------------------------------------------

#include <windows.h>

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
		static api(application) HINSTANCE getWindowsInstance();

		static api(application) const wstring & getRootPath();
		static const array_list<wstring> api(application) & startupArguments();

		static api(application) int getShowCommand();

	private:
		static api(application) void main(int argc, wchar_t * argv[]);

		Application() {}

		static void load();
		static wstring getExecutionPath(HINSTANCE hInstance);

		HINSTANCE hInstance = nullptr;
		wstring rootPath;
		array_list<wstring> args;

		EntranceFunction entrance = nullptr;
		int showCommand;
	};

	struct api(application) Entrance
	{
		Entrance(EntranceFunction func);

		EntranceFunction func;
	};
}

//---------------------------------------------------------------------------
#endif
