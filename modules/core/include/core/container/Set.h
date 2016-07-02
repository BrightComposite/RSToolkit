//---------------------------------------------------------------------------

#pragma once

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
	using std::multiset;
	using std::unordered_multiset;

	template<class T, class ... Owner>
	class Set : public set<Handle<T, Owner...>>
	{
	public:
		using set<Handle<T, Owner...>>::set;
	};

	template<class T, class Hash, class ... Owner>
	class CustomSet : public set<Handle<T, Owner...>, Hash>
	{
	public:
		using set<Handle<T, Owner...>, Hash>::set;
	};

	template<typename T>
	class PointerSet : public set<T *>
	{
	public:
		using set<T *>::set;
	};

	template<class T, class ... Owner>
	class HashSet : public unordered_set<Handle<T, Owner...>>
	{
	public:
		using unordered_set<Handle<T, Owner...>>::unordered_set;
	};

	template<class T, class Hash, class ... Owner>
	class CustomHashSet : public unordered_set<Handle<T, Owner...>, Hash>
	{
	public:
		using unordered_set<Handle<T, Owner...>, Hash>::unordered_set;
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
