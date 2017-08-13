//---------------------------------------------------------------------------

#pragma once

#ifndef STACK_H
#define STACK_H

//---------------------------------------------------------------------------

#include <core/handle.h>
#include <container/Container.h>

#include <stack>

//---------------------------------------------------------------------------

namespace asd
{
	using std::stack;

	template<typename T>
	class Stack : public stack<handle<T>>
	{
	public:
		using stack<handle<T>>::stack;
	};
}

//---------------------------------------------------------------------------
#endif