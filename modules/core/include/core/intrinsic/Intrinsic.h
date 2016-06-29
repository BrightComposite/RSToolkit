//---------------------------------------------------------------------------

#ifndef INTRINSIC_H
#define INTRINSIC_H

//---------------------------------------------------------------------------

#include "IntrinsicData.h"
#include "IntrinsicCvt.h"

#include <meta/Bitmask.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T, int N>
	struct Intrinsic
	{
		static const bool implemented = false;
	};

#ifndef _mm_permute_ps
#define _mm_permute_ps(a, imm8) _mm_shuffle_ps(a, a, imm8)
#endif // _mm_permute_ps

#define _mm_reverse_ps(a) _mm_permute_ps(a, reverse_shuffle_4)
#define _mm_reverse_pd(a) _mm_permute_pd(a, reverse_shuffle_2)
#define _mm256_reverse_pd(a) _mm256_permute4x64_pd(a, reverse_shuffle_4)

	namespace Internals
	{
		template<class T, template<class T> class Constant, class S>
		struct intrinsic_mask {};
	}

	template<class T, template<class T> class Constant, size_t Mask, int N = 4>
	struct IntrinsicMask
	{
		template<useif <Intrinsic<T, N>::implemented> endif>
		static inline const auto & get()
		{
			return Internals::intrinsic_mask<T, Constant, unfold_mask<Mask, N>>::get();
		}
	};

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

	intrinsic_constant(IntrinZero);
	intrinsic_constant(IntrinMax);
	intrinsic_constant(IntrinSignmask);
	intrinsic_constant(IntrinNofrac);

	template struct api(core) IntrinData<int, 4>;
	template struct api(core) IntrinData<float, 4>;
	template struct api(core) IntrinData<double, 4>;

	/**
	 *	Integer intrinsics
	 */
	template<>
	struct api(core) Intrinsic<int, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<int, 4>;
		using type = typename inner::type;

		static const inner maximum;
		static const inner signmask;
		static const size_t size = sizeof(inner);

		static inline void __vectorcall load(const inner & in, type & out)
		{
			out = _mm_load_si128(&in.v);
		}

		static inline inner __vectorcall load(const inner & in)
		{
			return {_mm_load_si128(&in.v)};
		}

		static inline void __vectorcall load(const int & a, const int & b, const int & c, const int & d, type & out)
		{
			out = _mm_set_epi32(a, b, c, d);
		}

		static inline inner __vectorcall load(const int & a, const int & b, const int & c, const int & d)
		{
			return {_mm_set_epi32(a, b, c, d)};
		}

		static inline void __vectorcall load(const int * data, type & out)
		{
			out = _mm_set_epi32(data[0], data[1], data[2], data[3]);
		}

		static inline inner __vectorcall load(const int * data)
		{
			return {_mm_set_epi32(data[0], data[1], data[2], data[3])};
		}

		static inline void __vectorcall store(const type & in, inner & out)
		{
			_mm_store_si128(&out.v, in);
		}

		static inline void __vectorcall zero(type & out)
		{
			out = _mm_setzero_si128();
		}

		static inline inner __vectorcall zero()
		{
			return {_mm_setzero_si128()};
		}

		static inline void __vectorcall fill(int val, type & out)
		{
			out = _mm_set1_epi32(val);
		}

		static inline void __vectorcall fill(int val, inner & out)
		{
			out = {_mm_set1_epi32(val)};
		}

		static inline inner __vectorcall fill(int val)
		{
			return {_mm_set1_epi32(val)};
		}

		static inline void __vectorcall add(const type & a, const type & b, type & out)
		{
			out = _mm_add_epi32(a, b);
		}

		static inline inner __vectorcall add(const type & a, const type & b)
		{
			return {_mm_add_epi32(a, b)};
		}

		static inline void __vectorcall sub(const type & a, const type & b, type & out)
		{
			out = _mm_sub_epi32(a, b);
		}

		static inline inner __vectorcall sub(const type & a, const type & b)
		{
			return {_mm_sub_epi32(a, b)};
		}

		static inline void __vectorcall mul(const type & a, const type & b, type & out)
		{
			out = _mm_mul_epi32(a, b);
		}

		static inline inner __vectorcall mul(const type & a, const type & b)
		{
			return {_mm_mul_epi32(a, b)};
		}

		static inline void __vectorcall div(const type & a, const type & b, type & out)
		{
			_mm_insert_epi32(out, _mm_extract_epi32(a, 3) / _mm_extract_epi32(b, 3), 3);
			_mm_insert_epi32(out, _mm_extract_epi32(a, 2) / _mm_extract_epi32(b, 2), 2);
			_mm_insert_epi32(out, _mm_extract_epi32(a, 1) / _mm_extract_epi32(b, 1), 1);
			_mm_insert_epi32(out, _mm_extract_epi32(a, 0) / _mm_extract_epi32(b, 0), 0);
		}

		static inline inner __vectorcall div(const type & a, const type & b)
		{
			return {_mm_set_epi32(
				_mm_extract_epi32(a, 3) / _mm_extract_epi32(b, 3),
				_mm_extract_epi32(a, 2) / _mm_extract_epi32(b, 2),
				_mm_extract_epi32(a, 1) / _mm_extract_epi32(b, 1),
				_mm_extract_epi32(a, 0) / _mm_extract_epi32(b, 0)
			)};
		}

		static inline void __vectorcall sqr(const type & a, type & out)
		{
			out = _mm_mul_epi32(a, a);
		}

		static inline inner __vectorcall sqr(const type & a)
		{
			return {_mm_mul_epi32(a, a)};
		}

		static inline void __vectorcall invert(const type & a, type & out)
		{
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 3), 3);
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 2), 2);
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 1), 1);
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 0), 0);
		}

		static inline inner __vectorcall invert(const type & a)
		{
			return {_mm_set_epi32(
				1 / _mm_extract_epi32(a, 3),
				1 / _mm_extract_epi32(a, 2),
				1 / _mm_extract_epi32(a, 1),
				1 / _mm_extract_epi32(a, 0)
				)};
		}

		static inline void __vectorcall min(const type & a, const type & b, type & out)
		{
			out = _mm_min_epi32(a, b);
		}

		static inline inner __vectorcall min(const type & a, const type & b)
		{
			return _mm_min_epi32(a, b);
		}

		static inline void __vectorcall max(const type & a, const type & b, type & out)
		{
			out = _mm_max_epi32(a, b);
		}

		static inline inner __vectorcall max(const type & a, const type & b)
		{
			return _mm_max_epi32(a, b);
		}

		static inline void __vectorcall hadd2(const type & a, const type & b, type & out)
		{
			out = _mm_hadd_epi32(a, b);
		}

		static inline inner __vectorcall hadd2(const type & a, const type & b)
		{
			return {_mm_hadd_epi32(a, b)};
		}

		static inline void __vectorcall hadd(const type & a, type & out)
		{
			out = _mm_hadd_epi32(a, _mm_setzero_si128());
		}

		static inline inner __vectorcall hadd(const type & a)
		{
			return {_mm_hadd_epi32(a, _mm_setzero_si128())};
		}

		static inline int __vectorcall sum(const type & a)
		{
			type v = _mm_hadd_epi32(a, a);
			v = _mm_hadd_epi32(v, v);
			return _mm_cvtsi128_si32(v);
		}

		static inline void __vectorcall fillsum(const type & a, type & out)
		{
			out = _mm_hadd_epi32(a, a);
			out = _mm_hadd_epi32(out, out);
		}

		static inline inner __vectorcall fillsum(const type & a)
		{
			type v = _mm_hadd_epi32(a, a);
			return _mm_hadd_epi32(v, v);
		}

		static inline void __vectorcall sqrt(const type & a, type & out)
		{
			out = _mm_cvtps_ph(_mm_sqrt_ps(_mm_cvtph_ps(a)), 0);
		}

		static inline inner __vectorcall sqrt(const type & a)
		{
			return {_mm_cvtps_ph(_mm_sqrt_ps(_mm_cvtph_ps(a)), 0)};
		}

		static inline void __vectorcall bit_and(const type & a, const type & b, type & out)
		{
			out = _mm_and_si128(a, b);
		}

		static inline inner __vectorcall bit_and(const type & a, const type & b)
		{
			return {_mm_and_si128(a, b)};
		}

		static inline void __vectorcall bit_or(const type & a, const type & b, type & out)
		{
			out = _mm_or_si128(a, b);
		}

		static inline inner __vectorcall bit_or(const type & a, const type & b)
		{
			return {_mm_or_si128(a, b)};
		}

		static inline void __vectorcall bit_andnot(const type & a, const type & b, type & out)
		{
			out = _mm_andnot_si128(a, b);
		}

		static inline inner __vectorcall bit_andnot(const type & a, const type & b)
		{
			return {_mm_andnot_si128(a, b)};
		}

		static inline void __vectorcall bit_xor(const type & a, const type & b, type & out)
		{
			out = _mm_xor_si128(a, b);
		}

		static inline inner __vectorcall bit_xor(const type & a, const type & b)
		{
			return {_mm_xor_si128(a, b)};
		}

		static inline bool __vectorcall equal(const type & a, const type & b)
		{
			return (_mm_movemask_epi8(_mm_cmpeq_epi32(a, b)) & 0x8888) == 0x8888;
		}

		static inline bool __vectorcall notequal(const type & a, const type & b)
		{
			return (_mm_movemask_epi8(_mm_cmpeq_epi32(a, b)) & 0x8888) != 0x8888;
		}

		static inline void __vectorcall cmple(const type & a, const type & b, type & out)
		{
			out = bit_xor(_mm_cmpgt_epi32(a, b), maximum);
		}

		static inline inner __vectorcall cmple(const type & a, const type & b)
		{
			return {bit_xor(_mm_cmpgt_epi32(a, b), maximum)};
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
			out = shuffle<3, 2, 1, 0>(a);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return {shuffle<3, 2, 1, 0>(a)};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)> endif>
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm_blend_epi32(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)> endif>
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return {_mm_blend_epi32(a, b, mk_mask4(A, B, C, D))};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = _mm_blend_epi32(
				_mm_shuffle_epi32(a, mk_shuffle_4(A, B, 0, 0)),
				_mm_shuffle_epi32(b, mk_shuffle_4(0, 0, C, D)),
				0x0C
				);
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return {_mm_blend_epi32(
				_mm_shuffle_epi32(a, mk_shuffle_4(A, B, 0, 0)),
				_mm_shuffle_epi32(b, mk_shuffle_4(0, 0, C, D)),
				0x0C
				)};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = _mm_shuffle_epi32(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		static inline inner __vectorcall shuffle(const type & a)
		{
			return {_mm_shuffle_epi32(a, mk_shuffle_4(A, B, C, D))};
		}
	};

	/**
	 *	Float intrinsics
	 */
	template<>
	struct api(core) Intrinsic<float, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<float, 4>;
		using type = inner::type;

		static const inner signmask;
		static const inner nofrac;
		static const size_t size = sizeof(inner);

		static inline void __vectorcall load(const inner & in, type & out)
		{
			out = _mm_load_ps(in);
		}

		static inline inner __vectorcall load(const inner & in)
		{
			return {_mm_load_ps(in)};
		}

		static inline void __vectorcall load(const float & a, const float & b, const float & c, const float & d, type & out)
		{
			out = _mm_set_ps(d, c, b, a);
		}

		static inline inner __vectorcall load(const float & a, const float & b, const float & c, const float & d)
		{
			return {_mm_set_ps(d, c, b, a)};
		}

		static inline inner __vectorcall load(const __m64 & a, const __m64 & b)
		{
			return {_mm_set_ps(_m_hi(b), _m_lo(b), _m_hi(a), _m_lo(a))};
		}

		static inline void __vectorcall load(const float * data, type & out)
		{
			out = _mm_set_ps(data[3], data[2], data[1], data[0]);
		}

		static inline inner __vectorcall load(const float * data)
		{
			return {_mm_set_ps(data[3], data[2], data[1], data[0])};
		}

		static inline void __vectorcall store(const type & in, inner & out)
		{
			_mm_store_ps(out, in);
		}

		static inline void __vectorcall zero(type & out)
		{
			out = _mm_setzero_ps();
		}

		static inline inner __vectorcall zero()
		{
			return {_mm_setzero_ps()};
		}

		static inline void __vectorcall fill(float val, type & out)
		{
			out = _mm_set_ps1(val);
		}

		static inline inner __vectorcall fill(float val)
		{
			return {_mm_set_ps1(val)};
		}

		static inline void __vectorcall add(const type & a, const type & b, type & out)
		{
			out = _mm_add_ps(a, b);
		}

		static inline inner __vectorcall add(const type & a, const type & b)
		{
			return {_mm_add_ps(a, b)};
		}

		static inline void __vectorcall addx(const type & a, const type & b, type & out)
		{
			out = _mm_add_ss(a, b);
		}

		static inline inner __vectorcall addx(const type & a, const type & b)
		{
			return {_mm_add_ss(a, b)};
		}

		static inline void __vectorcall sub(const type & a, const type & b, type & out)
		{
			out = _mm_sub_ps(a, b);
		}

		static inline inner __vectorcall sub(const type & a, const type & b)
		{
			return {_mm_sub_ps(a, b)};
		}

		static inline void __vectorcall mul(const type & a, const type & b, type & out)
		{
			out = _mm_mul_ps(a, b);
		}

		static inline inner __vectorcall mul(const type & a, const type & b)
		{
			return {_mm_mul_ps(a, b)};
		}

		static inline void __vectorcall div(const type & a, const type & b, type & out)
		{
			out = _mm_div_ps(a, b);
		}

		static inline inner __vectorcall div(const type & a, const type & b)
		{
			return {_mm_div_ps(a, b)};
		}

		static inline void __vectorcall sqr(const type & a, type & out)
		{
			out = _mm_mul_ps(a, a);
		}

		static inline inner __vectorcall sqr(const type & a)
		{
			return {_mm_mul_ps(a, a)};
		}

		static inline void __vectorcall invert(const type & a, type & out)
		{
			type tmp = _mm_rcp_ps(a);
			out = sub(add(tmp, tmp), mul(a, mul(tmp, tmp)));
		}

		static inline inner __vectorcall invert(const type & a)
		{
			type tmp = _mm_rcp_ps(a);
			return sub(add(tmp, tmp), mul(a, mul(tmp, tmp)));
		}

		static inline void __vectorcall min(const type & a, const type & b, type & out)
		{
			out = _mm_min_ps(a, b);
		}

		static inline inner __vectorcall min(const type & a, const type & b)
		{
			return _mm_min_ps(a, b);
		}

		static inline void __vectorcall max(const type & a, const type & b, type & out)
		{
			out = _mm_max_ps(a, b);
		}

		static inline inner __vectorcall max(const type & a, const type & b)
		{
			return _mm_max_ps(a, b);
		}

		static inline void __vectorcall hadd2(const type & a, const type & b, type & out)
		{
			out = _mm_hadd_ps(a, b);
		}

		static inline inner __vectorcall hadd2(const type & a, const type & b)
		{
			return {_mm_hadd_ps(a, b)};
		}

		static inline void __vectorcall hadd(const type & a, type & out)
		{
			out = _mm_hadd_ps(a, a);
		}

		static inline inner __vectorcall hadd(const type & a)
		{
			return {_mm_hadd_ps(a, a)};
		}

		static inline float __vectorcall sum(const type & a)
		{
			type v = _mm_hadd_ps(a, a);
			v = _mm_hadd_ps(v, v);
			return _mm_cvtss_f32(v);
		}

		static inline void __vectorcall fillsum(const type & a, type & out)
		{
			out = _mm_hadd_ps(a, a);
			out = _mm_hadd_ps(out, out);
		}

		static inline inner __vectorcall fillsum(const type & a)
		{
			type v = _mm_hadd_ps(a, a);
			return _mm_hadd_ps(v, v);
		}

		static inline void __vectorcall sqrt(const type & a, type & out)
		{
			out = _mm_sqrt_ps(a);
		}

		static inline inner __vectorcall sqrt(const type & a)
		{
			return {_mm_sqrt_ps(a)};
		}

		static inline void __vectorcall bit_and(const type & a, const type & b, type & out)
		{
			out = _mm_and_ps(a, b);
		}

		static inline inner __vectorcall bit_and(const type & a, const type & b)
		{
			return {_mm_and_ps(a, b)};
		}

		static inline void __vectorcall bit_or(const type & a, const type & b, type & out)
		{
			out = _mm_or_ps(a, b);
		}

		static inline inner __vectorcall bit_or(const type & a, const type & b)
		{
			return {_mm_or_ps(a, b)};
		}

		static inline void __vectorcall bit_andnot(const type & a, const type & b, type & out)
		{
			out = _mm_andnot_ps(a, b);
		}

		static inline inner __vectorcall bit_andnot(const type & a, const type & b)
		{
			return {_mm_andnot_ps(a, b)};
		}

		static inline void __vectorcall bit_xor(const type & a, const type & b, type & out)
		{
			out = _mm_xor_ps(a, b);
		}

		static inline inner __vectorcall bit_xor(const type & a, const type & b)
		{
			return {_mm_xor_ps(a, b)};
		}

		static inline bool __vectorcall equal(const type & a, const type & b)
		{
			return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) == 0xf;
		}

		static inline bool __vectorcall notequal(const type & a, const type & b)
		{
			return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) != 0xf;
		}

		static inline void __vectorcall cmple(const type & a, const type & b, type & out)
		{
			out = _mm_cmple_ps(a, b);
		}

		static inline inner __vectorcall cmple(const type & a, const type & b)
		{
			return {_mm_cmple_ps(a, b)};
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
			auto v = bit_or (nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			out = bit_xor(bit_and (sub(add(a, v), v), mask), bit_andnot(mask, a));
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
			out = _mm_reverse_ps(a);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return {_mm_reverse_ps(a)};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 2 && B < 2 && C < 2 && D < 2)
			> endif
		>
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm_blend_ps(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 2 && B < 2 && C < 2 && D < 2)
			> endif
		>
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return _mm_blend_ps(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			> endif
		>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = _mm_shuffle_ps(a, b, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			> endif
		>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return {_mm_shuffle_ps(a, b, mk_shuffle_4(A, B, C, D))};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			> endif
		>
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = _mm_permute_ps(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			> endif
		>
		static inline inner __vectorcall shuffle(const type & a)
		{
			return {_mm_permute_ps(a, mk_shuffle_4(A, B, C, D))};
		}
	};

#ifdef USE_AVX

	/**
	 *	Double intrinsics
	 */
	template<>
	struct api(core) Intrinsic<double, 4>
	{
		static const bool implemented = true;

		using inner = IntrinData<double, 4>;
		using type = inner::type;

		static const inner signmask;
		static const inner nofrac;
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

		static inline void __vectorcall fillsum(const type & a, type & out)
		{
			out = _mm256_hadd_pd(shuffle<0, 1, 0, 1>(a), shuffle<2, 3, 2, 3>(a));
			out = _mm256_hadd_pd(shuffle<0, 1, 0, 1>(out), shuffle<2, 3, 2, 3>(out));
		}

		static inline inner __vectorcall fillsum(const type & a)
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

		template<byte A, byte B, byte C, byte D, useif <
            (A < 2 && B < 2 && C < 2 && D < 2)
            > endif
        >
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm256_blend_pd(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
            (A < 2 && B < 2 && C < 2 && D < 2)
            > endif
        >
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return _mm256_blend_pd(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
            (A < 4 && B < 4 && C < 4 && D < 4)
            > endif
        >
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = blend<0, 0, 1, 1>(_mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D)), _mm256_permute4x64_pd(b, mk_shuffle_4(A, B, C, D)));
		}

		template<byte A, byte B, byte C, byte D, useif <
            (A < 4 && B < 4 && C < 4 && D < 4)
            > endif
        >
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return blend<0, 0, 1, 1>(_mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D)), _mm256_permute4x64_pd(b, mk_shuffle_4(A, B, C, D)));
		}

		template<byte A, byte B, byte C, byte D, useif <
            (A < 4 && B < 4 && C < 4 && D < 4)
            > endif
        >
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = _mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <
            (A < 4 && B < 4 && C < 4 && D < 4)
            > endif
        >
		static inline inner __vectorcall shuffle(const type & a)
		{
			return _mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D));
		}
	};

