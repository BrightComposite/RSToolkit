//---------------------------------------------------------------------------

#pragma once

#ifndef STARTER_H
#define STARTER_H

//---------------------------------------------------------------------------

#include <application/application.h>

#if defined(__MINGW32__)
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
#elif defined(__GNUC__)

#include <locale>

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args && ...args) : Facet(std::forward<Args>(args)...) {}
	
	~deletable_facet() {}
};

int main(int argc, char * argv[]) {
	using namespace asd;
	
	array_list<wstring> nargv;
	array_list<wchar_t *> cargv;
	
	repeat(i, argc) {
		nargv.push_back(from_utf8(argv[i]));
		cargv.push_back(&nargv.back()[0]);
	}
	
	return wmain(argc, cargv.data());
}

#endif

int wmain(int argc, wchar_t * argv[]) {
	using namespace asd;

#ifdef WIN32
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	
	application::main(argc, argv);
	return 0;
}


//---------------------------------------------------------------------------
#endif
