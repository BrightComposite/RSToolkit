//---------------------------------------------------------------------------

#ifndef LIST_H
#define LIST_H

//---------------------------------------------------------------------------

#include <core/Handle.h>

#include <list>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::list;

	template<typename T, class ... Owner>
	class List : public list<Handle<T, Owner...>>
	{
	public:
		using list<Handle<T, Owner...>>::list;
	};

	template<typename T>
	class PointerList : public list<T *>
	{
	public:
		using list<T *>::list;
	};

	template<typename T>
	void sort(list<T> & l)
	{
		l.sort();
	}

	template<typename Pred, typename T, useif <
		is_callable<Pred, const T &, const T &>::value
		> endif
	>
	void sort(list<T> & l)
	{
		l.sort(Pred());
	}

	template<typename T, typename Pred, useif <
		is_callable<Pred, const T &, const T &>::value
		> endif
	>
	void sort(list<T> & l, Pred pred)
	{
		l.sort(pred);
	}
}

//---------------------------------------------------------------------------
#endif
