//---------------------------------------------------------------------------

#ifndef STACK_H
#define STACK_H

//---------------------------------------------------------------------------

#include <core/Object.h>

#include <stack>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::stack;

	template<typename T>
	class StackObject : public Object, public stack<T> {};

	template<typename T, class ... OwnerAttr>
	using HandleStack = stack<Handle<T, OwnerAttr...>>;

	template<class T, class ... OwnerAttr>
	using Stack = StackObject<Handle<T, OwnerAttr...>>;
}

//---------------------------------------------------------------------------
#endif