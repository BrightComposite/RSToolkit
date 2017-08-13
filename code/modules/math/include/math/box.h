//---------------------------------------------------------------------------

#pragma once

#ifndef BOX_H
#define BOX_H

//---------------------------------------------------------------------------

#include <math/vector.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<class T>
		class box
		{
		public:
			vector<T> min, max;
			
			bool contains(const vector<T> & point) {
				return point > min && point < max;
			}
		};
	}
}

//---------------------------------------------------------------------------
#endif
