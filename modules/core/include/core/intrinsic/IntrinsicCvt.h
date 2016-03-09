//---------------------------------------------------------------------------

#ifndef INTRINSIC_CVT_H
#define INTRINSIC_CVT_H

//---------------------------------------------------------------------------

#include <intrin.h>
#include <core/meta/Types.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T1, typename T2>
	struct IntrinsicCvt {};

	template<>
	struct IntrinsicCvt<__m128, __m128>
	{
		static inline void perform(const __m128 & in, __m128 & out)
		{
			out = in;
		}

		static inline void perform(__m128 && in, __m128 & out)
		{
			out = forward<__m128>(in);
		}
	};

	template<>
	struct IntrinsicCvt<__m128, __m128d>
	{
		static inline void perform(const __m128 & in, __m256d & out)
		{
			out = _mm256_cvtps_pd(in);
		}

		static inline void perform(__m128 && in, __m256d & out)
		{
			out = _mm256_cvtps_pd(forward<__m128>(in));
		}
	};

	template<>
	struct IntrinsicCvt<__m256d, __m128>
	{
		static inline void perform(const __m256d & in, __m128 & out)
		{
			out = _mm256_cvtpd_ps(in);
		}

		static inline void perform(__m256d && in, __m128 & out)
		{
			out = _mm256_cvtpd_ps(forward<__m256d>(in));
		}
	};

	template<>
	struct IntrinsicCvt<__m256d, __m256d>
	{
		static inline void perform(const __m256d & in, __m256d & out)
		{
			out = in;
		}

		static inline void perform(__m256d && in, __m256d & out)
		{
			out = forward<__m256d>(in);
		}
	};

	template<>
	struct IntrinsicCvt<__m256, __m256>
	{
		static inline void perform(const __m256 &in, __m256 & out)
		{
			out = in;
		}

		static inline void perform(__m256 && in, __m256 & out)
		{
			out = forward<__m256>(in);
		}
	};

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
			out = _mm256_set_m128(_mm256_cvtpd_ps(in[0]), _mm256_cvtpd_ps(in[1]));
		}

		static inline void perform(__m256d (&&in)[2], __m256 & out)
		{
			out = _mm256_set_m128(_mm256_cvtpd_ps(move(in[0])), _mm256_cvtpd_ps(move(in[1])));
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

		static inline void perform(__m256d (&&in)[2], __m256d (&out)[2])
		{
			out[0] = move(in[0]);
			out[1] = move(in[1]);
		}
	};

	template<typename A, typename B>
	inline void intin_cvt(A && in, B && out)
	{
		IntrinsicCvt<decay_t<A>, decay_t<B>>::perform(forward<A>(in), forward<B>(out));
	}
}

//---------------------------------------------------------------------------
#endif