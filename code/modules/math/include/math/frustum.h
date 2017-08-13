//---------------------------------------------------------------------------

#pragma once

#ifndef FRUSTUM_H
#define FRUSTUM_H

//---------------------------------------------------------------------------

#include "plane.h"

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<class T>
		class frustum
		{
		public:
			frustum(const matrix<T> & vp) {
				auto m = vp.transposition();
				auto & p = *_planes;
				
				for(int i = 0; i < 3; ++i) {
					(p[i * 2 + 0] = m[i] + m[3]).normalize();
					(p[i * 2 + 1] = m[i] - m[3]).normalize();
				}
			}
		
		protected:
			aligned_array<plane<T>, 6> _planes[6]; // left, right, bottom, top, near, far
		};
	}
}

//---------------------------------------------------------------------------
#endif
