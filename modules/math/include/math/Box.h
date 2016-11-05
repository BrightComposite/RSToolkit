//---------------------------------------------------------------------------

#pragma once

#ifndef BOX_H
#define BOX_H

//---------------------------------------------------------------------------

#include <math/Vector.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T>
	class Box
	{
	public:
		Vector<T> min, max;

		bool contains(const Vector<T> & point)
		{
			return point > min && point < max;
		}
	};
}

//---------------------------------------------------------------------------
#endif
