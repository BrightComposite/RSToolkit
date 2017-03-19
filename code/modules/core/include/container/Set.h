//---------------------------------------------------------------------------

#pragma once

#ifndef SET_H
#define SET_H

//---------------------------------------------------------------------------

#include <core/Handle.h>

#include <set>
#include <unordered_set>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T, class Pred = less<T>, class Alloc = std::allocator<T>>
	using set = std::set<T, Pred, Alloc>;

	template<class T, class Hasher = hash<T>, class Eq = equal_to<T>, class Alloc = std::allocator<T>>
	using unordered_set = std::unordered_set<T, Hasher, Eq, Alloc>;

	template<class T, class Pred = less<T>, class Alloc = std::allocator<T>>
	using multiset = std::multiset<T, Pred, Alloc>;

	template<class T, class Hasher = hash<T>, class Eq = equal_to<T>, class Alloc = std::allocator<T>>
	using unordered_multiset = std::unordered_multiset<T, Hasher, Eq, Alloc>;

	template<class T, class Pred = less<Handle<T>>, class Alloc = std::allocator<Handle<T>>>
	class Set : public set<Handle<T>, Pred, Alloc>
	{
	public:
		using set<Handle<T>, Pred, Alloc>::set;
	};

	template<class T, class Owner, class Pred = less<Handle<T, Owner>>, class Alloc = std::allocator<Handle<T, Owner>>>
	class OwnedSet : public set<Handle<T, Owner>, Pred, Alloc>
	{
	public:
		using set<Handle<T, Owner>, Pred, Alloc>::set;
	};

	template<class T, class Hasher = hash<Handle<T>>, class Eq = equal_to<Handle<T>>, class Alloc = std::allocator<Handle<T>>>
	class HashSet : public unordered_set<Handle<T>, Hasher, Eq, Alloc>
	{
	public:
		using unordered_set<Handle<T>, Hasher, Eq, Alloc>::unordered_set;
	};

	template<class T, class Owner, class Hasher = hash<Handle<T, Owner>>, class Eq = equal_to<Handle<T, Owner>>, class Alloc = std::allocator<Handle<T, Owner>>>
	class OwnedHashSet : public unordered_set<Handle<T, Owner>, Hasher, Eq, Alloc>
	{
	public:
		using unordered_set<Handle<T, Owner>, Hasher, Eq, Alloc>::unordered_set;
	};
}

//---------------------------------------------------------------------------
#endif
