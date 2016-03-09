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
		template<typename Tx, typename Ty, require(std::is_pod<Tx>::value && std::is_pod<Ty>::value)>
        Point(Tx x, Ty y)           : x(static_cast<T>(x)),		y(static_cast<T>(y))     {}
		template<typename T_, require(std::is_pod<T_>::value)>
		Point(const T_ (&pt)[2])	: x(static_cast<T>(pt[0])),	y(static_cast<T>(pt[1])) {}
        template<typename T_, require(not_same(T, T_) && std::is_pod<T_>::value)>
        explicit Point(const Point<T_> & pt) : x(static_cast<T>(pt.x)),	y(static_cast<T>(pt.y))  {}

		template<typename Tx, typename Ty, require(std::is_pod<Tx>::value && std::is_pod<Ty>::value)>
        void set(Tx x, Ty y)
        {
            this->x = static_cast<T>(x);
			this->y = static_cast<T>(y);
        }

        template<typename T_>
        void set(const Point<T_> & pt)
        {
            x = static_cast<T>(pt.x);
            y = static_cast<T>(pt.y);
        }

        template<typename T_>
        void add(const Point<T_> & pt)
        {
            x += static_cast<T>(pt.x);
            y += static_cast<T>(pt.y);
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        void add(T_ value)
        {
            x += static_cast<T>(value);
            y += static_cast<T>(value);
        }

		template<typename T_>
		void subtract(const Point<T_> & pt)
		{
			x -= static_cast<T>(pt.x);
			y -= static_cast<T>(pt.y);
		}

		template<typename T_, require(std::is_pod<T_>::value)>
		void subtract(T_ value)
		{
			x -= static_cast<T>(value);
			y -= static_cast<T>(value);
		}

        template<typename T_>
        void multiply(const Point<T_> & pt)
        {
            x *= static_cast<T>(pt.x);
            y *= static_cast<T>(pt.y);
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        void multiply(T_ value)
        {
            x *= static_cast<T>(value);
            y *= static_cast<T>(value);
        }

        template<typename T_>
        void divide(const Point<T_> & pt)
        {
            x /= static_cast<T>(pt.x);
            y /= static_cast<T>(pt.y);
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        void divide(T_ value)
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

        template<typename T_>
        int compare(const Point<T_> & pt) const
        {
            return (x - pt.x) + (y - pt.y);
        }

        template<typename T_>
        Point middle(const Point<T_> & pt)
        {
			return {Rapture::middle(x, pt.x), Rapture::middle(y, pt.y)};
        }

        template<typename T_>
        Point & operator = (const Point<T_> & pt)
        {
            set(pt);
            return *this;
        }

        template<typename T_>
        bool operator == (const Point<T_> & pt) const
        {
            return x == pt.x && y == pt.y;
        }

        template<typename T_>
        bool operator != (const Point<T_> & pt) const
        {
            return x != pt.x || y != pt.y;
        }

        template<typename T_>
        bool operator > (const Point<T_> & pt) const
        {
            return x + y > pt.x + pt.y;
        }

        template<typename T_>
        bool operator < (const Point<T_> & pt) const
        {
            return x + y < pt.x + pt.y;
        }

        template<typename T_>
        bool operator >= (const Point<T_> & pt) const
        {
            return x + y >= pt.x + pt.y;
        }

        template<typename T_>
        bool operator <= (const Point<T_> & pt) const
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

        template<typename T_>
        Point & operator += (const Point<T_> & pt)
        {
            add(pt);
            return *this;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point & operator += (T_ value)
        {
            add(value);
            return *this;
        }

        template<typename T_>
        Point & operator -= (const Point<T_> & pt)
        {
            subtract(pt);
            return *this;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point & operator -= (T_ value)
        {
			subtract(value);
            return *this;
        }

        template<typename T_>
        Point & operator *= (const Point<T_> & pt)
        {
            multiply(pt);
            return *this;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point & operator *= (T_ value)
        {
            multiply(value);
            return *this;
        }

        template<typename T_>
        Point & operator /= (const Point<T_> & pt)
        {
            divide(pt);
            return *this;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point & operator /= (T_ value)
        {
            divide(value);
            return *this;
        }

        template<typename T_>
        Point operator + (const Point<T_> & pt) const
        {
            return Point(*this) += pt;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point operator + (T_ value) const
        {
            return Point(*this) += value;
        }

        template<typename T_>
        Point operator - (const Point<T_> & pt) const
        {
            return Point(*this) -= pt;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point operator - (T_ value) const
        {
            return Point(*this) -= value;
        }

        template<typename T_>
        Point operator * (const Point<T_> & pt) const
        {
            return Point(*this) *= pt;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point operator * (T_ value) const
        {
            return Point(*this) *= value;
        }

        template<typename T_>
        Point operator / (const Point<T_> & pt) const
        {
            return Point(*this) /= pt;
        }

		template<typename T_, require(std::is_pod<T_>::value)>
        Point operator / (T_ value) const
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
