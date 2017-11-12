//---------------------------------------------------------------------------

#pragma once

#ifndef INTRINSIC_H
#define INTRINSIC_H

//---------------------------------------------------------------------------

#include "IntrinsicData.h"
#include "IntrinsicCvt.h"

#include <meta/bitmask.h>
#include <cmath>

//---------------------------------------------------------------------------

namespace asd
{
	template<typename T, int N>
	struct Intrinsic
	{
		static const bool implemented = false;
	};
	
#define _mm_reverse_ps(a) _mm_shuffle_ps(a, a, reverse_shuffle_4)
#define _mm_reverse_pd(a) _mm_shuffle_ps(a, a, reverse_shuffle_2)

	namespace internals
	{
		template<class T, template<class T> class Constant, class S>
		struct intrinsic_mask {};
	}

	template<class T, template<class T> class Constant, size_t Mask, int N = 4>
	struct IntrinsicMask
	{
		template<useif<Intrinsic<T, N>::implemented>>
		static inline const auto & get()
		{
			return internals::intrinsic_mask<T, Constant, unfold_mask<Mask, N>>::get();
		}
	};

	template struct api(core) IntrinData<int, 4>;
	template struct api(core) IntrinData<float, 4>;

#if SIMD_LEVEL >= SIMD_AVX
	template struct api(core) IntrinData<double, 4>;

#define _mm256_reverse_pd(a) _mm256_permute4x64_pd(a, reverse_shuffle_4)

#define intrinsic_constant(name)				\
	template<class T>							\
	struct name									\
	{											\
		static const T value;					\
	};											\
												\
	template struct api(core) name<byte>;		\
	template struct api(core) name<int>;		\
	template struct api(core) name<int64>;		\
	template struct api(core) name<float>;		\
	template struct api(core) name<double>;		\

#else

#define intrinsic_constant(name)				\
	template<class T>							\
	struct name									\
	{											\
		static const T value;					\
	};											\
												\
	template struct api(core) name<byte>;		\
	template struct api(core) name<int>;		\
	template struct api(core) name<int64>;		\
	template struct api(core) name<float>;
#endif

	intrinsic_constant(IntrinZero);
	intrinsic_constant(IntrinMax);
	intrinsic_constant(IntrinSignmask);
	intrinsic_constant(IntrinNofrac);

	/**
	 *	Integer intrinsics
	 */
	template<>
	struct Intrinsic<int, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<int, 4>;
		using type = typename inner::type;

	//#ifdef UNALIGNED_VECTORS
		using in_type = type;
	//#else
	//	using in_type = const type &;
	//#endif

		static api(core) const inner maximum;
		static api(core) const inner signmask;

		static const size_t size = sizeof(inner);

		static inline void __vectorcall load(in_type in, type & out)
		{
			out = _mm_load_si128(&in);
		}

		static inline type __vectorcall load(in_type & in)
		{
			return _mm_load_si128(&in);
		}

		static inline void __vectorcall load(const int & a, const int & b, const int & c, const int & d, type & out)
		{
			out = _mm_set_epi32(d, c, b, a);
		}

		static inline type __vectorcall load(const int & a, const int & b, const int & c, const int & d)
		{
			return _mm_set_epi32(d, c, b, a);
		}

		static inline void __vectorcall load(const int * data, type & out)
		{
			out = _mm_load_si128(reinterpret_cast<const type *>(data));
		}

		static inline type __vectorcall load(const int * data)
		{
			return _mm_load_si128(reinterpret_cast<const type *>(data));
		}

		static inline void __vectorcall store(in_type in, int * out)
		{
			_mm_store_si128(reinterpret_cast<type *>(out), in);
		}

		static inline void __vectorcall store(in_type in, type & out)
		{
			_mm_store_si128(&out, in);
		}

		static inline void __vectorcall zero(type & out)
		{
			out = _mm_setzero_si128();
		}

		static inline type __vectorcall zero()
		{
			return _mm_setzero_si128();
		}

		static inline void __vectorcall fill(int val, type & out)
		{
			out = _mm_set1_epi32(val);
		}

		static inline type __vectorcall fill(int val)
		{
			return _mm_set1_epi32(val);
		}

		static inline void __vectorcall add(in_type a, in_type b, type & out)
		{
			out = _mm_add_epi32(a, b);
		}

		static inline type __vectorcall add(in_type a, in_type b)
		{
			return _mm_add_epi32(a, b);
		}

		static inline void __vectorcall sub(in_type a, in_type b, type & out)
		{
			out = _mm_sub_epi32(a, b);
		}

		static inline type __vectorcall sub(in_type a, in_type b)
		{
			return _mm_sub_epi32(a, b);
		}
		
		#if SIMD_LEVEL >= SIMD_SSE4_1
		static inline void __vectorcall mul(in_type a, in_type b, type & out)
		{
			out = _mm_mullo_epi32(a, b);
		}

