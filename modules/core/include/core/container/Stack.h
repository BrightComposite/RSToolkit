//---------------------------------------------------------------------------

#ifndef STACK_H
#define STACK_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/container/Container.h>

#include <stack>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::stack;

	template<typename T, class ... OwnerAttr>
	class Stack : public stack<Handle<T, OwnerAttr...>>
	{
	public:
		using stack<Handle<T, OwnerAttr...>>::stack;
	};
}

//---------------------------------------------------------------------------
#endif