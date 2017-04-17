//---------------------------------------------------------------------------

#pragma once

#ifndef EXCEPTION_H
#define EXCEPTION_H

//---------------------------------------------------------------------------

#include <core/String.h>
#include <exception>
#include <functional>

//---------------------------------------------------------------------------

namespace asd
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

	api(core) void viewException(const exception & ex);

    class Exception : public runtime_error
    {
    public:
		template<class H, class ... T, useif<
			not_based_on<H, exception>::value,
			can_construct<String, H>::value,
			can_construct<String, T>::value...
			>
		>
		Exception(H && head, T &&... tail) : runtime_error(""), data(forward<H>(head))
		{
			data.add(forward<T>(tail)...);
			viewException(*this);
		}

		Exception(const Exception & except) : runtime_error(except), data(except.data) {}
		Exception(Exception && except) : runtime_error(forward<Exception>(except)), data(move(except.data)) {}

		template<class ... A, useif<
			can_construct<String, A>::value...
			>
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

	api(core) String stdError();
	api(core) String stdError(errno_t err);

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