//---------------------------------------------------------------------------

#pragma once

#ifndef RECT_H
#define RECT_H

//---------------------------------------------------------------------------

#include <meta/Inheritance.h>

#include "Point.h"
#include "Range.h"

//---------------------------------------------------------------------------

namespace asd
{
	template <typename T>
	subclass(Size, Point<T>);

    template <typename T>
    class Rect
    {
    public:
		union
		{
			struct
			{
				T left, top, right, bottom;
			};

			T data[4];
		};

        Rect() :
			left(static_cast<T>(0)), top(static_cast<T>(0)), right(static_cast<T>(0)), bottom(static_cast<T>(0)) {}

		Rect(const Rect & r) :
			left(r.left), top(r.top), right(r.right), bottom(r.bottom) {}

		template<typename Tl, typename Tt, typename Tr, typename Tb, useif<
			std::is_pod<Tl>::value && std::is_pod<Tt>::value && std::is_pod<Tr>::value && std::is_pod<Tb>::value
			>
		>
        Rect(Tl left, Tt top, Tr right, Tb bottom) :
			left(static_cast<T>(left)), top(static_cast<T>(top)), right(static_cast<T>(right)), bottom(static_cast<T>(bottom)) {}

		template<typename Tw, typename Th, useif<std::is_pod<Tw>::value && std::is_pod<Th>::value>>
		Rect(Tw w, Th h) :
			left(static_cast<T>(0)), top(static_cast<T>(0)), right(static_cast<T>(w)), bottom(static_cast<T>(h)) {}

		template<typename U, useif<not_same_type<T, U>::value>>
		explicit Rect(const Rect<U> & r) :
			left(static_cast<T>(r.left)), top(static_cast<T>(r.top)), right(static_cast<T>(r.right)), bottom(static_cast<T>(r.bottom)) {}

		template<typename U, typename V>
		Rect(const Point<U> & min, const Point<V> & max) :
			left(static_cast<T>(min.x)), top(static_cast<T>(min.y)), right(static_cast<T>(max.x)), bottom(static_cast<T>(max.y)) {}

		template<typename U, typename V>
		Rect(const Range<U> & h, const Range<V> & v) :
			left(static_cast<T>(h.min)), top(static_cast<T>(v.min)), right(static_cast<T>(h.max)), bottom(static_cast<T>(v.max)) {}

		template<typename U, typename V>
		Rect(const Point<U> & pos, const Size<V> & sz) :
			left(static_cast<T>(pos.x)), top(static_cast<T>(pos.y)), right(static_cast<T>(pos.x + sz.x)), bottom(static_cast<T>(pos.y + sz.y)) {}

		template<typename U>
		Rect(const Size<U> & sz) :
			left(static_cast<T>(0)), top(static_cast<T>(0)), right(static_cast<T>(sz.x)), bottom(static_cast<T>(sz.y)) {}

		template<typename U, typename V, useif<std::is_pod<U>::value && std::is_pod<V>::value>>
		Rect(const initializer_list<U> & pt, const initializer_list<V> & sz) :
			left(static_cast<T>(pt.begin()[0])),
			top(static_cast<T>(pt.begin()[1])),
			right(static_cast<T>(pt.begin()[0] + sz.begin()[0])),
			bottom(static_cast<T>(pt.begin()[1] + sz.begin()[1])) {}

        bool isEmpty()
        {
            return width() <= 0 || height() <= 0;
        }

        T width() const
        {
            return right - left;
        }

        T height() const
        {
            return bottom - top;
        }

        T area() const
        {
            return width() * height();
        }

		Point<T> pos() const
		{
			return Point<T>(left, top);
		}

        Point<T> minPos() const
        {
            return Point<T>(left, top);
        }

        Point<T> maxPos() const
        {
            return Point<T>(right, bottom);
        }

        Range<T> horizontal() const
        {
            return Range<T>(left, right);
        }

		Range<T> vertical() const
        {
            return Range<T>(top, bottom);
        }

        Point<T> center() const
        {
            return Point<T>(avg(left, right), avg(top, bottom));
        }

        Size<T> size() const
        {
            return Size<T>(width(), height());
        }

        void setWidth(T value)
        {
            right = left + value;
        }

        void setHeight(T value)
        {
            bottom = top  + value;
        }

