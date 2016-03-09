//---------------------------------------------------------------------------

#ifndef LIST_H
#define LIST_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <list>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::list;

	template<typename T>
	class ListObject : public Object, public list<T>
	{
	public:
		using list<T>::list;
	};

	template<typename T, class ... OwnerAttr>
	using HandleList = list<Handle<T, OwnerAttr...>>;

	template<typename T, class ... OwnerAttr>
	using List = ListObject<Handle<T, OwnerAttr...>>;
}

//---------------------------------------------------------------------------
#endif