#else

	/**
	 *	Double intrinsics
	 */
	template<>
	struct api(core) Intrinsic<double, 4>
	{
		static const bool implemented = true;

		typedef IntrinData<double, 4> inner;
		typedef inner::type type;
		static const size_t size = sizeof(inner);
		static const inner signmask;

		static inline void __vectorcall load(const inner & in, type & out)
		{
			out[0] = _mm_load_pd(in[0]);
			out[1] = _mm_load_pd(in[1]);
		}

		static inline inner __vectorcall load(const inner & in)
		{
			return {{_mm_load_pd(in[0]), _mm_load_pd(in[1])}};
		}

		static inline void __vectorcall load(const double & a, const double & b, const double & c, const double & d, type & out)
		{
			out[0] = _mm_set_pd(d, c);
			out[1] = _mm_set_pd(b, a);
		}

		static inline inner __vectorcall load(const double & a, const double & b, const double & c, const double & d)
		{
			return {{_mm_set_pd(d, c), _mm_set_pd(b, a)}};
		}

		static inline void __vectorcall load(const double * data, type & out)
		{
			out[0] = _mm_set_pd(data[1], data[0]);
			out[1] = _mm_set_pd(data[3], data[2]);
		}

		static inline inner __vectorcall load(const double * data)
		{
			return {{_mm_set_pd(data[1], data[0]), _mm_set_pd(data[3], data[2])}};
		}

		static inline void __vectorcall store(const type & in, inner & out)
		{
			_mm_store_pd(out[0], in[0]);
			_mm_store_pd(out[1], in[1]);
		}

		static inline void __vectorcall zero(type & out)
		{
			out[0] = _mm_setzero_pd();
			out[1] = _mm_setzero_pd();
		}

		static inline inner __vectorcall zero()
		{
			return {{_mm_setzero_pd(), _mm_setzero_pd()}};
		}

		static inline void __vectorcall fill(double val, type & out)
		{
			out[0] = _mm_set1_pd(val);
			out[1] = _mm_set1_pd(val);
		}

		static inline inner __vectorcall fill(double val)
		{
			return {{_mm_set1_pd(val), _mm_set1_pd(val)}};
		}

		static inline void __vectorcall add(const type & a, const type & b, type & out)
		{
			out[0] = _mm_add_pd(a[0], b[0]);
			out[1] = _mm_add_pd(a[1], b[1]);
		}

		static inline inner __vectorcall add(const type & a, const type & b)
		{
			return {{_mm_add_pd(a[0], b[0]), _mm_add_pd(a[1], b[1])}};
		}

		static inline void __vectorcall sub(const type & a, const type & b, type & out)
		{
			out[0] = _mm_sub_pd(a[0], b[0]);
			out[1] = _mm_sub_pd(a[1], b[1]);
		}

		static inline inner __vectorcall sub(const type & a, const type & b)
		{
			return {{_mm_sub_pd(a[0], b[0]), _mm_sub_pd(a[1], b[1])}};
		}

		static inline void __vectorcall mul(const type & a, const type & b, type & out)
		{
			out[0] = _mm_mul_pd(a[0], b[0]);
			out[1] = _mm_mul_pd(a[1], b[1]);
		}

		static inline inner __vectorcall mul(const type & a, const type & b)
		{
			return {{_mm_mul_pd(a[0], b[0]), _mm_mul_pd(a[1], b[1])}};
		}

		static inline void __vectorcall div(const type & a, const type & b, type & out)
		{
			out[0] = _mm_div_pd(a[0], b[0]);
			out[1] = _mm_div_pd(a[1], b[1]);
		}

		static inline inner __vectorcall div(const type & a, const type & b)
		{
			return {{_mm_div_pd(a[0], b[0]), _mm_div_pd(a[1], b[1])}};
		}

		static inline void __vectorcall hadd2(const type & a, const type & b, type & out)
		{
			out[0] = _mm_hadd_pd(a[0], a[1]);
			out[1] = _mm_hadd_pd(b[0], b[1]);
		}

		static inline inner __vectorcall hadd2(const type & a, const type & b)
		{
			return {{
					_mm_hadd_pd(a[0], a[1]),
					_mm_hadd_pd(b[0], b[1])
				}};
		}

		static inline void __vectorcall hadd(const type & a, type & out)
		{
			out[0] = _mm_hadd_pd(a[0], a[1]);
			out[1] = _mm_setzero_pd();
		}

		static inline inner __vectorcall hadd(const type & a)
		{
			return {{
					_mm_hadd_pd(a[0], a[1]),
					_mm_setzero_pd()
				}};
		}

		static inline void __vectorcall sqrt(const type & a, type & out)
		{
			out[0] = _mm_sqrt_pd(a[0]);
			out[1] = _mm_sqrt_pd(a[1]);
		}

		static inline inner __vectorcall sqrt(const type & a)
		{
			return {{_mm_sqrt_pd(a[0]), _mm_sqrt_pd(a[1])}};
		}

		static inline void __vectorcall abs(const type & a, type & out)
		{
			out[0] = _mm_andnot_pd(signmask[0], a[0]);
			out[1] = _mm_andnot_pd(signmask[1], a[1]);
		}

		static inline inner __vectorcall abs(const type & a)
		{
			return {{
					_mm_andnot_pd(signmask[0], a[0]),
					_mm_andnot_pd(signmask[1], a[1])
				}};
		}

		static inline void __vectorcall negate(const type & a, type & out)
		{
			out[0] = _mm_xor_pd(signmask[0], a[0]);
			out[1] = _mm_xor_pd(signmask[1], a[1]);
		}

		static inline inner __vectorcall negate(const type & a)
		{
			return {{
					_mm_xor_pd(signmask[0], a[0]),
					_mm_xor_pd(signmask[1], a[1])
				}};
		}

		static inline void __vectorcall reverse(const type & a, type & out)
		{
			out[0] = _mm_reverse_pd(a[1]);
			out[1] = _mm_reverse_pd(a[0]);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return {{
					_mm_reverse_pd(a[1]),
					_mm_reverse_pd(a[0])
				}};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out[0] = _mm_shuffle_pd(a[A / 2], a[B / 2], mk_shuffle_2(A % 2, B % 2));
			out[1] = _mm_shuffle_pd(b[C / 2], b[D / 2], mk_shuffle_2(C % 2, D % 2));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return {{
					_mm_shuffle_pd(a[A / 2], a[B / 2], mk_shuffle_2(A % 2, B % 2)),
					_mm_shuffle_pd(b[C / 2], b[D / 2], mk_shuffle_2(C % 2, D % 2))
				}};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out[0] = _mm_shuffle_pd(a[A / 2], a[B / 2], mk_shuffle_2(A % 2, B % 2));
			out[1] = _mm_shuffle_pd(a[C / 2], a[D / 2], mk_shuffle_2(C % 2, D % 2));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline inner __vectorcall shuffle(const type & a)
		{
			return {{
					_mm_shuffle_pd(a[A / 2], a[B / 2], mk_shuffle_2(A % 2, B % 2)),
					_mm_shuffle_pd(a[C / 2], a[D / 2], mk_shuffle_2(C % 2, D % 2))
				}};
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

		static const inner maximum;
		static const inner signmask;
		static const size_t size = sizeof(inner);

		static inline void __vectorcall load(const inner & in, type & out)
		{
			out = in.v;
		}

		static inline inner __vectorcall load(const inner & in)
		{
			return in.v;
		}

		static inline void __vectorcall load(const byte & a, const byte & b, const byte & c, const byte & d, type & out)
		{
			out = {d, c, b, a};
		}

		static inline inner __vectorcall load(const byte & a, const byte & b, const byte & c, const byte & d)
		{
			return __m32 {d, c, b, a};
		}

		static inline void __vectorcall load(const byte * data, type & out)
		{
			out = {data[3], data[2], data[1], data[0]};
		}

		static inline inner __vectorcall load(const byte * data)
		{
			return __m32 {data[3], data[2], data[1], data[0]};
		}

		static inline void __vectorcall store(const type & in, inner & out)
		{
			out = in;
		}

		static inline void __vectorcall zero(type & out)
		{
			out.i = 0;
		}

		static inline inner __vectorcall zero()
		{
			return __m32{};
		}

		static inline void __vectorcall fill(byte val, type & out)
		{
			out = {val, val, val, val};
		}

		static inline void __vectorcall fill(byte val, inner & out)
		{
			out = {val, val, val, val};
		}

		static inline inner __vectorcall fill(byte val)
		{
			return __m32 {val, val, val, val};
		}

		static inline void __vectorcall add(const type & a, const type & b, type & out)
		{
			out = {byte(a.x + b.x), byte(a.y + b.y), byte(a.z + b.z), byte(a.w + b.w)};
		}

		static inline inner __vectorcall add(const type & a, const type & b)
		{
			return __m32 {byte(a.x + b.x), byte(a.y + b.y), byte(a.z + b.z), byte(a.w + b.w)};
		}

		static inline void __vectorcall sub(const type & a, const type & b, type & out)
		{
			out = {byte(a.x - b.x), byte(a.y - b.y), byte(a.z - b.z), byte(a.w - b.w)};
		}

		static inline inner __vectorcall sub(const type & a, const type & b)
		{
			return __m32 {byte(a.x - b.x), byte(a.y - b.y), byte(a.z - b.z), byte(a.w - b.w)};
		}

		static inline void __vectorcall mul(const type & a, const type & b, type & out)
		{
			out = {byte(a.x * b.x), byte(a.y * b.y), byte(a.z * b.z), byte(a.w * b.w)};
		}

		static inline inner __vectorcall mul(const type & a, const type & b)
		{
			return __m32 {byte(a.x * b.x), byte(a.y * b.y), byte(a.z * b.z), byte(a.w * b.w)};
		}

		static inline void __vectorcall div(const type & a, const type & b, type & out)
		{
			out = {byte(a.x / b.x), byte(a.y / b.y), byte(a.z / b.z), byte(a.w / b.w)};
		}

		static inline inner __vectorcall div(const type & a, const type & b)
		{
			return __m32 {byte(a.x / b.x), byte(a.y / b.y), byte(a.z / b.z), byte(a.w / b.w)};
		}

		static inline void __vectorcall sqr(const type & a, type & out)
		{
			out = {byte(a.x * a.x), byte(a.y * a.y), byte(a.z * a.z), byte(a.w * a.w)};
		}

		static inline inner __vectorcall sqr(const type & a)
		{
			return __m32 {byte(a.x * a.x), byte(a.y * a.y), byte(a.z * a.z), byte(a.w * a.w)};
		}

		static inline void __vectorcall invert(const type & a, type & out)
		{
			out.i = 0;
		}

		static inline inner __vectorcall invert(const type & a)
		{
			return __m32 {};
		}

		static inline void __vectorcall min(const type & a, const type & b, type & out)
		{
			out = {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
		}

		static inline inner __vectorcall min(const type & a, const type & b)
		{
			return __m32 {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
		}

		static inline void __vectorcall max(const type & a, const type & b, type & out)
		{
			out = {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
		}

		static inline inner __vectorcall max(const type & a, const type & b)
		{
			return __m32 {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
		}

		static inline void __vectorcall hadd2(const type & a, const type & b, type & out)
		{
			out = {byte(a.x + a.y), byte(a.z + a.w), byte(b.x + b.y), byte(b.z + b.w)};
		}

		static inline inner __vectorcall hadd2(const type & a, const type & b)
		{
			return __m32 {byte(a.x + a.y), byte(a.z + a.w), byte(b.x + b.y), byte(b.z + b.w)};
		}

		static inline void __vectorcall hadd(const type & a, type & out)
		{
			out = {byte(a.x + a.y), byte(a.z + a.w), 0, 0};
		}

		static inline inner __vectorcall hadd(const type & a)
		{
			return __m32 {byte(a.x + a.y), byte(a.z + a.w), 0, 0};
		}

		static inline byte __vectorcall sum(const type & a)
		{
			return byte(a.x + a.y + a.z + a.w);
		}

		static inline void __vectorcall fillsum(const type & a, type & out)
		{
			byte b = a.x + a.y + a.z + a.w;
			out = {b, b, b, b};
		}

		static inline inner __vectorcall fillsum(const type & a)
		{
			byte b = a.x + a.y + a.z + a.w;
			return __m32 {b, b, b, b};
		}

		static inline void __vectorcall sqrt(const type & a, type & out)
		{
			out = {byte(std::sqrt(a.x)), byte(std::sqrt(a.y)), byte(std::sqrt(a.z)), byte(std::sqrt(a.w))};
		}

		static inline inner __vectorcall sqrt(const type & a)
		{
			return __m32 {byte(std::sqrt(a.x)), byte(std::sqrt(a.y)), byte(std::sqrt(a.z)), byte(std::sqrt(a.w))};
		}

		static inline void __vectorcall bit_and(const type & a, const type & b, type & out)
		{
			out.i = a.i & b.i;
		}

		static inline inner __vectorcall bit_and(const type & a, const type & b)
		{
			type out;
			out.i = a.i & b.i;
			return out;
		}

		static inline void __vectorcall bit_or(const type & a, const type & b, type & out)
		{
			out.i = a.i | b.i;
		}

		static inline inner __vectorcall bit_or(const type & a, const type & b)
		{
			type out;
			out.i = a.i | b.i;
			return out;
		}

		static inline void __vectorcall bit_andnot(const type & a, const type & b, type & out)
		{
			out.i = a.i & ~b.i;
		}

		static inline inner __vectorcall bit_andnot(const type & a, const type & b)
		{
			type out;
			out.i = a.i & ~b.i;
			return out;
		}

		static inline void __vectorcall bit_xor(const type & a, const type & b, type & out)
		{
			out.i = a.i ^ b.i;
		}

		static inline inner __vectorcall bit_xor(const type & a, const type & b)
		{
			type out;
			out.i = a.i ^ b.i;
			return out;
		}

		static inline bool __vectorcall equal(const type & a, const type & b)
		{
			return a.i == b.i;
		}

		static inline bool __vectorcall notequal(const type & a, const type & b)
		{
			return a.i != b.i;
		}

		static inline void __vectorcall cmple(const type & a, const type & b, type & out)
		{
			out = {a.x > b.x ? byte(0x0) : byte(0xFF), a.y > b.y ? byte(0x0) : byte(0xFF), a.z > b.z ? byte(0x0) : byte(0xFF), a.w > b.w ? byte(0x0) : byte(0xFF)};
		}

		static inline inner __vectorcall cmple(const type & a, const type & b)
		{
			return __m32 {a.x > b.x ? byte(0x0) : byte(0xFF), a.y > b.y ? byte(0x0) : byte(0xFF), a.z > b.z ? byte(0x0) : byte(0xFF), a.w > b.w ? byte(0x0) : byte(0xFF)};
		}

		static inline void __vectorcall abs(const type & a, type & out)
		{
			out = a;
		}

		static inline inner __vectorcall abs(const type & a)
		{
			return a;
		}

		static inline void __vectorcall sign(const type & a, type & out)
		{
			out = {byte(0x1), byte(0x1), byte(0x1), byte(0x1)};
		}

		static inline inner __vectorcall sign(const type & a)
		{
			return __m32 {byte(0x1), byte(0x1), byte(0x1), byte(0x1)};
		}

		static inline void __vectorcall negate(const type & a, type & out)
		{
			out = a;
		}

		static inline inner __vectorcall negate(const type & a)
		{
			return a;
		}

		static inline void __vectorcall reverse(const type & a, type & out)
		{
			out = {a.z, a.w, a.y, a.x};
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return __m32 {a.z, a.w, a.y, a.x};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 2 && B < 2 && C < 2 && D < 2)
			> endif
		>
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = {A == 0 ? a.x : b.x, B == 0 ? a.y : b.y, C == 0 ? a.z : b.z, D == 0 ? a.w : b.w};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 2 && B < 2 && C < 2 && D < 2)
			> endif
		>
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return __m32 {A == 0 ? a.x : b.x, B == 0 ? a.y : b.y, C == 0 ? a.z : b.z, D == 0 ? a.w : b.w};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			>endif
		>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? b.x : C == 1 ? b.y : C == 2 ? b.z : b.w,
				D == 0 ? b.x : D == 1 ? b.y : D == 2 ? b.z : b.w
			};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			> endif
		>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return __m32 {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? b.x : C == 1 ? b.y : C == 2 ? b.z : b.w,
				D == 0 ? b.x : D == 1 ? b.y : D == 2 ? b.z : b.w
			};
		}

		template<byte A, byte B, byte C, byte D, useif <
			(A < 4 && B < 4 && C < 4 && D < 4)
			>endif
		>
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? a.x : C == 1 ? a.y : C == 2 ? a.z : a.w,
				D == 0 ? a.x : D == 1 ? a.y : D == 2 ? a.z : a.w
			};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		static inline inner __vectorcall shuffle(const type & a)
		{
			return __m32 {
				A == 0 ? a.x : A == 1 ? a.y : A == 2 ? a.z : a.w,
				B == 0 ? a.x : B == 1 ? a.y : B == 2 ? a.z : a.w,
				C == 0 ? a.x : C == 1 ? a.y : C == 2 ? a.z : a.w,
				D == 0 ? a.x : D == 1 ? a.y : D == 2 ? a.z : a.w
			};
		}
	};


	namespace Internals
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
