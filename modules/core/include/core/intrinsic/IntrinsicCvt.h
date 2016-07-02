//---------------------------------------------------------------------------

#pragma once

#ifndef INTRINSIC_CVT_H
#define INTRINSIC_CVT_H

//---------------------------------------------------------------------------

#ifdef USE_AVX
#include <immintrin.h>
#else
#include <tmmintrin.h>
#endif

#include <meta/Types.h>

//---------------------------------------------------------------------------

namespace Rapture
{
#define mk_shuffle_2(a, b) (((b) << 1) | (a))
#define mk_shuffle_4(a, b, c, d) (((d) << 6) | ((c) << 4) | ((b) << 2) | (a))
#define mk_mask4(a, b, c, d) (((d) << 3) | ((c) << 2) | ((b) << 1) | (a))
#define reverse_shuffle_2 mk_shuffle_2(1, 0)
#define reverse_shuffle_4 mk_shuffle_4(3, 2, 1, 0)

	template<typename From, typename To>
	struct IntrinsicCvt {};

	template<>
	struct IntrinsicCvt<__m128, __m128>
	{
		static inline void perform(const __m128 & in, __m128 & out)
		{
			out = in;
		}
	};

	template<>
	struct IntrinsicCvt<__m128d[2], __m128>
	{
		static inline void perform(const __m128d (& in)[2], __m128 & out)
		{
			out = _mm_shuffle_ps(_mm_cvtpd_ps(in[0]), _mm_cvtpd_ps(in[1]), mk_shuffle_4(0, 1, 2, 3));
		}
	};

	template<>
	struct IntrinsicCvt<__m128, __m128d[2]>
	{
		static inline void perform(const __m128 & in, __m128d (& out)[2])
		{
			out[0] = _mm_cvtps_pd(in);
			out[1] = _mm_cvtps_pd(_mm_shuffle_ps(in, in, mk_shuffle_4(2, 3, 0, 1)));
		}
	};

	template<>
	struct IntrinsicCvt<__m128d[2], __m128d[2]>
	{
		static inline void perform(const __m128d (& in)[2], __m128d (& out)[2])
		{
			out[0] = in[0];
			out[1] = in[1];
		}
	};

//---------------------------------------------------------------------------

	inline float & _m_hi(__m64 & in)
	{
		return *(reinterpret_cast<float *>(&in) + 1);
	}

	inline const float & _m_hi(const __m64 & in)
	{
		return *(reinterpret_cast<const float *>(&in) + 1);
	}

	inline float & _m_lo(__m64 & in)
	{
		return *(reinterpret_cast<float *>(&in));
	}

	inline const float & _m_lo(const __m64 & in)
	{
		return *(reinterpret_cast<const float *>(&in));
	}

	inline __m64 & _mm_hi(__m128 & in)
	{
		return *(reinterpret_cast<__m64 *>(&in) + 1);
	}

	inline const __m64 & _mm_hi(const __m128 & in)
	{
		return *(reinterpret_cast<const __m64 *>(&in) + 1);
	}

	inline __m64 & _mm_lo(__m128 & in)
	{
		return *(reinterpret_cast<__m64 *>(&in));
	}

	inline const __m64 & _mm_lo(const __m128 & in)
	{
		return *(reinterpret_cast<const __m64 *>(&in));
	}

	inline double & _mm_hi(__m128d & in)
	{
		return *(reinterpret_cast<double *>(&in) + 1);
	}

	inline const double & _mm_hi(const __m128d & in)
	{
		return *(reinterpret_cast<const double *>(&in) + 1);
	}

	inline double & _mm_lo(__m128d & in)
	{
		return *(reinterpret_cast<double *>(&in));
	}

	inline const double & _mm_lo(const __m128d & in)
	{
		return *(reinterpret_cast<const double *>(&in));
	}

#ifdef USE_AVX

	inline __m128 & _mm256_hi(__m256 & in)
	{
		return *(reinterpret_cast<__m128 *>(&in) + 1);
	}

	inline const __m128 & _mm256_hi(const __m256 & in)
	{
		return *(reinterpret_cast<const __m128 *>(&in) + 1);
	}

	inline __m128 & _mm256_lo(__m256 & in)
	{
		return *(reinterpret_cast<__m128 *>(&in));
	}

	inline const __m128 & _mm256_lo(const __m256 & in)
	{
		return *(reinterpret_cast<const __m128 *>(&in));
	}

	inline __m128d & _mm256_hi(__m256d & in)
	{
		return *(reinterpret_cast<__m128d *>(&in) + 1);
	}

	inline const __m128d & _mm256_hi(const __m256d & in)
	{
		return *(reinterpret_cast<const __m128d *>(&in) + 1);
	}

	inline __m128d & _mm256_lo(__m256d & in)
	{
		return *(reinterpret_cast<__m128d *>(&in));
	}

	inline const __m128d & _mm256_lo(const __m256d & in)
	{
		return *(reinterpret_cast<const __m128d *>(&in));
	}

//---------------------------------------------------------------------------

	template<>
	struct IntrinsicCvt<__m128, __m256d>
	{
		static inline void perform(const __m128 & in, __m256d & out)
		{
			out = _mm256_cvtps_pd(in);
		}
	};

	template<>
	struct IntrinsicCvt<__m256d, __m128>
	{
		static inline void perform(const __m256d & in, __m128 & out)
		{
			out = _mm256_cvtpd_ps(in);
		}
	};

	template<>
	struct IntrinsicCvt<__m256d, __m256d>
	{
		static inline void perform(const __m256d & in, __m256d & out)
		{
			out = in;
		}
	};

	template<>
	struct IntrinsicCvt<__m256, __m256>
	{
		static inline void perform(const __m256 &in, __m256 & out)
		{
			out = in;
		}
	};

	template<>
	struct IntrinsicCvt<__m256, __m256d[2]>
	{
		static inline void perform(const __m256 & in, __m256d (&out)[2])
		{
			out[0] = _mm256_cvtps_pd(_mm256_lo(in));
			out[1] = _mm256_cvtps_pd(_mm256_hi(in));
		}
	};

	template<>
	struct IntrinsicCvt<__m256d[2], __m256>
	{
		static inline void perform(const __m256d (&in)[2], __m256 & out)
		{
#ifdef MSVC
			out = _mm256_set_m128(_mm256_cvtpd_ps(in[0]), _mm256_cvtpd_ps(in[1]));
#else
			_mm256_lo(out) = _mm256_cvtpd_ps(in[0]);
			_mm256_hi(out) = _mm256_cvtpd_ps(in[1]);
#endif // MSVC
		}
	};

	template<>
	struct IntrinsicCvt<__m256d[2], __m256d[2]>
	{
		static inline void perform(const __m256d (&in)[2], __m256d (&out)[2])
		{
			out[0] = in[0];
			out[1] = in[1];
		}
	};
#endif // USE_AVX

	template<typename A, typename B>
	inline void intrin_cvt(A && in, B & out)
	{
		IntrinsicCvt<remove_cv_ref_t<A>, remove_cv_ref_t<B>>::perform(forward<A>(in), out);
	}

	template<typename Out, typename In>
	inline Out intrin_cvt(In && in)
	{
		Out out;
		IntrinsicCvt<remove_cv_ref_t<In>, Out>::perform(forward<In>(in), out);
		return out;
	}
}

//---------------------------------------------------------------------------
#endif
