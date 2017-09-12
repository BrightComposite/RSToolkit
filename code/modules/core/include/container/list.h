//---------------------------------------------------------------------------

#pragma once

#ifndef LIST_H
#define LIST_H

//---------------------------------------------------------------------------

#include <core/handle.h>

#include <list>

//---------------------------------------------------------------------------

namespace asd
{
	using std::list;

	template<typename T>
	void sort(list<T> & l)
	{
		l.sort();
	}

	template<typename Pred, typename T, useif<is_callable<Pred, const T &, const T &>::value>>
	void sort(list<T> & l)
	{
		l.sort(Pred());
	}

}

//---------------------------------------------------------------------------
#endif
