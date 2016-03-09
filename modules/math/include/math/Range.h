//---------------------------------------------------------------------------

#ifndef RANGE_H
#define RANGE_H

//---------------------------------------------------------------------------

#include "Math.h"

//---------------------------------------------------------------------------

#undef min
#undef max

namespace Rapture
{
    template<typename T>
    class Range
    {
    public:
        T min, max;

		Range() : min(0), max(0) {}
		Range(T min, T max) : min(min), max(max) {}
        template<typename T_>
		Range(const Range<T_> & range) : min(range.min), max(range.max) {}

        bool isEmpty() const
        {
            return length() == 0;
        }

        T length() const
        {
            return max - min;
        }

        T center() const
        {
            return middle(min, max);
        }

        void set(T min, T max)
        {
			this->min = min;
			this->max = max;
        }

        template<typename T_>
        void set(const Range<T_> & range)
        {
			min = range.min;
			max = range.max;
        }

        void setWidth(T value)
        {
            max = min + value;
        }

        template<typename T_>
		void intersect(const Range<T_> & range)
        {
            if(range.min > min)
                min  = range.min;

            if(range.max < max)
				max = range.max;

            if(length() < 0)
                max = min;
        }

        template<typename T_>
		void include(const Range<T_> & range)
        {
            if(range.min < min)
				min = range.min;

            if(range.max > max)
				max = range.max;
        }

        template<typename T_>
        int compare(const Range<T_> & range) const
        {
            return (min == range.min && max == range.max) ? 0 : icomp(length(), range.length());
        }

        int compare(T value) const
        {
            return (value > max) ? 1 : (value < min) ? -1 : 0;
        }

        template<typename T_>
        bool includes(const Range<T_> & range) const
        {
            return (compare(range.min) & compare(range.max)) == 0;
        }

        bool includes(T value) const
        {
            return between(value, min, max);
        }

        template<typename T_>
        bool isIntersecting(const Range<T_> & range) const
        {
			return range.max >= min && range.min <= max;
        }

        template<typename T_>
		Range & operator = (const Range<T_> & range)
        {
            set(range);
            return *this;
        }

        template<typename T_>
		Range & operator &= (const Range<T_> & range)
        {
			intersect(range);
			return *this;
        }

        template<typename T_>
		Range & operator |= (const Range<T_> & range)
        {
            include(range);
			return *this;
        }

        template<typename T_>
        bool operator == (const Range<T_> & range)
        {
            return compare(range) == 0;
        }

        template<typename T_>
        bool operator != (const Range<T_> & range)
        {
            return compare(range) != 0;
        }

        template<typename T_>
        bool operator >= (const Range<T_> & range)
        {
            return compare(range) >= 0;
        }

        template<typename T_>
        bool operator <= (const Range<T_> & range)
        {
            return compare(range) <= 0;
        }

        template<typename T_>
        bool operator > (const Range<T_> & range)
        {
            return compare(range) > 0;
        }

        template<typename T_>
        bool operator < (const Range<T_> & range)
        {
            return compare(range) < 0;
        }

		template<typename T_>
		Range operator & (const Range<T_> & range)
		{
			return Range(*this) &= range;
		}

		template<typename T_>
		Range operator | (const Range<T_> & range)
		{
			return Range(*this) |= range;
		}
    };

    typedef Range<long> LRange, LongRange;
}

//---------------------------------------------------------------------------
#endif
