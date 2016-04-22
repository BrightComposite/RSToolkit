//---------------------------------------------------------------------------

#ifndef POINT_H
#define POINT_H

//---------------------------------------------------------------------------

#include "Math.h"

//---------------------------------------------------------------------------

namespace Rapture {

    template<typename T>
    class Point
    {
    public:
        Point()                     : x(static_cast<T>(0)),		y(static_cast<T>(0))     {}
		Point(const Point & pt)		: x(pt.x),					y(pt.y)					 {}
		template<typename Tx, typename Ty, useif(std::is_pod<Tx>::value && std::is_pod<Ty>::value)>
        Point(Tx x, Ty y)           : x(static_cast<T>(x)),		y(static_cast<T>(y))     {}
		template<typename U, useif(std::is_pod<U>::value)>
		Point(const U (&pt)[2])	: x(static_cast<T>(pt[0])),	y(static_cast<T>(pt[1])) {}
        template<typename U, useif(is_not_same(T, U) && std::is_pod<U>::value)>
        explicit Point(const Point<U> & pt) : x(static_cast<T>(pt.x)),	y(static_cast<T>(pt.y))  {}

		template<typename Tx, typename Ty, useif(std::is_pod<Tx>::value && std::is_pod<Ty>::value)>
        void set(Tx x, Ty y)
        {
            this->x = static_cast<T>(x);
			this->y = static_cast<T>(y);
        }

        template<typename U>
        void set(const Point<U> & pt)
        {
            x = static_cast<T>(pt.x);
            y = static_cast<T>(pt.y);
        }

        template<typename U>
        void add(const Point<U> & pt)
        {
            x += static_cast<T>(pt.x);
            y += static_cast<T>(pt.y);
        }

		template<typename U, useif(std::is_pod<U>::value)>
        void add(U value)
        {
            x += static_cast<T>(value);
            y += static_cast<T>(value);
        }

		template<typename U>
		void subtract(const Point<U> & pt)
		{
			x -= static_cast<T>(pt.x);
			y -= static_cast<T>(pt.y);
		}

		template<typename U, useif(std::is_pod<U>::value)>
		void subtract(U value)
		{
			x -= static_cast<T>(value);
			y -= static_cast<T>(value);
		}

        template<typename U>
        void multiply(const Point<U> & pt)
        {
            x *= static_cast<T>(pt.x);
            y *= static_cast<T>(pt.y);
        }

		template<typename U, useif(std::is_pod<U>::value)>
        void multiply(U value)
        {
            x *= static_cast<T>(value);
            y *= static_cast<T>(value);
        }

        template<typename U>
        void divide(const Point<U> & pt)
        {
            x /= static_cast<T>(pt.x);
            y /= static_cast<T>(pt.y);
        }

		template<typename U, useif(std::is_pod<U>::value)>
        void divide(U value)
        {
            x /= static_cast<T>(value);
            y /= static_cast<T>(value);
        }

        void invert()
        {
            x = -x;
            y = -y;
        }

        /*=================================

         ^ y
		\|
         |
         |\
         | \   *C
         |  \
         |   *B
         |    \
         | *A  \
        -+-----------> x
        0|       \

			C > B and A < B

        =================================*/

        template<typename U>
        int compare(const Point<U> & pt) const
        {
            return (x - pt.x) + (y - pt.y);
        }

        template<typename U>
        Point middle(const Point<U> & pt)
        {
			return {Rapture::middle(x, pt.x), Rapture::middle(y, pt.y)};
        }

        template<typename U>
        Point & operator = (const Point<U> & pt)
        {
            set(pt);
            return *this;
        }

        template<typename U>
        bool operator == (const Point<U> & pt) const
        {
            return x == pt.x && y == pt.y;
        }

        template<typename U>
        bool operator != (const Point<U> & pt) const
        {
            return x != pt.x || y != pt.y;
        }

        template<typename U>
        bool operator > (const Point<U> & pt) const
        {
            return x + y > pt.x + pt.y;
        }

        template<typename U>
        bool operator < (const Point<U> & pt) const
        {
            return x + y < pt.x + pt.y;
        }

        template<typename U>
        bool operator >= (const Point<U> & pt) const
        {
            return x + y >= pt.x + pt.y;
        }

        template<typename U>
        bool operator <= (const Point<U> & pt) const
        {
            return x + y <= pt.x + pt.y;
        }

        T & operator [](int axis)
        {
            return data[axis];
        }

		const T & operator [](int axis) const
		{
			return data[axis];
		}

        template<typename U>
        Point & operator += (const Point<U> & pt)
        {
            add(pt);
            return *this;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point & operator += (U value)
        {
            add(value);
            return *this;
        }

        template<typename U>
        Point & operator -= (const Point<U> & pt)
        {
            subtract(pt);
            return *this;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point & operator -= (U value)
        {
			subtract(value);
            return *this;
        }

        template<typename U>
        Point & operator *= (const Point<U> & pt)
        {
            multiply(pt);
            return *this;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point & operator *= (U value)
        {
            multiply(value);
            return *this;
        }

        template<typename U>
        Point & operator /= (const Point<U> & pt)
        {
            divide(pt);
            return *this;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point & operator /= (U value)
        {
            divide(value);
            return *this;
        }

        template<typename U>
        Point operator + (const Point<U> & pt) const
        {
            return Point(*this) += pt;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point operator + (U value) const
        {
            return Point(*this) += value;
        }

        template<typename U>
        Point operator - (const Point<U> & pt) const
        {
            return Point(*this) -= pt;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point operator - (U value) const
        {
            return Point(*this) -= value;
        }

        template<typename U>
        Point operator * (const Point<U> & pt) const
        {
            return Point(*this) *= pt;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point operator * (U value) const
        {
            return Point(*this) *= value;
        }

        template<typename U>
        Point operator / (const Point<U> & pt) const
        {
            return Point(*this) /= pt;
        }

		template<typename U, useif(std::is_pod<U>::value)>
        Point operator / (U value) const
        {
            return Point(*this) /= value;
        }

		operator array_t<T, 2> & ()
		{
			return data;
		}

		operator const array_t<T, 2> & () const
		{
			return data;
		}

		union
		{
			struct
			{
				T x, y;
			};

			T data[2];
		};
    };

    typedef Point<long>  LongPoint;
    typedef Point<int>   IntPoint;
    typedef Point<float> FloatPoint;
}

//---------------------------------------------------------------------------
#endif