        void setPos(T left, T top)
        {
            this->right  += left - this->left;
			this->bottom += top - this->top;
			this->left    = left;
			this->top     = top;
        }

		void setPos(const Point<T> & pos)
		{
			this->right += pos.x - this->left;
			this->bottom += pos.y - this->top;
			this->left = pos.x;
			this->top = pos.y;
		}

		void setEndPos(T right, T bottom)
		{
			this->left += right - this->right;
			this->top += bottom - this->bottom;
			this->right = right;
			this->bottom = bottom;
		}

		void setEndPos(const Point<T> & pos)
		{
			this->left  += pos.x - this->right;
			this->top   += pos.y - this->bottom;
			this->right  = pos.x;
			this->bottom = pos.y;
		}

        void setSize(T width, T height)
        {
            right  = left + width;
            bottom = top  + height;
        }

		void setSize(const Size<T> & size)
		{
			right = left + size.x;
			bottom = top + size.y;
		}

		void setPlacement(T left, T top, T width, T height)
		{
			this->left = left;
			this->top = top;
			this->right = left + width;
			this->bottom = top + height;
		}

        void setPlacement(const Point<T> & pos, const Size<T> & size)
        {
			this->left   = pos.x;
			this->top    = pos.y;
			this->right  = pos.x + size.x;
			this->bottom = pos.y + size.y;
        }

        void set(T left, T top, T right, T bottom)
        {
			this->left   = left;
			this->top    = top;
			this->right  = right;
			this->bottom = bottom;
        }

        template<typename U>
        void set(const Rect<U> & r)
        {
            left   = r.left;
            top    = r.top;
            right  = r.right;
            bottom = r.bottom;
        }

        template<typename U>
		void intersect(const Rect<U> & r)
        {
            if(r.left > left)
                left = r.left;

            if(r.top > top)
                top = r.top;

            if(r.right < right)
                right = r.right;

            if(r.bottom < bottom)
                bottom = r.bottom;

            if(width() < 0)
                right  = left;

            if(height() < 0)
                bottom = top;
        }

        template<typename U>
		void include(const Rect<U> & r)
        {
            if(r.left < left)
                left = r.left;

            if(r.top < top)
                top = r.top;

            if(r.right > right)
                right = r.right;

            if(r.bottom > bottom)
                bottom = r.bottom;
        }

        template<typename U>
        int compare(const Rect<U> & r) const
        {
            return left == r.left && top == r.top && right == r.right && bottom == r.bottom ? 0 : icomp(center(), r.center());
        }

        template<typename U>
        int compare(const Point<U> & pt) const
        {
            return contains(pt) ? 0 : icomp(center(), pt);
        }

        template<typename U>
        bool contains(const Rect<U> & r) const
        {
            return contains(r.minPos()) && contains(r.maxPos());
        }

        template<typename U>
        bool contains(const Point<U> & pt) const
        {
            return between(pt.x, left, right) && between(pt.y, top, bottom);
        }

        template<typename U>
        bool intersects(const Rect<U> & r) const
        {
            return (r.right >= left && r.left <= right) && (r.bottom >= top && r.top <= bottom);
        }
		
		Rect & resize(T dx, T dy)
		{
			left -= dx;
			top -= dy;
			right += dx;
			bottom += dy;

			return *this;
		}

		Rect & resize(T delta)
		{
			left -= delta;
			top -= delta;
			right += delta;
			bottom += delta;

			return *this;
		}

		void add(T x, T y)
		{
			left += x;
			right += x;
			top += y;
			bottom += y;
		}

		void subtract(T x, T y)
		{
			left -= x;
			right -= x;
			top -= y;
			bottom -= y;
		}

        template<typename U>
		void add(const Point<U> & pt)
        {
            left   += pt.x;
            right  += pt.x;
            top    += pt.y;
            bottom += pt.y;
        }

		template<typename U>
		void subtract(const Point<U> & pt)
		{
			left   -= pt.x;
			right  -= pt.x;
			top    -= pt.y;
			bottom -= pt.y;
		}

		Rect & moveTo(T x, T y)
		{
			add(x - left, y - top);
			return *this;
		}

		template<typename U>
		Rect & moveTo(const Point<U> & pt)
		{
			add(pt.x - left, pt.y - top);
			return *this;
		}

