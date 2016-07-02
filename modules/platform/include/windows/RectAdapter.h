//---------------------------------------------------------------------------

#pragma once

#ifndef RECT_ADAPTER_H
#define RECT_ADAPTER_H

//---------------------------------------------------------------------------

#include <math/Rect.h>
#include <windows.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	struct RectAdapter
	{
		deny_copy(RectAdapter);

		RectAdapter() : rect {0, 0, 0, 0} {}
		RectAdapter(const RECT & r) : rect (r) {}
		RectAdapter(RECT && r) : rect (forward<RECT>(r)) {}

		template<class T>
		RectAdapter(const Rect<T> & in)
		{
			set(in);
		}

		template<class T>
		RECT & set(const Rect<T> & in)
		{
			rect.left = static_cast<long>(in.left);
			rect.top = static_cast<long>(in.top);
			rect.right = static_cast<long>(in.right);
			rect.bottom = static_cast<long>(in.bottom);

			return rect;
		}

		template<class T>
		void assignTo(Rect<T> & in)
		{
			in.left = static_cast<T>(rect.left);
			in.top = static_cast<T>(rect.top);
			in.right = static_cast<T>(rect.right);
			in.bottom = static_cast<T>(rect.bottom);
		}

		long left() const
		{
			return rect.left;
		}

		long top() const
		{
			return rect.top;
		}

		long right() const
		{
			return rect.right;
		}

		long bottom() const
		{
			return rect.bottom;
		}

		long width() const
		{
			return rect.right - rect.left;
		}

		long height() const
		{
			return rect.bottom - rect.top;
		}

		RECT * operator & ()
		{
			return &rect;
		}

		const RECT * operator & () const
		{
			return &rect;
		}

		RECT rect;
	};
}

//---------------------------------------------------------------------------
#endif
