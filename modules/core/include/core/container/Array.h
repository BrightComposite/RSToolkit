//---------------------------------------------------------------------------

#ifndef ARRAY_H
#define ARRAY_H

//---------------------------------------------------------------------------

#include <core/Handle.h>

#include <vector>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::vector;

	template<typename T, class ... OwnerAttr>
	class Array : public vector<Handle<T, OwnerAttr...>>
	{
	public:
		using vector<Handle<T, OwnerAttr...>>::vector;
	};

	template<typename T>
	void sort(vector<T> & v)
	{
		std::sort(v.begin(), v.end());
	}

	template<typename Pred, typename T, useif <
		is_callable<Pred, const T &, const T &>::value
		> endif
	>
	void sort(vector<T> & v)
	{
		std::sort(v.begin(), v.end(), Pred());
	}

	template<typename T, typename Pred, useif <
		is_callable<Pred, const T &, const T &>::value
		> endif
	>
	void sort(vector<T> & v, Pred pred)
	{
		std::sort(v.begin(), v.end(), pred);
	}
}

//---------------------------------------------------------------------------
#endif
