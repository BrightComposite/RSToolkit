//---------------------------------------------------------------------------

#include <core/Exception.h>

#include <windows.h>

//---------------------------------------------------------------------------

#define EXC_BUFFER_SIZE 256

namespace Rapture
{
	void viewException(const exception & ex)
	{
		//MessageBoxA(0, ex.what(), "Exception", MB_ICONERROR);
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
