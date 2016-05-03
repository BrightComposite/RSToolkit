//---------------------------------------------------------------------------

#ifndef ARRAY_H
#define ARRAY_H

//---------------------------------------------------------------------------

#include <core/Handle.h>

#include <array>
#include <vector>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::vector;
	using std::array;

	template<typename T>
	class CustomArray : public vector<T>
	{
		typedef vector<T> Base;

	public:
		using Base::vector;
		using Base::erase;

		typename Base::iterator erase(int pos)
		{
			return erase(begin() + pos);
		}
	};

	template<typename T, class ... OwnerAttr>
	class Array : public CustomArray<Handle<T, OwnerAttr...>>
	{
	public:
		using CustomArray<Handle<T, OwnerAttr...>>::CustomArray;
	};
}

//---------------------------------------------------------------------------
#endif