		static inline type __vectorcall mul(in_type a, in_type b)
		{
			return _mm_mullo_epi32(a, b);
		}
		#else
		static inline void __vectorcall mul(in_type a, in_type b, type & out)
		{
			auto tmp1 = _mm_mul_epu32(a,b); /* mul 2, 0*/
			auto tmp2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)); /* mul 3, 1 */
			out = shuffle2<2, 0, 2, 0>(tmp1, tmp2);
		}
		
		static inline type __vectorcall mul(in_type a, in_type b)
		{
			auto tmp1 = _mm_mul_epu32(a,b); /* mul 2, 0*/
			auto tmp2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)); /* mul 3, 1 */
			return shuffle2<2, 0, 2, 0>(tmp1, tmp2);
		}
		#endif

		static inline void __vectorcall div(in_type a, in_type b, type & out)
		{
			intrin_cvt<__m128, __m128i>(_mm_div_ps(intrin_cvt<__m128>(a), intrin_cvt<__m128>(b)), out);
		}

		static inline type __vectorcall div(in_type a, in_type b)
		{
			return intrin_cvt<__m128i>(_mm_div_ps(intrin_cvt<__m128>(a), intrin_cvt<__m128>(b)));
		}
		
	#if SIMD_LEVEL >= SIMD_SSE4_1
		static inline void __vectorcall mod(in_type a, in_type b, type & out)
		{
			out = _mm_set_epi32(
				_mm_extract_epi32(a, 3) % _mm_extract_epi32(b, 3),
				_mm_extract_epi32(a, 2) % _mm_extract_epi32(b, 2),
				_mm_extract_epi32(a, 1) % _mm_extract_epi32(b, 1),
				_mm_extract_epi32(a, 0) % _mm_extract_epi32(b, 0)
				);
		}

		static inline type __vectorcall mod(in_type a, in_type b)
		{
			return _mm_set_epi32(
				_mm_extract_epi32(a, 3) % _mm_extract_epi32(b, 3),
				_mm_extract_epi32(a, 2) % _mm_extract_epi32(b, 2),
				_mm_extract_epi32(a, 1) % _mm_extract_epi32(b, 1),
				_mm_extract_epi32(a, 0) % _mm_extract_epi32(b, 0)
			);
		}
	#else
		static inline void __vectorcall mod(in_type a, in_type b, type & out)
		{
			inner x1 = a, x2 = b;
			
			out = _mm_set_epi32(
				x1.w % x2.w,
				x1.z % x2.z,
				x1.y % x2.y,
				x1.x % x2.x
			);
		}
		
		static inline type __vectorcall mod(in_type a, in_type b)
		{
			inner x1 = a, x2 = b;
			
			return _mm_set_epi32(
				x1.w % x2.w,
				x1.z % x2.z,
				x1.y % x2.y,
				x1.x % x2.x
			);
		}
	#endif
		
		static inline void __vectorcall sqr(in_type a, type & out)
		{
			out = mul(a, a);
		}

		static inline type __vectorcall sqr(in_type a)
		{
			return mul(a, a);
		}

#if SIMD_LEVEL >= SIMD_SSE4_1
		static inline void __vectorcall invert(in_type a, type & out)
		{
			out = _mm_set_epi32(
				1 / _mm_extract_epi32(a, 3),
				1 / _mm_extract_epi32(a, 2),
				1 / _mm_extract_epi32(a, 1),
				1 / _mm_extract_epi32(a, 0)
			);
		}

		static inline type __vectorcall invert(in_type a)
		{
			return _mm_set_epi32(
				1 / _mm_extract_epi32(a, 3),
				1 / _mm_extract_epi32(a, 2),
				1 / _mm_extract_epi32(a, 1),
				1 / _mm_extract_epi32(a, 0)
				);
		}
#else
		static inline void __vectorcall invert(in_type a, type & out)
		{
			inner x = a;
			out = _mm_set_epi32(1 / x.w, 1 / x.z, 1 / x.y, 1 / x.x);
		}
		
		static inline type __vectorcall invert(in_type a)
		{
			inner x = a;
			return _mm_set_epi32(1 / x.w, 1 / x.z, 1 / x.y, 1 / x.x);
		}
#endif
		
		static inline void __vectorcall select(in_type a, in_type b, in_type mask, type & out) {
			out = bit_or(bit_and(mask, a), bit_andnot(mask, b));
		}
		
		static inline type __vectorcall select(in_type a, in_type b, in_type mask) {
			return bit_or(bit_and(mask, a), bit_andnot(mask, b));
		}

#if SIMD_LEVEL >= SIMD_SSE4_1
		static inline void __vectorcall min(in_type a, in_type b, type & out)
		{
			out = _mm_min_epi32(a, b);
		}

		static inline type __vectorcall min(in_type a, in_type b)
		{
			return _mm_min_epi32(a, b);
		}

		static inline void __vectorcall max(in_type a, in_type b, type & out)
		{
			out = _mm_max_epi32(a, b);
		}

		static inline type __vectorcall max(in_type a, in_type b)
		{
			return _mm_max_epi32(a, b);
		}
#else
		static inline void __vectorcall min(in_type a, in_type b, type & out)
		{
			out = select(a, b, _mm_cmplt_epi32(a, b));
		}
		
		static inline type __vectorcall min(in_type a, in_type b)
		{
			return select(a, b, _mm_cmplt_epi32(a, b));
		}
		
		static inline void __vectorcall max(in_type a, in_type b, type & out)
		{
			out = select(a, b, _mm_cmpgt_epi32(a, b));
		}
		
		static inline type __vectorcall max(in_type a, in_type b)
		{
			return select(a, b, _mm_cmpgt_epi32(a, b));
		}
