//---------------------------------------------------------------------------

#ifndef SET_H
#define SET_H

//---------------------------------------------------------------------------

#include <core/Object.h>

#include <core/container/Hash.h>

#include <set>
#include <unordered_set>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::set;
	using std::unordered_set;

	template<class T>
	class SetObject : public Object, public unordered_set<T>
	{
	public:
		using unordered_set<T>::unordered_set;
	};

	template<class T, class ... OwnerAttr>
	using HandleSet = unordered_set<Handle<T, OwnerAttr...>>;

	template<typename T, class ... OwnerAttr>
	using Set = SetObject<Handle<T, OwnerAttr...>>;
}

//---------------------------------------------------------------------------
#endif
