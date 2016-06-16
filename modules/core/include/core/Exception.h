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

	void api(core) viewException(const exception & ex);

    class Exception : public runtime_error
    {
    public:
		template<class H, class ... T, useif <
			not_based_on<H, exception>::value,
			can_construct<String, H>::value,
			can_construct<String, T>::value...
			> endif
		>
		Exception(H && head, T &&... tail) : runtime_error(""), data(forward<H>(head))
		{
			data.add(forward<T>(tail)...);
			viewException(*this);
		}

		Exception(const Exception & except) : runtime_error(except), data(except.data) {}
		Exception(Exception && except) : runtime_error(forward<Exception>(except)), data(move(except.data)) {}

		template<class ... A, useif <
			can_construct<String, A>::value...
			> endif
		 >
		Exception(const exception & except, A &&... args) : runtime_error(""), data(except.what())
		{
			data.add(forward<A>(args)...);
		}

		virtual ~Exception() {}

		virtual const char * what() const noexcept override
		{
			return data.c_str();
		}

	protected:
		String data;
    };

	String api(core) stdError();
	String api(core) stdError(errno_t err);

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