#endif
		
		static inline void __vectorcall hadd2(in_type a, in_type b, type & out)
		{
			out = _mm_hadd_epi32(a, b);
		}

		static inline type __vectorcall hadd2(in_type a, in_type b)
		{
			return _mm_hadd_epi32(a, b);
		}

		static inline void __vectorcall hadd(in_type a, type & out)
		{
			out = _mm_hadd_epi32(a, _mm_setzero_si128());
		}

		static inline type __vectorcall hadd(in_type a)
		{
			return _mm_hadd_epi32(a, _mm_setzero_si128());
		}

		static inline int __vectorcall sum(in_type a)
		{
			type v = _mm_hadd_epi32(a, a);
			v = _mm_hadd_epi32(v, v);
			return _mm_cvtsi128_si32(v);
		}

		static inline void __vectorcall fill_sum(in_type a, type & out)
		{
			out = _mm_hadd_epi32(a, a);
			out = _mm_hadd_epi32(out, out);
		}

		static inline type __vectorcall fill_sum(in_type a)
		{
			type v = _mm_hadd_epi32(a, a);
			return _mm_hadd_epi32(v, v);
		}

		static inline void __vectorcall sqrt(in_type a, type & out)
		{
			out = _mm_cvtps_epi32(_mm_sqrt_ps(_mm_cvtepi32_ps(a)));
		}

		static inline type __vectorcall sqrt(in_type a)
		{
			return _mm_cvtps_epi32(_mm_sqrt_ps(_mm_cvtepi32_ps(a)));
		}

		static inline void __vectorcall bit_and(in_type a, in_type b, type & out)
		{
			out = _mm_and_si128(a, b);
		}

		static inline type __vectorcall bit_and(in_type a, in_type b)
		{
			return _mm_and_si128(a, b);
		}

		static inline void __vectorcall bit_or(in_type a, in_type b, type & out)
		{
			out = _mm_or_si128(a, b);
		}

		static inline type __vectorcall bit_or(in_type a, in_type b)
		{
			return _mm_or_si128(a, b);
		}

		static inline void __vectorcall bit_andnot(in_type a, in_type b, type & out)
		{
			out = _mm_andnot_si128(a, b);
		}

		static inline type __vectorcall bit_andnot(in_type a, in_type b)
		{
			return _mm_andnot_si128(a, b);
		}

		static inline void __vectorcall bit_xor(in_type a, in_type b, type & out)
		{
			out = _mm_xor_si128(a, b);
		}

		static inline type __vectorcall bit_xor(in_type a, in_type b)
		{
			return _mm_xor_si128(a, b);
		}

		static inline void __vectorcall bit_shr(in_type a, in_type b, type & out)
		{
			out = _mm_sra_epi32(a, b);
		}

		static inline type __vectorcall bit_shr(in_type a, in_type b)
		{
			return _mm_sra_epi32(a, b);
		}

		template<int I>
		static inline void __vectorcall bit_shr(in_type a, type & out)
		{
			out = _mm_srai_epi32(a, I);
		}

		template<int I>
		static inline type __vectorcall bit_shr(in_type a)
		{
			return _mm_srai_epi32(a, I);
		}

		static inline void __vectorcall bit_shl(in_type a, in_type b, type & out)
		{
			out = _mm_sll_epi32(a, b);
		}

		static inline type __vectorcall bit_shl(in_type a, in_type b)
		{
			return _mm_sll_epi32(a, b);
		}

		template<int I>
		static inline void __vectorcall bit_shl(in_type a, type & out)
		{
			out = _mm_slli_epi32(a, I);
		}

		template<int I>
		static inline type __vectorcall bit_shl(in_type a)
		{
			return _mm_slli_epi32(a, I);
		}

		static inline bool __vectorcall equal(in_type a, in_type b)
		{
			return (_mm_movemask_epi8(_mm_cmpeq_epi32(a, b)) & 0x8888) == 0x8888;
		}

		static inline bool __vectorcall notequal(in_type a, in_type b)
		{
			return (_mm_movemask_epi8(_mm_cmpeq_epi32(a, b)) & 0x8888) != 0x8888;
		}

		static inline void __vectorcall cmple(in_type a, in_type b, type & out)
		{
			out = bit_xor(_mm_cmpgt_epi32(a, b), maximum);
		}

		static inline type __vectorcall cmple(in_type a, in_type b)
		{
			return bit_xor(_mm_cmpgt_epi32(a, b), maximum);
		}

		static inline void __vectorcall abs(in_type a, type & out)
		{
			out = bit_andnot(signmask, a);
		}

		static inline type __vectorcall abs(in_type a)
		{
			return bit_andnot(signmask, a);
		}

		static inline void __vectorcall sign(in_type a, type & out)
		{
			out = bit_and(signmask, a);
		}

		static inline type __vectorcall sign(in_type a)
		{
			return bit_and(signmask, a);
		}

		static inline void __vectorcall negate(in_type a, type & out)
		{
			out = bit_xor(signmask, a);
		}

		static inline type __vectorcall negate(in_type a)
		{
			return bit_xor(signmask, a);
		}

		static inline void __vectorcall reverse(in_type a, type & out)
		{
			out = shuffle<3, 2, 1, 0>(a);
		}

		static inline type __vectorcall reverse(in_type a)
		{
			return shuffle<3, 2, 1, 0>(a);
		}
		
	#if SIMD_LEVEL >= SIMD_SSE4_1
		template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
		static inline void __vectorcall blend(in_type a, in_type b, type & out)
		{
			out = _mm_blend_epi16(a, b, mk_mask8(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
		static inline type __vectorcall blend(in_type a, in_type b)
		{
			return _mm_blend_epi16(a, b, mk_mask8(A, B, C, D));
		}
		
		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline void __vectorcall shuffle2(in_type a, in_type b, type & out)
		{
			out = _mm_blend_epi16(
				_mm_shuffle_epi32(a, mk_shuffle_4(A, B, 0, 0)),
				_mm_shuffle_epi32(b, mk_shuffle_4(0, 0, C, D)),
				0xF0
				);
		}
		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline type __vectorcall shuffle2(in_type a, in_type b)
		{
			return _mm_blend_epi16(
				_mm_shuffle_epi32(a, mk_shuffle_4(A, B, 0, 0)),
				_mm_shuffle_epi32(b, mk_shuffle_4(0, 0, C, D)),
				0xF0
				);
		}
	#else
		template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
		static inline void __vectorcall blend(in_type a, in_type b, type & out)
		{
			auto mask = IntrinsicMask<int, IntrinMax, mk_mask4(A, B, C, D)>::get();
			out = bit_or(bit_andnot(mask, a), bit_and(mask, b));
		}
		
		template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
		static inline type __vectorcall blend(in_type a, in_type b)
		{
			auto mask = IntrinsicMask<int, IntrinMax, mk_mask4(A, B, C, D)>::get();
			return bit_or(bit_andnot(mask, a), bit_and(mask, b));
		}
		
		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline void __vectorcall shuffle2(in_type a, in_type b, type & out)
		{
			out = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), mk_shuffle_4(A, B, C, D)));
		}
		
		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline type __vectorcall shuffle2(in_type a, in_type b)
		{
			return _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), mk_shuffle_4(A, B, C, D)));
		}
	#endif

		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline void __vectorcall shuffle(in_type a, type & out)
		{
			out = _mm_shuffle_epi32(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline type __vectorcall shuffle(in_type a)
		{
			return _mm_shuffle_epi32(a, mk_shuffle_4(A, B, C, D));
		}
	};

	/**
	 *	Float intrinsics
	 */
	template<>
	struct Intrinsic<float, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<float, 4>;
		using type = typename inner::type;

		using in_type = type;

		static api(core) const inner signmask;
		static api(core) const inner nofrac;

		static const size_t size = sizeof(inner);

		static inline void __vectorcall load(in_type in, type & out)
		{
			out = _mm_load_ps(reinterpret_cast<const float *>(&in));
		}

		static inline type __vectorcall load(in_type in)
		{
			return _mm_load_ps(reinterpret_cast<const float *>(&in));
		}

		static inline void __vectorcall load(const float & a, const float & b, const float & c, const float & d, type & out)
		{
			out = _mm_set_ps(d, c, b, a);
		}

		static inline type __vectorcall load(const float & a, const float & b, const float & c, const float & d)
		{
			return _mm_set_ps(d, c, b, a);
		}

		static inline type __vectorcall load(const __m64 & a, const __m64 & b)
		{
			return _mm_set_ps(_m_hi(b), _m_lo(b), _m_hi(a), _m_lo(a));
		}

		static inline void __vectorcall load(const float * in, type & out)
		{
			out = _mm_load_ps(in);
		}

		static inline type __vectorcall load(const float * in)
		{
			return _mm_load_ps(in);
		}

		static inline void __vectorcall store(in_type in, float * out)
		{
			_mm_store_ps(out, in);
		}

		static inline void __vectorcall store(in_type in, type & out)
		{
			_mm_store_ps(reinterpret_cast<float *>(&out), in);
		}
		
		static inline type __vectorcall zero()
		{
			return _mm_setzero_ps();
		}

		static inline type __vectorcall fill(float val)
		{
			return _mm_set_ps1(val);
		}

		static inline type __vectorcall add(in_type a, in_type b)
		{
			return _mm_add_ps(a, b);
		}

		static inline type __vectorcall addx(in_type a, in_type b)
		{
			return _mm_add_ss(a, b);
		}

		static inline type __vectorcall sub(in_type a, in_type b)
		{
			return _mm_sub_ps(a, b);
		}

		static inline type __vectorcall mul(in_type a, in_type b)
		{
			return _mm_mul_ps(a, b);
		}

		static inline type __vectorcall div(in_type a, in_type b)
		{
			return _mm_div_ps(a, b);
		}

		static inline type __vectorcall mod(in_type a, in_type b)
		{
			return sub(a, mul(trunc(div(a, b)), b));
		}

		static inline type __vectorcall sqr(in_type a)
		{
			return mul(a, a);
		}

		static inline type __vectorcall invert(in_type a)
		{
			type tmp = _mm_rcp_ps(a);
			return sub(add(tmp, tmp), mul(a, mul(tmp, tmp)));
		}

		static inline type __vectorcall min(in_type a, in_type b)
		{
			return _mm_min_ps(a, b);
		}

		static inline type __vectorcall max(in_type a, in_type b)
		{
			return _mm_max_ps(a, b);
		}

		static inline type __vectorcall hadd2(in_type a, in_type b)
		{
			return _mm_hadd_ps(a, b);
		}

		static inline type __vectorcall hadd(in_type a)
		{
			return _mm_hadd_ps(a, a);
		}

		static inline float __vectorcall sum(in_type a)
		{
			type v = _mm_hadd_ps(a, a);
			v = _mm_hadd_ps(v, v);
			return _mm_cvtss_f32(v);
		}

		static inline type __vectorcall fill_sum(in_type a)
		{
			type v = _mm_hadd_ps(a, a);
			return _mm_hadd_ps(v, v);
		}

		static inline type __vectorcall sqrt(in_type a)
		{
			return _mm_sqrt_ps(a);
		}

		static inline type __vectorcall bit_and(in_type a, in_type b)
		{
			return _mm_and_ps(a, b);
		}

		static inline type __vectorcall bit_or(in_type a, in_type b)
		{
			return _mm_or_ps(a, b);
		}

		static inline type __vectorcall bit_andnot(in_type a, in_type b)
		{
			return _mm_andnot_ps(a, b);
		}

		static inline type __vectorcall bit_xor(in_type a, in_type b)
		{
			return _mm_xor_ps(a, b);
		}

		static inline bool __vectorcall equal(in_type a, in_type b)
		{
			return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) == 0xf;
		}

		static inline bool __vectorcall notequal(in_type a, in_type b)
		{
			return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) != 0xf;
		}

		static inline type __vectorcall cmple(in_type a, in_type b)
		{
			return _mm_cmple_ps(a, b);
		}

		static inline type __vectorcall cmpgt(in_type a, in_type b)
		{
			return _mm_cmpgt_ps(a, b);
		}

		static inline type __vectorcall abs(in_type a)
		{
			return bit_andnot(signmask, a);
		}

		static inline type __vectorcall sign(in_type a)
		{
			return bit_and(signmask, a);
		}

		static inline type __vectorcall trunc(in_type a)
		{
			return _mm_cvtepi32_ps(_mm_cvtps_epi32(a));
		}

		static inline type __vectorcall floor(in_type a)
		{
			static auto one = load(1.0f, 1.0f, 1.0f, 1.0f);

			auto v = trunc(a);
			return sub(v, bit_and(cmpgt(v, a), one)); // subtract one if truncation is greater than a
		}

		static inline type __vectorcall ceil(in_type a)
		{
			static auto one = load(1.0f, 1.0f, 1.0f, 1.0f);

			auto v = trunc(a);
			return add(v, bit_and(cmple(v, a), one)); // add one if truncation is less than a
		}

		static inline type __vectorcall round(in_type a)
		{
			auto v = bit_or(nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			return bit_xor(bit_and(sub(add(a, v), v), mask), bit_andnot(mask, a));
		}

		static inline type __vectorcall negate(in_type a)
		{
			return bit_xor(signmask, a);
		}

		static inline type __vectorcall reverse(in_type a)
		{
			return _mm_reverse_ps(a);
		}
		
	#if SIMD_LEVEL >= SIMD_SSE4_1
		template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
		static inline type __vectorcall blend(in_type a, in_type b)
		{
			return _mm_blend_ps(a, b, mk_mask4(A, B, C, D));
		}

	#else
		template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
		static inline type __vectorcall blend(in_type a, in_type b)
		{
			auto mask = IntrinsicMask<float, IntrinMax, mk_mask4(A, B, C, D)>::get();
			return bit_or(bit_andnot(mask, a), bit_and(mask, b));
		}
	#endif
		
		template<byte A, byte B, byte C, byte D, useif<
			(A < 4 && B < 4 && C < 4 && D < 4)
			>
		>
		static inline type __vectorcall shuffle2(in_type a, in_type b)
		{
			return _mm_shuffle_ps(a, b, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif<
			(A < 4 && B < 4 && C < 4 && D < 4)
			>
		>
		static inline type __vectorcall shuffle(in_type a)
		{
			return _mm_shuffle_ps(a, a, mk_shuffle_4(A, B, C, D));
		}
	};

#if SIMD_LEVEL >= SIMD_AVX

	/**
	 *	Double intrinsics
	 */
	template<>
	struct Intrinsic<double, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<double, 4>;
		using type = inner::type;

		static api(core) const inner signmask;
		static api(core) const inner nofrac;

		static const size_t size = sizeof(inner);

		static inline void __vectorcall load(const inner & in, type & out)
		{
			out = _mm256_load_pd(in);
		}

		static inline inner __vectorcall load(const inner & in)
		{
			return _mm256_load_pd(in);
		}

		static inline void __vectorcall load(const double & a, const double & b, const double & c, const double & d, type & out)
		{
			out = _mm256_set_pd(d, c, b, a);
		}

		static inline inner __vectorcall load(const double & a, const double & b, const double & c, const double & d)
		{
			return _mm256_set_pd(d, c, b, a);
		}

		static inline void __vectorcall load(const __m128d & a, const __m128d & b, type & out)
		{
			out = _mm256_insertf128_pd(_mm256_castpd128_pd256(a), b, 1);
		}

		static inline inner __vectorcall load(const __m128d & a, const __m128d & b)
		{
			return _mm256_insertf128_pd(_mm256_castpd128_pd256(a), b, 1);
		}

		static inline void __vectorcall load(const double * data, type & out)
		{
			out = _mm256_load_pd(data);
		}

		static inline inner __vectorcall load(const double * data)
		{
			return _mm256_load_pd(data);
		}

		static inline void __vectorcall store(const type & in, inner & out)
		{
			_mm256_store_pd(out, in);
		}

		static inline void __vectorcall zero(type & out)
		{
			out = _mm256_setzero_pd();
		}

		static inline inner __vectorcall zero()
		{
			return _mm256_setzero_pd();
		}

		static inline void __vectorcall fill(double val, type & out)
		{
			out = _mm256_set1_pd(val);
		}

		static inline inner __vectorcall fill(double val)
		{
			return _mm256_set1_pd(val);
		}

		static inline void __vectorcall add(const type & a, const type & b, type & out)
		{
			out = _mm256_add_pd(a, b);
		}

		static inline inner __vectorcall add(const type & a, const type & b)
		{
			return _mm256_add_pd(a, b);
		}

		static inline void __vectorcall sub(const type & a, const type & b, type & out)
		{
			out = _mm256_sub_pd(a, b);
		}

		static inline inner __vectorcall sub(const type & a, const type & b)
		{
			return _mm256_sub_pd(a, b);
		}

		static inline void __vectorcall mul(const type & a, const type & b, type & out)
		{
			out = _mm256_mul_pd(a, b);
		}

		static inline inner __vectorcall mul(const type & a, const type & b)
		{
			return _mm256_mul_pd(a, b);
		}

		static inline void __vectorcall div(const type & a, const type & b, type & out)
		{
			out = _mm256_div_pd(a, b);
		}

		static inline inner __vectorcall div(const type & a, const type & b)
		{
			return _mm256_div_pd(a, b);
		}

		static inline void __vectorcall sqr(const type & a, type & out)
		{
			out = _mm256_mul_pd(a, a);
		}

		static inline inner __vectorcall sqr(const type & a)
		{
			return {_mm256_mul_pd(a, a)};
		}

		static inline void __vectorcall invert(const type & a, type & out)
		{
			out = _mm256_div_pd(_mm256_set_pd(1.0, 1.0, 1.0, 1.0), a);
		}

		static inline inner __vectorcall invert(const type & a)
		{
			return _mm256_div_pd(_mm256_set_pd(1.0, 1.0, 1.0, 1.0), a);
		}

		static inline void __vectorcall min(const type & a, const type & b, type & out)
		{
			out = _mm256_min_pd(a, b);
		}

		static inline inner __vectorcall min(const type & a, const type & b)
		{
			return _mm256_min_pd(a, b);
		}

		static inline void __vectorcall max(const type & a, const type & b, type & out)
		{
			out = _mm256_max_pd(a, b);
		}

		static inline inner __vectorcall max(const type & a, const type & b)
		{
			return _mm256_max_pd(a, b);
		}

		static inline void __vectorcall hadd2(const type & a, const type & b, type & out)
		{
			out = _mm256_hadd_pd(a, b);
		}

		static inline inner __vectorcall hadd2(const type & a, const type & b)
		{
			return _mm256_hadd_pd(a, b);
		}

		static inline void __vectorcall hadd(const type & a, type & out)
		{
			out = _mm256_hadd_pd(a, a);
		}

		static inline inner __vectorcall hadd(const type & a)
		{
			return _mm256_hadd_pd(a, a);
		}

		static inline double __vectorcall sum(const type & a)
		{
			type v = _mm256_hadd_pd(a, shuffle<2, 3, 2, 3>(a));
			v = _mm256_hadd_pd(v, v);
			return _mm_cvtsd_f64(_mm256_castpd256_pd128(v));
		}

		static inline void __vectorcall fill_sum(const type & a, type & out)
		{
			out = _mm256_hadd_pd(shuffle<0, 1, 0, 1>(a), shuffle<2, 3, 2, 3>(a));
			out = _mm256_hadd_pd(shuffle<0, 1, 0, 1>(out), shuffle<2, 3, 2, 3>(out));
		}

		static inline inner __vectorcall fill_sum(const type & a)
		{
			type out = _mm256_hadd_pd(shuffle<0, 1, 0, 1>(a), shuffle<2, 3, 2, 3>(a));
			return _mm256_hadd_pd(shuffle<0, 1, 0, 1>(out), shuffle<2, 3, 2, 3>(out));
		}

		static inline void __vectorcall sqrt(const type & a, type & out)
		{
			out = _mm256_sqrt_pd(a);
		}

		static inline inner __vectorcall sqrt(const type & a)
		{
			return _mm256_sqrt_pd(a);
		}

		static inline void __vectorcall bit_and(const type & a, const type & b, type & out)
		{
			out = _mm256_and_pd(a, b);
		}

		static inline inner __vectorcall bit_and(const type & a, const type & b)
		{
			return {_mm256_and_pd(a, b)};
		}

		static inline void __vectorcall bit_or(const type & a, const type & b, type & out)
		{
			out = _mm256_or_pd(a, b);
		}

		static inline inner __vectorcall bit_or(const type & a, const type & b)
		{
			return {_mm256_or_pd(a, b)};
		}

		static inline void __vectorcall bit_andnot(const type & a, const type & b, type & out)
		{
			out = _mm256_andnot_pd(a, b);
		}

		static inline inner __vectorcall bit_andnot(const type & a, const type & b)
		{
			return {_mm256_andnot_pd(a, b)};
		}

		static inline void __vectorcall bit_xor(const type & a, const type & b, type & out)
		{
			out = _mm256_xor_pd(a, b);
		}

		static inline inner __vectorcall bit_xor(const type & a, const type & b)
		{
			return {_mm256_xor_pd(a, b)};
		}

		static inline bool __vectorcall equal(const type & a, const type & b)
		{
			return _mm256_movemask_pd(_mm256_cmp_pd(a, b, _CMP_EQ_OS)) == 0xf;
		}

		static inline bool __vectorcall notequal(const type & a, const type & b)
		{
			return _mm256_movemask_pd(_mm256_cmp_pd(a, b, _CMP_EQ_OS)) != 0xf;
		}

		static inline void __vectorcall cmple(const type & a, const type & b, type & out)
		{
			out = _mm256_cmp_pd(a, b, _CMP_LE_OS);
		}

		static inline inner __vectorcall cmple(const type & a, const type & b)
		{
			return {_mm256_cmp_pd(a, b, _CMP_LE_OS)};
		}

		static inline void __vectorcall abs(const type & a, type & out)
		{
			out = bit_andnot(signmask, a);
		}

		static inline inner __vectorcall abs(const type & a)
		{
			return {bit_andnot(signmask, a)};
		}

		static inline void __vectorcall sign(const type & a, type & out)
		{
			out = bit_and(signmask, a);
		}

		static inline inner __vectorcall sign(const type & a)
		{
			return {bit_and(signmask, a)};
		}

		static inline void __vectorcall round(const type & a, type & out)
		{
			auto v = bit_or(nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			out = bit_xor(bit_and(sub(add(a, v), v), mask), bit_andnot(mask, a));
		}

		static inline inner __vectorcall round(const type & a)
		{
			auto v = bit_or(nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			return {bit_xor(bit_and(sub(add(a, v), v), mask), bit_andnot(mask, a))};
		}

		static inline void __vectorcall negate(const type & a, type & out)
		{
			out = bit_xor(signmask, a);
		}

		static inline inner __vectorcall negate(const type & a)
		{
			return {bit_xor(signmask, a)};
		}

		static inline void __vectorcall reverse(const type & a, type & out)
		{
			out = _mm256_reverse_pd(a);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return _mm256_reverse_pd(a);
		}

		template<byte A, byte B, byte C, byte D, useif<
            (A < 2 && B < 2 && C < 2 && D < 2)
            >
        >
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm256_blend_pd(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif<
            (A < 2 && B < 2 && C < 2 && D < 2)
            >
        >
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return _mm256_blend_pd(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif<
            (A < 4 && B < 4 && C < 4 && D < 4)
            >
        >
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = blend<0, 0, 1, 1>(_mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D)), _mm256_permute4x64_pd(b, mk_shuffle_4(A, B, C, D)));
		}

		template<byte A, byte B, byte C, byte D, useif<
            (A < 4 && B < 4 && C < 4 && D < 4)
            >
        >
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return blend<0, 0, 1, 1>(_mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D)), _mm256_permute4x64_pd(b, mk_shuffle_4(A, B, C, D)));
		}

		template<byte A, byte B, byte C, byte D, useif<
            (A < 4 && B < 4 && C < 4 && D < 4)
            >
        >
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = _mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif<
            (A < 4 && B < 4 && C < 4 && D < 4)
            >
        >
		static inline inner __vectorcall shuffle(const type & a)
		{
			return _mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D));
		}
	};

