//---------------------------------------------------------------------------

#pragma once

#ifndef POINT_ADAPTER_H
#define POINT_ADAPTER_H

//---------------------------------------------------------------------------

#include <math/point.h>
//#include <windows.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct PointAdapter
	{
		deny_copy(PointAdapter);

		PointAdapter() : point {0, 0} {}
		PointAdapter(const POINT & r) : point (r) {}
		PointAdapter(POINT && r) : point (forward<POINT>(r)) {}

		template<class T>
		PointAdapter(const Point<T> & in)
		{
			set(in);
		}

		template<class T>
		POINT & set(const Point<T> & in)
		{
			point.x = static_cast<long>(in.x);
			point.y = static_cast<long>(in.y);

			return point;
		}

		template<class T>
		void assignTo(Point<T> & in)
		{
			in.x = static_cast<T>(point.x);
			in.y = static_cast<T>(point.y);
		}

		long x() const
		{
			return point.x;
		}

		long y() const
		{
			return point.y;
		}

		POINT * operator & ()
		{
			return &point;
		}

		const POINT * operator & () const
		{
			return &point;
		}

		POINT point;
	};
}

//---------------------------------------------------------------------------
#endif

