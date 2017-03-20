//---------------------------------------------------------------------------

#pragma once

#ifndef cONTAINER_H
#define cONTAINER_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <boost/iterator/iterator_facade.hpp>

#include <stack>

//---------------------------------------------------------------------------

namespace asd
{
	using std::stack;

	type_checker(is_iterable, iterator);
	type_checker(is_const_iterable, const_iterator);

	template<class container, useif<is_iterable<container>::value>>
	typename container::iterator erase(container & c, int pos)
	{
		return c.erase(std::next(c.begin(), pos));
	}

	template<class container, useif<is_iterable<container>::value>>
	typename container::iterator erase(container & c, int pos, size_t count)
	{
		auto i = std::next(c.begin(), pos);
		return c.erase(i, i + count);
	}

	template<class container, typename T, useif<is_iterable<container>::value>>
	typename container::iterator erase(container & c, const T & value)
	{
		return c.erase(std::remove(c.begin(), c.end(), value), c.end()); //move all values equal to the 'value' to the end of the 'container', then erase the end
	}

	template<class container, useif<is_iterable<container>::value>>
	bool check(typename container::iterator & iterator, const container & c)
	{
		return iterator != c.end();
	}

	template<class container, useif<is_const_iterable<container>::value>>
	bool check(typename container::const_iterator & iterator, const container & c)
	{
		return iterator != c.cend();
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

	static_method_checker(has_child_begin, child_begin);
	static_method_checker(has_child_end, child_end);

	template<class ctx>
	class tree_iterator;

	template<class impl, class cont>
	class tree
	{
	public:
		using container = cont;
		using iterator = tree_iterator<impl>;
		using const_iterator = tree_iterator<const impl>;

		tree(container & c) : _container(c) {}

		iterator begin()
		{
			return {_container.begin(), _container.end()};
		}

		iterator end()
		{
			return {_container.end()};
		}

		const_iterator cbegin()
		{
			return {_container.cbegin(), _container.cend()};
		}

		const_iterator cend()
		{
			return {_container.cend()};
		}

	private:
		container & _container;
	};

	template<class ctx>
	class tree_iterator : public boost::iterator_facade<tree_iterator<ctx>, typename ctx::container::value_type, std::forward_iterator_tag, typename ctx::container::iterator::reference>
	{
		friend class boost::iterator_core_access;

		using container = typename ctx::container;
		using underlying = conditional_t<is_const<ctx>::value, typename container::iterator, typename container::const_iterator>;

	public:
		tree_iterator(const underlying & end) : _node(end), _terminator(end) {}
		tree_iterator(const underlying & begin, const underlying & end) : _node(begin), _terminator(end) {}

	private:
		typename underlying::reference dereference() const
		{
			return *_node;
		}

		bool equal(const tree_iterator & other) const
		{
			return _node == other._node;
		}

		void increment()
		{
			if(_node == _terminator)
				return;

			auto begin = ctx::child_begin(_node);
			auto end = ctx::child_end(_node);

			if(begin == end)
			{
				_branch.push(_node);
				_terminators.push(_terminator);

				_node = begin;
				_terminator = end;
				return;
			}

			++_node;

			while(_node == _terminator)
			{
				if(_branch.size() == 0)
					return;

				_node = _branch.top();
				_terminator = _terminators.top();
				_branch.pop();
				_terminators.pop();
			}
		}

		underlying _node;
		underlying _terminator;

		stack<underlying> _branch;
		stack<underlying> _terminators;
	};
}

//---------------------------------------------------------------------------
#endif
