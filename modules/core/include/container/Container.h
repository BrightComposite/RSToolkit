//---------------------------------------------------------------------------

#pragma once

#ifndef CONTAINER_H
#define CONTAINER_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <boost/iterator/iterator_facade.hpp>

//---------------------------------------------------------------------------

namespace asd
{
	type_checker(is_iterable, iterator);
	type_checker(is_const_iterable, const_iterator);

	template<class Container, useif<is_iterable<Container>::value>>
	typename Container::iterator erase(Container & container, int pos)
	{
		return container.erase(std::next(container.begin(), pos));
	}

	template<class Container, useif<is_iterable<Container>::value>>
	typename Container::iterator erase(Container & container, int pos, size_t count)
	{
		auto i = std::next(container.begin(), pos);
		return container.erase(i, i + count);
	}

	template<class Container, typename T, useif<is_iterable<Container>::value>>
	typename Container::iterator erase(Container & container, const T & value)
	{
		return container.erase(std::remove(container.begin(), container.end(), value), container.end()); //move all values equal to the 'value' to the end of the 'container', then erase the end
	}

	template<class Container, useif<is_iterable<Container>::value>>
	bool check(typename Container::iterator & iterator, const Container & container)
	{
		return iterator != container.end();
	}

	template<class Container, useif<is_const_iterable<Container>::value>>
	bool check(typename Container::const_iterator & iterator, const Container & container)
	{
		return iterator != container.cend();
	}

	template<class T>
	struct reversed
	{
		reversed(T & cont) : _cont(cont) {}

		auto begin()
		{
			return _cont.rbegin();
		}

		auto end()
		{
			return _cont.rend();
		}

		auto cbegin()
		{
			return _cont.crbegin();
		}

		auto cend()
		{
			return _cont.crend();
		}

		auto rbegin()
		{
			return _cont.begin();
		}

		auto rend()
		{
			return _cont.end();
		}

		auto crbegin()
		{
			return _cont.cbegin();
		}

		auto crend()
		{
			return _cont.cend();
		}

	protected:
		T & _cont;
	};

	template<class T>
	reversed<T> reverse(T & cont)
	{
		return {cont};
	}

	template<class T, class A, A T::*member>
	struct MemberSort
	{
		bool operator() (const T & a, const T & b)
		{
			return a.*member < b.*member;
		}

		bool operator() (const T * a, const T * b)
		{
			return a->*member < b->*member;
		}
	};

	template<class Cont>
	class deep_iterator {};

	template<template <class> class Cont, class Elem>
	class deep_iterator<Cont<Elem>> : public boost::iterator_facade<deep_iterator<Cont<Elem>>, Elem, std::forward_iterator_tag>
	{
		friend class boost::iterator_core_access;

		using Container = Cont<Elem>;
		using underlying = typename Container::iterator;

	public:

	private:
		Elem & dereference() const
		{
			return *it;
		}

		bool equal(const iter & other) const
		{
			return end && other.end;
		}

		void increment()
		{
			///TODO advance(1);
		}

		underlying it;
		bool end = false;
	};
}

//---------------------------------------------------------------------------
#endif
