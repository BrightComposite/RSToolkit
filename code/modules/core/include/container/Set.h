//---------------------------------------------------------------------------

#pragma once

#ifndef SET_H
#define SET_H

//---------------------------------------------------------------------------

#include <core/handle.h>

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

	template<class T, class Pred = less<handle<T>>, class Alloc = std::allocator<handle<T>>>
	class Set : public set<handle<T>, Pred, Alloc>
	{
	public:
		using set<handle<T>, Pred, Alloc>::set;
	};

	template<class T, class Hasher = hash<handle<T>>, class Eq = equal_to<handle<T>>, class Alloc = std::allocator<handle<T>>>
	class HashSet : public unordered_set<handle<T>, Hasher, Eq, Alloc>
	{
	public:
		using unordered_set<handle<T>, Hasher, Eq, Alloc>::unordered_set;
	};
}

//---------------------------------------------------------------------------
#endif
