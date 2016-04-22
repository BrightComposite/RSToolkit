//---------------------------------------------------------------------------

#ifndef EXCEPTION_H
#define EXCEPTION_H

//---------------------------------------------------------------------------

#include <core/String.h>
#include <exception>
#include <functional>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::exception;
	using std::runtime_error;
	using std::function;

	class Interruption
	{
	public:
		Interruption() {}
		virtual ~Interruption() {}
	};

	void viewException(const exception & ex);

    class Exception : public runtime_error
    {
    public:
		template<class H, class ... T,
			useif(
				can_string_assemble(H, T...)
				)>
		Exception(H && head, T &&... tail) : runtime_error(""), data()
		{
			data.add(head, tail...);
			viewException(*this);
		}

		Exception(const Exception & except) : runtime_error(except), data(except.data) {}
		Exception(Exception && except) : runtime_error(forward<Exception>(except)), data(move(except.data)) {}

		Exception(const exception & except) : runtime_error(""), data(except.what()) {}

		virtual ~Exception() {}

		virtual const char * what() const override
		{
			return data.c_str();
		}

	protected:
		String data;
    };

	String stdError();
	String stdError(errno_t err);

	class ErrnoException : public Exception
	{
	public:
		ErrnoException() : Exception(stdError()) {}
		template<class H, class ... T>
		ErrnoException(H && head, T &&... tail) : Exception(stdError(), " | ", forward<H>(head), forward<T>(tail)...) {}
		ErrnoException(const ErrnoException & except) : Exception(static_cast<const Exception &>(except)) {}
		ErrnoException(ErrnoException && except) : Exception(forward<Exception>(except)) {}

		virtual ~ErrnoException() {}
	};
}

//---------------------------------------------------------------------------
#endif
