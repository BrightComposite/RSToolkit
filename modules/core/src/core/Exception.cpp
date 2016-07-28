//---------------------------------------------------------------------------

#include <core/Exception.h>

#include <windows.h>
#include <iostream>

//---------------------------------------------------------------------------

#define EXC_BUFFER_SIZE 256

namespace Rapture
{
	void viewException(const exception & ex)
	{
		using namespace std;
		//MessageBoxA(0, ex.what(), "Exception", MB_ICONERROR);
		cout << "EXCEPTION: " << ex.what() << endl;
	}

	String stdError()
	{
		char buffer[EXC_BUFFER_SIZE];
		strerror_s(buffer, EXC_BUFFER_SIZE, errno);

		return buffer;
	}

	String stdError(errno_t err)
	{
		char buffer[EXC_BUFFER_SIZE];
		strerror_s(buffer, EXC_BUFFER_SIZE, err);

		return buffer;
	}
}