#endif

	/**
	 *	byte intrinsics
	 */
	template<>
	struct Intrinsic<byte, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<byte, 4>;
		using type = typename inner::type;

		static api(core) const inner maximum;
		static api(core) const inner signmask;

		static const size_t size = sizeof(inner);

		static inline void load(const inner & in, type & out)
		{
			out = in;
		}

		static inline type load(const inner & in)
		{
			return in;
		}

		static inline void load(const byte & a, const byte & b, const byte & c, const byte & d, type & out)
		{
			out = {d, c, b, a};
		}

		static inline type load(const byte & a, const byte & b, const byte & c, const byte & d)
		{
			return type {d, c, b, a};
		}

		static inline void load(const byte * data, type & out)
		{
			out = {data[3], data[2], data[1], data[0]};
		}

		static inline type load(const byte * data)
		{
			return type {data[3], data[2], data[1], data[0]};
		}

		static inline void store(const type & in, inner & out)
		{
			out = in;
		}

		static inline void zero(type & out)
		{
			out.i = 0;
		}

		static inline type zero()
		{
			return type{};
		}

		static inline void fill(byte val, type & out)
		{
			out = {val, val, val, val};
		}

		static inline void fill(byte val, inner & out)
		{
			out = {val, val, val, val};
		}

		static inline inner fill(byte val)
		{
			return type {val, val, val, val};
		}

		static inline void add(const type & a, const type & b, type & out)
		{
			out = {byte(a.x + b.x), byte(a.y + b.y), byte(a.z + b.z), byte(a.w + b.w)};
		}

		static inline inner add(const type & a, const type & b)
		{
			return type {byte(a.x + b.x), byte(a.y + b.y), byte(a.z + b.z), byte(a.w + b.w)};
		}

		static inline void sub(const type & a, const type & b, type & out)
		{
			out = {byte(a.x - b.x), byte(a.y - b.y), byte(a.z - b.z), byte(a.w - b.w)};
		}

		static inline inner sub(const type & a, const type & b)
		{
			return __m32 {byte(a.x - b.x), byte(a.y - b.y), byte(a.z - b.z), byte(a.w - b.w)};
		}

		static inline void mul(const type & a, const type & b, type & out)
		{
			out = {byte(a.x * b.x), byte(a.y * b.y), byte(a.z * b.z), byte(a.w * b.w)};
		}

		static inline inner mul(const type & a, const type & b)
		{
			return __m32 {byte(a.x * b.x), byte(a.y * b.y), byte(a.z * b.z), byte(a.w * b.w)};
		}

		static inline void div(const type & a, const type & b, type & out)
		{
			out = {byte(a.x / b.x), byte(a.y / b.y), byte(a.z / b.z), byte(a.w / b.w)};
		}

		static inline inner div(const type & a, const type & b)
		{
			return __m32 {byte(a.x / b.x), byte(a.y / b.y), byte(a.z / b.z), byte(a.w / b.w)};
		}

		static inline void sqr(const type & a, type & out)
		{
			out = {byte(a.x * a.x), byte(a.y * a.y), byte(a.z * a.z), byte(a.w * a.w)};
		}

		static inline inner sqr(const type & a)
		{
			return __m32 {byte(a.x * a.x), byte(a.y * a.y), byte(a.z * a.z), byte(a.w * a.w)};
		}

		static inline void invert(const type & a, type & out)
		{
			out.i = 0;
		}

		static inline inner invert(const type & a)
		{
			return __m32 {};
		}

		static inline void min(const type & a, const type & b, type & out)
		{
			out = {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
		}

		static inline inner min(const type & a, const type & b)
		{
			return __m32 {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
		}

		static inline void max(const type & a, const type & b, type & out)
		{
			out = {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
		}

		static inline inner max(const type & a, const type & b)
		{
			return __m32 {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
		}

		static inline void hadd2(const type & a, const type & b, type & out)
		{
			out = {byte(a.x + a.y), byte(a.z + a.w), byte(b.x + b.y), byte(b.z + b.w)};
		}

		static inline inner hadd2(const type & a, const type & b)
		{
			return __m32 {byte(a.x + a.y), byte(a.z + a.w), byte(b.x + b.y), byte(b.z + b.w)};
		}

		static inline void hadd(const type & a, type & out)
		{
			out = {byte(a.x + a.y), byte(a.z + a.w), 0, 0};
		}

		static inline inner hadd(const type & a)
		{
			return __m32 {byte(a.x + a.y), byte(a.z + a.w), 0, 0};
		}

		static inline byte sum(const type & a)
		{
			return byte(a.x + a.y + a.z + a.w);
		}

		static inline void fill_sum(const type & a, type & out)
		{
			byte b = a.x + a.y + a.z + a.w;
			out = {b, b, b, b};
		}

		static inline inner fill_sum(const type & a)
		{
			byte b = a.x + a.y + a.z + a.w;
			return __m32 {b, b, b, b};
		}

		static inline void sqrt(const type & a, type & out)
		{
			out = {byte(std::sqrt(a.x)), byte(std::sqrt(a.y)), byte(std::sqrt(a.z)), byte(std::sqrt(a.w))};
		}

		static inline inner sqrt(const type & a)
		{
			return __m32 {byte(std::sqrt(a.x)), byte(std::sqrt(a.y)), byte(std::sqrt(a.z)), byte(std::sqrt(a.w))};
		}

		static inline void bit_and(const type & a, const type & b, type & out)
		{
			out.i = a.i & b.i;
		}

		static inline inner bit_and(const type & a, const type & b)
		{
			type out;
			out.i = a.i & b.i;
			return out;
		}

		static inline void bit_or(const type & a, const type & b, type & out)
		{
			out.i = a.i | b.i;
		}

		static inline inner bit_or(const type & a, const type & b)
		{
			type out;
			out.i = a.i | b.i;
			return out;
		}

		static inline void bit_andnot(const type & a, const type & b, type & out)
		{
			out.i = a.i & ~b.i;
		}

		static inline inner bit_andnot(const type & a, const type & b)
		{
			type out;
			out.i = a.i & ~b.i;
			return out;
		}

		static inline void bit_xor(const type & a, const type & b, type & out)
		{
			out.i = a.i ^ b.i;
		}

		static inline inner bit_xor(const type & a, const type & b)
		{
			type out;
			out.i = a.i ^ b.i;
			return out;
		}

		static inline bool equal(const type & a, const type & b)
		{
			return a.i == b.i;
		}

		static inline bool notequal(const type & a, const type & b)
		{
			return a.i != b.i;
		}

		static inline void cmple(const type & a, const type & b, type & out)
		{
			out = {a.x > b.x ? byte(0x0) : byte(0xFF), a.y > b.y ? byte(0x0) : byte(0xFF), a.z > b.z ? byte(0x0) : byte(0xFF), a.w > b.w ? byte(0x0) : byte(0xFF)};
		}

		static inline inner cmple(const type & a, const type & b)
		{
			return __m32 {a.x > b.x ? byte(0x0) : byte(0xFF), a.y > b.y ? byte(0x0) : byte(0xFF), a.z > b.z ? byte(0x0) : byte(0xFF), a.w > b.w ? byte(0x0) : byte(0xFF)};
		}

		static inline void abs(const type & a, type & out)
		{
			out = a;
		}

		static inline inner abs(const type & a)
		{
			return a;
		}

		static inline void sign(const type & a, type & out)
		{
			out = {byte(0x1), byte(0x1), byte(0x1), byte(0x1)};
		}

		static inline inner sign(const type & a)
		{
			return __m32 {byte(0x1), byte(0x1), byte(0x1), byte(0x1)};
		}

		static inline void negate(const type & a, type & out)
		{
			out = a;
		}

		static inline inner negate(const type & a)
		{
			return a;
		}

		static inline void reverse(const type & a, type & out)
		{
			out = {a.z, a.w, a.y, a.x};
		}

		static inline inner reverse(const type & a)
		{
			return __m32 {a.z, a.w, a.y, a.x};
		}

		template<byte A, byte B, byte C, byte D, useif<
			(A < 2 && B < 2 && C < 2 && D < 2)
			>
		>
		static inline void blend(const type & a, const type & b, type & out)
		{
			out = {A == 0 ? a.x : b.x, B == 0 ? a.y : b.y, C == 0 ? a.z : b.z, D == 0 ? a.w : b.w};
		}

		template<byte A, byte B, byte C, byte D, useif<
			(A < 2 && B < 2 && C < 2 && D < 2)
			>
		>
		static inline inner blend(const type & a, const type & b)
		{
			return __m32 {A == 0 ? a.x : b.x, B == 0 ? a.y : b.y, C == 0 ? a.z : b.z, D == 0 ? a.w : b.w};
		}

		template<byte A, byte B, byte C, byte D, useif<
			(A < 4 && B < 4 && C < 4 && D < 4)
			>
		>
		static inline void shuffle2(const type & a, const type & b, type & out)
		{
			out = {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? b.x : C == 1 ? b.y : C == 2 ? b.z : b.w,
				D == 0 ? b.x : D == 1 ? b.y : D == 2 ? b.z : b.w
			};
		}

		template<byte A, byte B, byte C, byte D, useif<
			(A < 4 && B < 4 && C < 4 && D < 4)
			>
		>
		static inline inner shuffle2(const type & a, const type & b)
		{
			return __m32 {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? b.x : C == 1 ? b.y : C == 2 ? b.z : b.w,
				D == 0 ? b.x : D == 1 ? b.y : D == 2 ? b.z : b.w
			};
		}

		template<byte A, byte B, byte C, byte D, useif<
			(A < 4 && B < 4 && C < 4 && D < 4)
			>
		>
		static inline void shuffle(const type & a, type & out)
		{
			out = {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? a.x : C == 1 ? a.y : C == 2 ? a.z : a.w,
				D == 0 ? a.x : D == 1 ? a.y : D == 2 ? a.z : a.w
			};
		}

		template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
		static inline inner shuffle(const type & a)
		{
			return __m32 {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? a.x : C == 1 ? a.y : C == 2 ? a.z : a.w,
				D == 0 ? a.x : D == 1 ? a.y : D == 2 ? a.z : a.w
			};
		}
	};


	namespace internals
	{
		template<class T, template<class T> class Constant, bool ... Values>
		struct intrinsic_mask<T, Constant, std::integer_sequence<bool, Values...>>
		{
			static const int size = sizeof...(Values);
			typedef IntrinData<T, size> inner;

			template<byte Val>
			using cond = conditional_t<Val, Constant<T>, IntrinZero<T>>;

			static inline const inner & get()
			{
				static const inner v = Intrinsic<T, size>::load(cond<Values>::value...);
				return v;
			}
		};
	}
}

//---------------------------------------------------------------------------
#endif