        Rect & moveToZero()
        {
            subtract(left, top);
			return *this;
        }

        Rect & centralize()
        {
            subtract(minPos() + size() / 2);
			return *this;
        }

		Rect & setMinimum(T x, T y)
		{
			if(left < x)
			{
				right += x - left;
				left = x;
			}

			if(top < y)
			{
				bottom += y - top;
				top = y;
			}

			return *this;
		}

		Rect & setMaximum(T x, T y)
		{
			if(right > x)
			{
				left -= right - x;
				right = x;
			}

			if(bottom > y)
			{
				top -= bottom - y;
				bottom = y;
			}

			return *this;
		}

		template<typename U>
		Rect & setMinimum(const Point<U> & pt)
		{
			return setMinimum(pt.x, pt.y);
		}

		template<typename U>
		Rect & setMaximum(const Point<U> & pt)
		{
			return setMaximum(pt.x, pt.y);
		}

        template<typename U>
        Rect & operator = (const Rect<U> & r)
        {
            set(r);
			return *this;
        }

        template<typename U>
        Rect & operator &= (const Rect<U> & r)
        {
            intersect(r);
			return *this;
        }

        template<typename U>
        Rect & operator |= (const Rect<U> & r)
        {
            include(r);
			return *this;
        }

        template<typename U>
        bool operator == (const Rect<U> & r) const
        {
            return compare(r) == 0;
        }

        template<typename U>
        bool operator != (const Rect<U> & r) const
        {
            return compare(r) != 0;
        }

        template<typename U>
        bool operator >= (const Rect<U> & r) const
        {
            return compare(r) >= 0;
        }

        template<typename U>
        bool operator <= (const Rect<U> & r) const
        {
            return compare(r) <= 0;
        }

        template<typename U>
        bool operator > (const Rect<U> & r) const
        {
            return compare(r) > 0;
        }

        template<typename U>
        bool operator < (const Rect<U> & r) const
        {
            return compare(r) < 0;
        }

        T & operator [](int i)
        {
            return data[i];
        }

		const T & operator [](int i) const
		{
			return data[i];
		}

        operator array<T, 4> & ()
        {
            return data;
        }

        operator const array<T, 4> & () const
        {
            return data;
        }

        template<typename U>
        Rect & operator += (const Point<U> & pt)
        {
            add(pt);
            return *this;
        }

        template<typename U>
        Rect & operator -= (const Point<U> & pt)
        {
            subtract(pt);
            return *this;
        }

        template<typename U>
        Rect operator + (const Point<U> & pt) const
        {
            return Rect(*this) += pt;
        }

        template<typename U>
        Rect operator - (const Point<U> & pt) const
        {
            return Rect(*this) -= pt;
        }
    };

    template <typename T, typename U>
    Rect<T> operator & (const Rect<T> & r1, const Rect<U> & r2)
    {
        return Rect<T>(r1) &= r2;
    }

    template <typename T, typename U>
    Rect<T> operator | (const Rect<T> & r1, const Rect<U> & r2)
    {
        return Rect<T>(r1) |= r2;
    }

    template <typename T>
    T minSide(const Rect<T> & r)
    {
        T min = r[0];

        for(uint i = 0; i < 4; i++)
            if(r[i] < min)
                min = r[i];

        return min;
    }

	template <typename T>
	Rect<T> centeredRect(const Point<T> & pos, const Size<T> & size)
	{
		const auto s = size / 2;
		return {pos.x - s.x, pos.y - s.y, pos.x + s.x, pos.y + s.y};
	}

	template <typename T>
	Rect<T> centeredSquare(const Point<T> & pos, T size)
	{
		const T s = size / 2;
		return {pos.x - s, pos.y - s, pos.x + s, pos.y + s};
	}

    typedef Rect<byte>  ByteRect;
    typedef Rect<int>   IntRect;
    typedef Rect<uint>  UintRect;
    typedef Rect<long>  LongRect;
    typedef Rect<float> FloatRect;

	typedef Size<byte>  ByteSize;
	typedef Size<int>   IntSize;
	typedef Size<uint>  UintSize;
	typedef Size<long>  LongSize;
	typedef Size<float> FloatSize;
}

//---------------------------------------------------------------------------
#endif
