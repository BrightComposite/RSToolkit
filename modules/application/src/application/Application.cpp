//---------------------------------------------------------------------------

#include <windows.h>
#include <application\Application.h>

#if defined(_MSC_VER) && (defined(_DEBUG) || defined(DEBUG))
//#include <vld.h>
#endif

//---------------------------------------------------------------------------

namespace Rapture
{
	wstring getDir(wstring path)
	{
		return path.substr(0, path.find_last_of(L"/\\"));
	}
}

using namespace Rapture;

int wmain(int argc, wchar_t ** argv)
{
	Application::main(argc, argv);
	return 0;
}