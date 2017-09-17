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
	
#define exception_class(class_name) 																		\
	class class_name : public Exception 																	\
	{ 																										\
	public: 																								\
		template<class H, class ... T> 																		\
		class_name(H && head, T &&... tail) : Exception(std::forward<H>(head), std::forward<T>(tail)...) {} \
		class_name(const class_name & except) : Exception(static_cast<const Exception &>(except)) {} 		\
		class_name(class_name && except) : Exception(std::forward<Exception>(except)) {} 					\
		 																									\
		virtual ~class_name() {} 																			\
	};

#define exception_subclass(name, base) 																	\
	class name : public base 																			\
	{ 																									\
	public: 																							\
		template<class H, class ... T> 																	\
		name(H && head, T &&... tail) : base(std::forward<H>(head), std::forward<T>(tail)...) {} 		\
		name(const name & except) : base(static_cast<const base &>(except)) {} 							\
		name(name && except) : base(std::forward<base>(except)) {} 										\
		 																								\
		virtual ~name() {} 																				\
	};

}

//---------------------------------------------------------------------------
#endif
