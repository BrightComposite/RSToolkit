//---------------------------------------------------------------------------

#ifndef INTRINSIC_H
#define INTRINSIC_H

//---------------------------------------------------------------------------

#include "IntrinsicData.h"
#include "IntrinsicCvt.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T, size_t N>
	struct Intrinsic
	{
		static const bool implemented = false;
	};

#define _mm_permute_ps(a, imm8) _mm_shuffle_ps(a, a, imm8)

#define _mm_reverse_ps(a) _mm_permute_ps(a, reverse_shuffle_4)
#define _mm_reverse_pd(a) _mm_permute_pd(a, reverse_shuffle_2)
#define _mm256_reverse_pd(a) _mm256_permute4x64_pd(a, reverse_shuffle_4)

	namespace Internals
	{
		template<class T, template<class T> class Constant, class S>
		struct intrinsic_mask0 {};
	}

	template<class T, template<class T> class Constant, size_t Mask, int N = 4>
	struct IntrinsicMask
	{
		template<useif <Intrinsic<T, N>::implemented> endif>
		static inline const auto & get()
		{
			return Internals::intrinsic_mask0<T, Constant, unfold_mask<Mask, N>>::get();
		}
	};

	template_constant(IntrinZero);
	template_constant(IntrinMax);
	template_constant(IntrinSignmask);
	template_constant(IntrinNofrac);

	/**
	 *	Integer intrinsics
	 */
	template<>
	struct Intrinsic<int, 4>
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
			out = _mm_set_epi32(d, c, b, a);
		}

		static inline inner __vectorcall load(const int & a, const int & b, const int & c, const int & d)
		{
			return {_mm_set_epi32(d, c, b, a)};
		}

		static inline void __vectorcall load(const int * data, type & out)
		{
			out = _mm_set_epi32(data[3], data[2], data[1], data[0]);
		}

		static inline inner __vectorcall load(const int * data)
		{
			return {_mm_set_epi32(data[3], data[2], data[1], data[0])};
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
			_mm_insert_epi32(out, _mm_extract_epi32(a, 0) / _mm_extract_epi32(b, 0), 0);
			_mm_insert_epi32(out, _mm_extract_epi32(a, 1) / _mm_extract_epi32(b, 1), 1);
			_mm_insert_epi32(out, _mm_extract_epi32(a, 2) / _mm_extract_epi32(b, 2), 2);
			_mm_insert_epi32(out, _mm_extract_epi32(a, 3) / _mm_extract_epi32(b, 3), 3);
		}

		static inline inner __vectorcall div(const type & a, const type & b)
		{
			return {_mm_set_epi32(
				_mm_extract_epi32(a, 0) / _mm_extract_epi32(b, 0),
				_mm_extract_epi32(a, 1) / _mm_extract_epi32(b, 1),
				_mm_extract_epi32(a, 2) / _mm_extract_epi32(b, 2),
				_mm_extract_epi32(a, 3) / _mm_extract_epi32(b, 3)
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
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 0), 0);
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 1), 1);
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 2), 2);
			_mm_insert_epi32(out, 1 / _mm_extract_epi32(a, 3), 3);
		}

		static inline inner __vectorcall invert(const type & a)
		{
			return {_mm_set_epi32(
				1 / _mm_extract_epi32(a, 0),
				1 / _mm_extract_epi32(a, 1),
				1 / _mm_extract_epi32(a, 2),
				1 / _mm_extract_epi32(a, 3)
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
			return v.m128i_i32[0] + v.m128i_i32[1];
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

		static inline void __vectorcall and (const type & a, const type & b, type & out)
		{
			out = _mm_and_si128(a, b);
		}

		static inline inner __vectorcall and (const type & a, const type & b)
		{
			return {_mm_and_si128(a, b)};
		}

		static inline void __vectorcall or (const type & a, const type & b, type & out)
		{
			out = _mm_or_si128(a, b);
		}

		static inline inner __vectorcall or (const type & a, const type & b)
		{
			return {_mm_or_si128(a, b)};
		}

		static inline void __vectorcall andnot(const type & a, const type & b, type & out)
		{
			out = _mm_andnot_si128(a, b);
		}

		static inline inner __vectorcall andnot(const type & a, const type & b)
		{
			return {_mm_andnot_si128(a, b)};
		}

		static inline void __vectorcall xor (const type & a, const type & b, type & out)
		{
			out = _mm_xor_si128(a, b);
		}

		static inline inner __vectorcall xor (const type & a, const type & b)
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
			out = xor(_mm_cmpgt_epi32(a, b), maximum);
		}

		static inline inner __vectorcall cmple(const type & a, const type & b)
		{
			return {xor(_mm_cmpgt_epi32(a, b), maximum)};
		}

		static inline void __vectorcall abs(const type & a, type & out)
		{
			out = andnot(signmask, a);
		}

		static inline inner __vectorcall abs(const type & a)
		{
			return {andnot(signmask, a)};
		}

		static inline void __vectorcall sign(const type & a, type & out)
		{
			out = and(signmask, a);
		}

		static inline inner __vectorcall sign(const type & a)
		{
			return {and(signmask, a)};
		}

		static inline void __vectorcall negate(const type & a, type & out)
		{
			out = xor(signmask, a);
		}

		static inline inner __vectorcall negate(const type & a)
		{
			return {xor(signmask, a)};
		}

		static inline void __vectorcall reverse(const type & a, type & out)
		{
			out = shuffle<3, 2, 1, 0>(a);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return {shuffle<3, 2, 1, 0>(a)};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)>
			endif>
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm_blend_epi32(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)>
			endif>
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return {_mm_blend_epi32(a, b, mk_mask4(A, B, C, D))};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = _mm_blend_epi32(
				_mm_shuffle_epi32(a, mk_shuffle_4(A, B, 0, 0)),
				_mm_shuffle_epi32(b, mk_shuffle_4(0, 0, C, D)),
				0x0C
				);
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return {_mm_blend_epi32(
				_mm_shuffle_epi32(a, mk_shuffle_4(A, B, 0, 0)),
				_mm_shuffle_epi32(b, mk_shuffle_4(0, 0, C, D)),
				0x0C
				)};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
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
	struct Intrinsic<float, 4>
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

		static inline void __vectorcall load(const __m64 & a, const __m64 & b, type & out)
		{
			_mm_lo(out) = a;
			_mm_hi(out) = b;
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
			return v.m128_f32[0] + v.m128_f32[1];
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

		static inline void __vectorcall and(const type & a, const type & b, type & out)
		{
			out = _mm_and_ps(a, b);
		}

		static inline inner __vectorcall and(const type & a, const type & b)
		{
			return {_mm_and_ps(a, b)};
		}

		static inline void __vectorcall or(const type & a, const type & b, type & out)
		{
			out = _mm_or_ps(a, b);
		}

		static inline inner __vectorcall or(const type & a, const type & b)
		{
			return {_mm_or_ps(a, b)};
		}

		static inline void __vectorcall andnot(const type & a, const type & b, type & out)
		{
			out = _mm_andnot_ps(a, b);
		}

		static inline inner __vectorcall andnot(const type & a, const type & b)
		{
			return {_mm_andnot_ps(a, b)};
		}

		static inline void __vectorcall xor(const type & a, const type & b, type & out)
		{
			out = _mm_xor_ps(a, b);
		}

		static inline inner __vectorcall xor(const type & a, const type & b)
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
			out = andnot(signmask, a);
		}

		static inline inner __vectorcall abs(const type & a)
		{
			return {andnot(signmask, a)};
		}

		static inline void __vectorcall sign(const type & a, type & out)
		{
			out = and(signmask, a);
		}

		static inline inner __vectorcall sign(const type & a)
		{
			return {and(signmask, a)};
		}

		static inline void __vectorcall round(const type & a, type & out)
		{
			auto v = or (nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			out = xor(and (sub(add(a, v), v), mask), andnot(mask, a));
		}

		static inline inner __vectorcall round(const type & a)
		{
			auto v = or(nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			return {xor(and(sub(add(a, v), v), mask), andnot(mask, a))};
		}

		static inline void __vectorcall negate(const type & a, type & out)
		{
			out = xor(signmask, a);
		}

		static inline inner __vectorcall negate(const type & a)
		{
			return {xor(signmask, a)};
		}

		static inline void __vectorcall reverse(const type & a, type & out)
		{
			out = _mm_reverse_ps(a);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return {_mm_reverse_ps(a)};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)>
			endif>
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm_blend_ps(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)>
			endif>
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return _mm_blend_ps(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = _mm_shuffle_ps(a, b, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return {_mm_shuffle_ps(a, b, mk_shuffle_4(A, B, C, D))};
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = _mm_permute_ps(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
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
	struct Intrinsic<double, 4>
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
			out = _mm256_set_m128d(b, a);
		}

		static inline inner __vectorcall load(const __m128d & a, const __m128d & b)
		{
			return _mm256_set_m128d(b, a);
		}

		static inline void __vectorcall load(const double * data, type & out)
		{
			out = _mm256_set_pd(data[3], data[2], data[1], data[0]);
		}

		static inline inner __vectorcall load(const double * data)
		{
			return _mm256_set_pd(data[3], data[2], data[1], data[0]);
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

		static inline void __vectorcall addx(const type & a, const type & b, type & out)
		{
			out = _mm256_set_m128d(_mm256_hi(a), _mm_add_sd(_mm256_lo(a), _mm256_lo(b)));
		}

		static inline inner __vectorcall addx(const type & a, const type & b)
		{
			return {_mm256_set_m128d(_mm256_hi(a), _mm_add_sd(_mm256_lo(a), _mm256_lo(b)))};
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
			out = _mm256_div_pd({1.0, 1.0, 1.0, 1.0}, a);
		}

		static inline inner __vectorcall invert(const type & a)
		{
			return _mm256_div_pd({1.0, 1.0, 1.0, 1.0}, a);
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
			return v.m256d_f64[0] + v.m256d_f64[1];
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

		static inline void __vectorcall and(const type & a, const type & b, type & out)
		{
			out = _mm256_and_pd(a, b);
		}

		static inline inner __vectorcall and(const type & a, const type & b)
		{
			return {_mm256_and_pd(a, b)};
		}

		static inline void __vectorcall or(const type & a, const type & b, type & out)
		{
			out = _mm256_or_pd(a, b);
		}

		static inline inner __vectorcall or(const type & a, const type & b)
		{
			return {_mm256_or_pd(a, b)};
		}

		static inline void __vectorcall andnot(const type & a, const type & b, type & out)
		{
			out = _mm256_andnot_pd(a, b);
		}

		static inline inner __vectorcall andnot(const type & a, const type & b)
		{
			return {_mm256_andnot_pd(a, b)};
		}

		static inline void __vectorcall xor(const type & a, const type & b, type & out)
		{
			out = _mm256_xor_pd(a, b);
		}

		static inline inner __vectorcall xor(const type & a, const type & b)
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
			out = andnot(signmask, a);
		}

		static inline inner __vectorcall abs(const type & a)
		{
			return {andnot(signmask, a)};
		}

		static inline void __vectorcall sign(const type & a, type & out)
		{
			out = and(signmask, a);
		}

		static inline inner __vectorcall sign(const type & a)
		{
			return {and(signmask, a)};
		}

		static inline void __vectorcall round(const type & a, type & out)
		{
			auto v = or(nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			out = xor(and(sub(add(a, v), v), mask), andnot(mask, a));
		}

		static inline inner __vectorcall round(const type & a)
		{
			auto v = or(nofrac, sign(a));
			auto mask = cmple(abs(a), nofrac);
			return {xor(and(sub(add(a, v), v), mask), andnot(mask, a))};
		}

		static inline void __vectorcall negate(const type & a, type & out)
		{
			out = xor(signmask, a);
		}

		static inline inner __vectorcall negate(const type & a)
		{
			return {xor(signmask, a)};
		}

		static inline void __vectorcall reverse(const type & a, type & out)
		{
			out = _mm256_reverse_pd(a);
		}

		static inline inner __vectorcall reverse(const type & a)
		{
			return _mm256_reverse_pd(a);
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)>
			endif>
		static inline void __vectorcall blend(const type & a, const type & b, type & out)
		{
			out = _mm256_blend_pd(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)>
			endif>
		static inline inner __vectorcall blend(const type & a, const type & b)
		{
			return _mm256_blend_pd(a, b, mk_mask4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle2(const type & a, const type & b, type & out)
		{
			out = blend<0, 0, 1, 1>(_mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D)), _mm256_permute4x64_pd(b, mk_shuffle_4(A, B, C, D)));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline inner __vectorcall shuffle2(const type & a, const type & b)
		{
			return blend<0, 0, 1, 1>(_mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D)), _mm256_permute4x64_pd(b, mk_shuffle_4(A, B, C, D)));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
		static inline void __vectorcall shuffle(const type & a, type & out)
		{
			out = _mm256_permute4x64_pd(a, mk_shuffle_4(A, B, C, D));
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)>
			endif>
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
	struct Intrinsic<double, 4>
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
	namespace Internals
	{
		template<class T, template<class T> class Constant, bool ... Values>
		struct intrinsic_mask0<T, Constant, std::integer_sequence<bool, Values...>>
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