//---------------------------------------------------------------------------

#pragma once

#ifndef STARTER_H
#define STARTER_H

//---------------------------------------------------------------------------

#include <application/Application.h>

#ifdef __MINGW32__
#ifdef __cplusplus
#define C_EXTERN "C"
#else
#define C_EXTERN
#endif

extern int _CRT_glob;
extern C_EXTERN void __wgetmainargs(int *, wchar_t ***, wchar_t ***, int, int *);

int main()
{
	wchar_t ** enpv, ** argv;
	int argc, si = 0;
	__wgetmainargs(&argc, &argv, &enpv, _CRT_glob, &si);

	return wmain(argc, argv);
}
#endif

int wmain(int argc, wchar_t * argv[])
{
	using namespace Rapture;

#ifdef WIN32
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	Application::main(argc, argv);
	return 0;
}


//---------------------------------------------------------------------------
#endif
