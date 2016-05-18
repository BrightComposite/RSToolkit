//---------------------------------------------------------------------------

#ifndef SET_H
#define SET_H

//---------------------------------------------------------------------------

#include <core/Handle.h>

#include <set>
#include <unordered_set>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::set;
	using std::unordered_set;

	template<class T, class ... OwnerAttr>
	class Set : public set<Handle<T, OwnerAttr...>>
	{
	public:
		using set<Handle<T, OwnerAttr...>>::set;
	};

	template<class T, class Hash, class ... OwnerAttr>
	class CustomSet : public set<Handle<T, OwnerAttr...>, Hash>
	{
	public:
		using set<Handle<T, OwnerAttr...>, Hash>::set;
	};

	template<typename T>
	class PointerSet : public set<T *>
	{
	public:
		using set<T *>::set;
	};

	template<class T, class ... OwnerAttr>
	class HashSet : public unordered_set<Handle<T, OwnerAttr...>>
	{
	public:
		using unordered_set<Handle<T, OwnerAttr...>>::unordered_set;
	};

	template<class T, class Hash, class ... OwnerAttr>
	class CustomHashSet : public unordered_set<Handle<T, OwnerAttr...>, Hash>
	{
	public:
		using unordered_set<Handle<T, OwnerAttr...>, Hash>::unordered_set;
	};

	template<typename T>
	class PointerHashSet : public unordered_set<T *>
	{
	public:
		using unordered_set<T *>::unordered_set;
	};
}

//---------------------------------------------------------------------------
#endif
