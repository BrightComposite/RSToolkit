//---------------------------------------------------------------------------

#ifndef INTRINSIC_DATA_H
#define INTRINSIC_DATA_H

//---------------------------------------------------------------------------

#include <intrin.h>

#define USE_AVX

#ifdef USE_AVX
#include <immintrin.h>
#else
#include <tmmintrin.h>
#endif

#include <memory.h>

#include <core/meta/Meta.h>

#undef min
#undef max

//---------------------------------------------------------------------------

namespace Rapture
{
	template<size_t N>
	struct IntrinTypes {};

	template<>
	struct IntrinTypes<2>
	{
		typedef __m64 int_type;
		typedef __m64 float_type;
		typedef __m128d double_type;
	};

#ifdef USE_AVX
	template<>
	struct IntrinTypes<4>
	{
		typedef __m128i int_type;
		typedef __m128 float_type;
		typedef __m256d double_type;
	};
#else
	template<>
	struct IntrinTypes<4>
	{
		typedef __m128i int_type;
		typedef __m128 float_type;
		typedef __m128d double_type[2];
	};
#endif

	template<class T, size_t N>
	struct IntrinType {};

	template<size_t N>
	struct IntrinType<int, N>
	{
		typedef typename IntrinTypes<N>::int_type type;
	};

	template<size_t N>
	struct IntrinType<float, N>
	{
		typedef typename IntrinTypes<N>::float_type type;
	};

	template<size_t N>
	struct IntrinType<double, N>
	{
		typedef typename IntrinTypes<N>::double_type type;
	};

	template<class T, size_t N>
	using intrin_t = typename IntrinType<T, N>::type;

	template<class T, size_t N>
	using intrin_base_t = remove_extent_t<intrin_t<T, N>>;

	template<typename T, size_t N, size_t sz = array_size<intrin_t<T, N>>::value>
	struct alignas(sizeof(intrin_t<T, N>)) IntrinData
	{
		typedef intrin_t<T, N> type;
		typedef T inner[N];
		typedef IntrinData<T, N / sz> unit;

		static const size_t stride = sizeof(intrin_base_t<T, N>);

		union
		{
			inner data;
			type v;
		};

		IntrinData() {}
		IntrinData(const type & v)
		{
			Initialize::init(this->v, v);
		}

		IntrinData & operator = (const type & v)
		{
			Initialize::init(this->v, v);
			return *this;
		}

		operator type & ()
		{
			return v;
		}

		operator const type & () const
		{
			return v;
		}

		operator inner & ()
		{
			return data;
		}

		operator const inner & () const
		{
			return data;
		}

		unit & operator [] (size_t index)
		{
			return *reinterpret_cast<unit *>(&v[index]);
		}

		const unit & operator [] (size_t index) const
		{
			return *reinterpret_cast<const unit *>(&v[index]);
		}

		unit & operator [] (int index)
		{
			return *reinterpret_cast<unit *>(&v[index]);
		}

		const unit & operator [] (int index) const
		{
			return *reinterpret_cast<const unit *>(&v[index]);
		}

		template<int I, useif(I < N)>
		static inline T __vectorcall get(const IntrinData & in)
		{
			return in.data[I];
		}

		template<int I, useif(I < N)>
		static inline void __vectorcall set(type & out, T value)
		{
			reinterpret_cast<IntrinData *>(out)->data[I] = value;
		}
	};

	template<typename T>
	struct alignas(sizeof(intrin_t<T, 2>)) IntrinData<T, 2, 0>
	{
		typedef intrin_t<T, 2> type;
		typedef T inner[2];

		union
		{
			inner data;
			type v;
			T x, y;
		};

		IntrinData() {}
		IntrinData(const type & v) : v(v) {}

		IntrinData & __vectorcall operator = (const type & v)
		{
			this->v = v;
			return *this;
		}

		operator type & ()
		{
			return v;
		}

		operator const type & () const
		{
			return v;
		}

		operator inner & ()
		{
			return data;
		}

		operator const inner & () const
		{
			return data;
		}

		template<int I, useif(I < 2)>
		static inline T __vectorcall get(const IntrinData & in)
		{
			return in.data[I];
		}

		template<int I, useif(I < 2)>
		static inline void __vectorcall set(type & out, T value)
		{
			reinterpret_cast<IntrinData *>(out)->data[I] = value;
		}
	};

	template<typename T, size_t sz>
	struct alignas(sizeof(intrin_t<T, 4>)) IntrinData<T, 4, sz>
	{
		typedef intrin_t<T, 4> type;
		typedef T inner[4];
		typedef IntrinData<T, 4 / sz> unit;

		static const size_t stride = sizeof(intrin_base_t<T, 4>);

		union
		{
			inner data;
			type v;
			T x, y, z, w;
		};

		IntrinData() {}
		IntrinData(const type & v)
		{
			Initialize::init(this->v, v);
		}

		IntrinData & operator = (const type & v)
		{
			Initialize::init(this->v, v);
			return *this;
		}

		operator type & ()
		{
			return v;
		}

		operator const type & () const
		{
			return v;
		}

		operator inner & ()
		{
			return data;
		}

		operator const inner & () const
		{
			return data;
		}

		unit & operator [] (size_t index)
		{
			return *reinterpret_cast<unit *>(&v[index]);
		}

		const unit & operator [] (size_t index) const
		{
			return *reinterpret_cast<const unit *>(&v[index]);
		}

		unit & operator [] (int index)
		{
			return *reinterpret_cast<unit *>(&v[index]);
		}

		const unit & operator [] (int index) const
		{
			return *reinterpret_cast<const unit *>(&v[index]);
		}

		template<int I, useif(I < 4)>
		static inline T __vectorcall get(const IntrinData & in)
		{
			return in.data[I];
		}

		template<int I, useif(I < 4)>
		static inline void __vectorcall set(type & out, T value)
		{
			reinterpret_cast<IntrinData *>(out)->data[I] = value;
		}
	};

	template<typename T>
	struct alignas(sizeof(intrin_t<T, 4>)) IntrinData<T, 4, 0>
	{
		typedef intrin_t<T, 4> type;
		typedef T inner[4];

		union
		{
			inner data;
			type v;
			T x, y, z, w;
		};

		IntrinData() {}
		IntrinData(const type & v) : v(v) {}

		IntrinData & __vectorcall operator = (const type & v)
		{
			this->v = v;
			return *this;
		}

		operator type & ()
		{
			return v;
		}

		operator const type & () const
		{
			return v;
		}

		operator inner & ()
		{
			return data;
		}

		operator const inner & () const
		{
			return data;
		}

		template<int I, useif(I < 4)>
		static inline T __vectorcall get(const IntrinData & in)
		{
			return in.data[I];
		}

		template<int I, useif(I < 4)>
		static inline void __vectorcall set(type & out, T value)
		{
			reinterpret_cast<IntrinData *>(out)->data[I] = value;
		}
	};

	template<class T, size_t N>
	using intrin_data = typename IntrinData<T, N>;
}

//---------------------------------------------------------------------------
#endif