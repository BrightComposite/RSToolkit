//---------------------------------------------------------------------------

#ifndef ARRAY_H
#define ARRAY_H

//---------------------------------------------------------------------------

#include <core/Object.h>

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
		using Base::erase;

		typename Base::iterator erase(int pos)
		{
			return erase(begin() + pos);
		}
	};

	template<typename T>
	class ArrayObject : public Object, public CustomArray<T> {};

	template<typename T>
	using PointerArray = CustomArray<T *>;

	template<typename T, class ... OwnerAttr>
	using HandleArray = CustomArray<Handle<T, OwnerAttr...>>;

	template<class T, class ... OwnerAttr>
	using Array = ArrayObject<Handle<T, OwnerAttr...>>;
}

//---------------------------------------------------------------------------
#endif
