//---------------------------------------------------------------------------

#ifndef SET_H
#define SET_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/container/Hash.h>

#include <set>
#include <unordered_set>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::set;
	using std::unordered_set;

	template<class T, class ... OwnerAttr>
	class Set : public unordered_set<Handle<T, OwnerAttr...>>
	{
	public:
		using unordered_set<Handle<T, OwnerAttr...>>::unordered_set;
	};

	template<class T, class ... OwnerAttr>
	class OrderedSet : public set<Handle<T, OwnerAttr...>>
	{
	public:
		using set<Handle<T, OwnerAttr...>>::set;
	};

	template<typename T>
	class PointerSet : public unordered_set<T *>
	{
	public:
		using unordered_set<T *>::unordered_set;
	};

	template<typename T>
	class PointerOrderedSet : public set<T *>
	{
	public:
		using set<T *>::set;
	};
}

//---------------------------------------------------------------------------
#endif
