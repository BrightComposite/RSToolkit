//---------------------------------------------------------------------------

#ifndef COM_EXCEPTION_H
#define COM_EXCEPTION_H

//---------------------------------------------------------------------------

#include <comdef.h>
#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class ComException : public Exception
	{
		static String getError(HRESULT hr)
		{
			_com_error err(hr);
			return err.ErrorMessage();
		}

	public:
		ComException(HRESULT hr) : Exception(getError(hr)) {}
		template<class H, class ... T>
		ComException(HRESULT hr, H && head, T &&... tail) : Exception(getError(hr), " | ", forward<H>(head), forward<T>(tail)...) {}
		ComException(const ComException & except) : Exception(static_cast<const Exception &>(except)) {}
		ComException(ComException && except) : Exception(forward<Exception>(except)) {}

		virtual ~ComException() {}
	};

	template<typename ... A>
	inline void com_assert(HRESULT hr, A &&... args)
	{
		if(FAILED(hr))
			throw ComException(hr, forward<A>(args)...);
	}
}

//---------------------------------------------------------------------------
#endif