//---------------------------------------------------------------------------

#pragma once

#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

//---------------------------------------------------------------------------

#include <core/Handle.h>

#include <array>
#include <vector>

//---------------------------------------------------------------------------

namespace std
{
	template<class T, class A = allocator<T>>
	struct array_list : vector<T, A>
	{
	public:
		using vector<T, A>::vector;
	};
}

namespace Rapture
{
	using std::array_list;

	template<typename T, class ... Owner>
	class ArrayList : public array_list<Handle<T, Owner...>>
	{
	public:
		using array_list<Handle<T, Owner...>>::array_list;
	};

	template<typename T>
	class AlignedArrayList : public array_list<T, aligned_allocator<T, alignof(T)>>
	{
	public:
		using array_list<T, aligned_allocator<T, alignof(T)>>::array_list;
	};

	template<typename T>
	void sort(array_list<T> & v)
	{
		std::sort(v.begin(), v.end());
	}

	template<typename Pred, typename T, useif<
		is_callable<Pred, const T &, const T &>::value
		>
	>
	void sort(array_list<T> & v)
	{
		std::sort(v.begin(), v.end(), Pred());
	}

	template<typename T, typename Pred, useif<
		is_callable<Pred, const T &, const T &>::value
		>
	>
	void sort(array_list<T> & v, Pred pred)
	{
		std::sort(v.begin(), v.end(), pred);
	}
}

//---------------------------------------------------------------------------
#endif
